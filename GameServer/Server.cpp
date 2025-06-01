#include "Server.h"
#include "TexasHoldemRules.h"


Server::Server(int port)
    : ioContext(),
    acceptor(ioContext, tcp::endpoint(tcp::v4(), port)),
    playerManager(std::make_unique<PlayerManager>()),
    registrationHandler(std::make_unique<RegistrationHandler>()),
    connectionManager(std::make_unique<ConnectionManager>()),
    entryHandler(std::make_unique<EntryHandler>(playerManager.get()))
{
  
    auto tablePtr = std::make_unique<Table>(std::make_unique<Deck>());
    ITable* pTable = tablePtr.get();

    auto bettingSystemPtr = std::make_unique<BettingSystem>(pTable, 10); // мин размер ставки ---------------------------

    auto rulesPtr = std::make_unique<TexasHoldemRules>();

    serverGame = std::make_unique<ServerGame>(
        std::move(tablePtr),
        std::move(bettingSystemPtr),
        std::move(rulesPtr),
        [this](const std::string& msg) { this->broadcastMessage(msg); },
        [this](int playerID, const std::string& message) { this->sendMessageToPlayer(playerID, message); }
    );

    std::cout << "Server//\tСервер запущен на порту " << port << "! Ожидание игроков...\n";
}

//  Запуск сервера: начинаем принимать соединения и запускаем обработку сетевых событий
void Server::start() {
    acceptConnection();
    while (true) {
        try {
            ioContext.run();
        }
        catch (const std::exception& ex) {
            std::cerr << "Ошибка в ioContext: " << ex.what() << std::endl;
        }
        // Если ioContext.run() завершился – можно его перезапустить,
        // например, после небольшой задержки повторного запуска.
        std::this_thread::sleep_for(std::chrono::seconds(1));
        ioContext.restart();
    }
}

//  Ожидание новых подключений
void Server::acceptConnection() {
    auto socket = std::make_shared<tcp::socket>(ioContext);

    acceptor.async_accept(*socket, [this, socket](boost::system::error_code ec) {
        if (!ec) {
            std::cout << "Server//\tИгрок подключился!" << std::endl;
            sendMessage(socket, "Добро пожаловать на сервер!\n");
            // Проверяем, не существует ли уже сокет в activeConnections
            if (activeConnections.find(socket) == activeConnections.end()) {
                activeConnections.insert(socket);
                startRead(socket);
            }
        }
        else {
            std::cerr << "Server//\tОшибка подключения: " << ec.message() << std::endl;
        }

        acceptConnection(); 
        });
}

void Server::startRead(std::shared_ptr<tcp::socket> socket) {
    auto buffer = std::make_shared<boost::asio::streambuf>();
    auto readOperation = std::make_shared<std::function<void()>>();

    *readOperation = [this, socket, buffer, readOperation]() {
        boost::asio::async_read_until(*socket, *buffer, '\n',
            [this, socket, buffer, readOperation](boost::system::error_code ec, std::size_t bytesTransferred) {
                if (!ec) {
                    std::istream is(buffer.get());
                    std::string message;
                    std::getline(is, message);
                    buffer->consume(bytesTransferred);

                    if (message.empty()) {
                        std::cerr << "Server//\tОшибка: получено пустое сообщение!" << std::endl;
                        return;
                    }

                    std::cout << "Server//\tИгрок отправил: " << message << std::endl;
                    handleMessage(socket, message);

                    if (socket->is_open() && activeConnections.find(socket) != activeConnections.end() &&
                        !serverGame->isRoundCancelled())
                    {
                        std::cout << "Server//\tОжидание следующего сообщения для сокета: " << socket.get() << std::endl;
                        (*readOperation)();
                    }
                }
                else {
                    std::cerr << "Server//\tОшибка получения данных: " << ec.message() << std::endl;
                    handleDisconnect(socket);
                }
            });
        };

    (*readOperation)();
}



void Server::broadcastGameState() {
    std::cout << "Server//\tЗапрос состояния стола..." << std::endl;
    serverGame->broadcastGameState();
}

void Server::sendMessageToPlayer(int playerID, const std::string& message) {
    connectionManager->sendData(playerID, message);
}

void Server::registerPlayerConnection(int playerID, std::shared_ptr<tcp::socket> socket) {
    connectionManager->registerConnection(playerID, socket);
    socketToPlayerID.erase(socket);
    socketToPlayerID[socket] = playerID;
    activeConnections.insert(socket);
    startRead(socket);
    std::cout << "Server//\tИгрок ID " << playerID << " теперь привязан к сокету." << std::endl;
}

void Server::handleMessage(std::shared_ptr<tcp::socket> socket, const std::string& message) {
    std::string trimmedMessage = message; // Можно добавить trim, если требуется
    std::string response;

    if (trimmedMessage.rfind("REGISTRATION ", 0) == 0) {
        std::string data = trimmedMessage.substr(13);
        response = registrationHandler->processRegistration(data);
    }
    else if (trimmedMessage.rfind("ENTRY ", 0) == 0) {
        std::string data = trimmedMessage.substr(6);
        std::string entryResponse = entryHandler->processEntry(data);

        if (entryResponse.rfind("Вход успешен!", 0) != 0) {
            response = entryResponse; // ошибка входа
        }
        else {

            
            size_t startPos = entryResponse.find("! ") + 2;
            std::string playerData = entryResponse.substr(startPos);
            size_t balanceDelimiter = playerData.find(":");

            std::string username = playerData.substr(0, balanceDelimiter);
            int balance = std::stoi(playerData.substr(balanceDelimiter + 1));
            int playerID = playerManager->getNextPlayerID();
            auto newPlayer = std::make_shared<Player>(username, balance, playerID);


            if (playerManager->IsPlayerExist(username)) {
                response = "Ошибка: Игрок с таким именем уже подключён!\n";
                sendMessage(socket, response);
                return;
            }

            if (playerManager->addPlayer(newPlayer)) {
                response = "Вход успешен! TABLE\n";
                sendMessage(socket, response);
                registerPlayerConnection(playerID, socket);
                auto gamePlayer = playerManager->getPlayer(playerID);
                if (gamePlayer) {
                    gamePlayer->SetActive(false);
                    serverGame->addPlayer(gamePlayer);
                }
                else {
                    std::cerr << "Server//\tОшибка: Не удалось получить игрока для добавления в serverGame!\n";
                }
                std::cout << "Server//\tИгрок ID " << playerID << " добавлен и привязан к сокету." << std::endl;
                // Не отправляем дополнительный ответ далее.
                return;
            }
            else {
                response = "Ошибка: игра заполнена!\n";
            }
        }
    }
    else if (trimmedMessage.rfind("START_GAME", 0) == 0) {
        response = "Игра запущена!\n";
    }
    else if (trimmedMessage.rfind("ACTIONS: ", 0) == 0) {
        // Если сообщение начинается с "ACTIONS:", затем обрабатываем как ответ игрока.
      
        std::string actionPrefix = "ACTIONS: ";
        std::string processedMessage = trimmedMessage.substr(actionPrefix.size());

       // std::cout << "AAAAAAAAAAAAAAAAAAAAAAA" << processedMessage << " SSSSSSSSSSSSSSSSSSSSSSSS" << std::endl;
        int playerID = connectionManager->getPlayerIDBySocket(socket);
        if (playerID != -1) {
            serverGame->processPlayerAction(playerID, processedMessage);
        }
        else {
            std::cerr << "Ошибка: не удалось найти игрока по сокету!" << std::endl;
        }
        // Не отправляем ответ обратно.
        return;
    }
    else if (trimmedMessage.rfind("EXIT", 0) == 0) {
        handleDisconnect(socket);
        return;
    }
    else {
        response = "Неизвестная команда: " + trimmedMessage + "\n";
    }

    sendMessage(socket, response);
}


void Server::sendMessage(std::shared_ptr<tcp::socket> socket, const std::string& message) {
    // Если сокет закрыт, выходим сразу
    if (!socket->is_open()) {
        std::cerr << "Ошибка: сокет закрыт, команда не отправлена.\n";
        return;
    }

    auto safeResponse = std::make_shared<std::string>(message);
    boost::asio::async_write(*socket, boost::asio::buffer(*safeResponse),
        [this, socket, safeResponse](boost::system::error_code ec, std::size_t /*bytesTransferred*/) {
            if (ec) {
                std::cerr << "Ошибка отправки данных: " << ec.message() << std::endl;
                // Закрываем и удаляем сокет
                handleDisconnect(socket);
            }
            else {
                std::cout << "Ответ отправлен клиенту: " << *safeResponse << std::endl;
            }
        });
}



void Server::broadcastMessage(const std::string& message) {
    std::cout << "Server//\tОтвет отправлен клиенту: " << message << std::endl;
    for (auto& socket : activeConnections) {
        sendMessage(socket, message);
    }
}
void Server::handleDisconnect(std::shared_ptr<tcp::socket> socket) {
    // Корректное закрытие сокета
    try {
        if (socket->is_open()) {
            boost::system::error_code ec;
            socket->shutdown(tcp::socket::shutdown_both, ec);
            if (ec) {
                std::cerr << "Ошибка при shutdown сокета: " << ec.message() << std::endl;
            }
            socket->cancel(ec); // Отменяем все ожидающие операции
            socket->close(ec);
            if (ec) {
                std::cerr << "Ошибка при закрытии сокета: " << ec.message() << std::endl;
            }
        }
    }
    catch (const std::exception& ex) {
        std::cerr << "Исключение при закрытии сокета: " << ex.what() << std::endl;
    }

    auto it = socketToPlayerID.find(socket);
    if (it == socketToPlayerID.end()) {
        std::cerr << "Ошибка: отключился неизвестный сокет.\n";
        return;
    }

    int playerID = it->second;
    std::cout << "Игрок с ID " << playerID << " отключился." << std::endl;
    
    std::string username = playerManager->getPlayer(playerID)->GetName(); // Получаем имя игрока
    int balance = playerManager->getPlayer(playerID)->GetBalance(); // Получаем его текущий баланс

    //  Обновляем баланс игрока в базе
    registrationHandler->updateBalance(username, balance);

    try {


        // Удаляем игрока из PlayerManager
        auto player = playerManager->getPlayer(playerID);
        if (player) {
            playerManager->removePlayer(playerID);
            std::cout << "Игрок с ID " << playerID << " удалён из PlayerManager.\n";
        }
        else {
            std::cerr << "Ошибка: игрок с ID " << playerID << " не найден.\n";
        }


        // Удаляем запись из ConnectionManager
        connectionManager->handleDisconnect(playerID);
        std::cout << "Игрок с ID " << playerID << " удалён из ConnectionManager.\n";

        if (serverGame) {
            serverGame->removePlayer(playerID);
            std::cout << "Игрок с ID " << playerID << " удалён из ServerGame.\n";
        }
        else {
            std::cerr << "Ошибка: серверная логика не инициализирована.\n";
        }

        // Удаляем сокет из контейнера mapping сокет->ID
        socketToPlayerID.erase(it);
        // Удаляем сокет из множества activeConnections
        activeConnections.erase(socket);
    }
    catch (const std::exception& e) {
        std::cerr << "Ошибка при обработке разрыва соединения: " << e.what() << std::endl;
    }

}
