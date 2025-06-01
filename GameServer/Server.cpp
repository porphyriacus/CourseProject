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

    auto bettingSystemPtr = std::make_unique<BettingSystem>(pTable, 10); // ��� ������ ������ ---------------------------

    auto rulesPtr = std::make_unique<TexasHoldemRules>();

    serverGame = std::make_unique<ServerGame>(
        std::move(tablePtr),
        std::move(bettingSystemPtr),
        std::move(rulesPtr),
        [this](const std::string& msg) { this->broadcastMessage(msg); },
        [this](int playerID, const std::string& message) { this->sendMessageToPlayer(playerID, message); }
    );

    std::cout << "Server//\t������ ������� �� ����� " << port << "! �������� �������...\n";
}

//  ������ �������: �������� ��������� ���������� � ��������� ��������� ������� �������
void Server::start() {
    acceptConnection();
    while (true) {
        try {
            ioContext.run();
        }
        catch (const std::exception& ex) {
            std::cerr << "������ � ioContext: " << ex.what() << std::endl;
        }
        // ���� ioContext.run() ���������� � ����� ��� �������������,
        // ��������, ����� ��������� �������� ���������� �������.
        std::this_thread::sleep_for(std::chrono::seconds(1));
        ioContext.restart();
    }
}

//  �������� ����� �����������
void Server::acceptConnection() {
    auto socket = std::make_shared<tcp::socket>(ioContext);

    acceptor.async_accept(*socket, [this, socket](boost::system::error_code ec) {
        if (!ec) {
            std::cout << "Server//\t����� �����������!" << std::endl;
            sendMessage(socket, "����� ���������� �� ������!\n");
            // ���������, �� ���������� �� ��� ����� � activeConnections
            if (activeConnections.find(socket) == activeConnections.end()) {
                activeConnections.insert(socket);
                startRead(socket);
            }
        }
        else {
            std::cerr << "Server//\t������ �����������: " << ec.message() << std::endl;
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
                        std::cerr << "Server//\t������: �������� ������ ���������!" << std::endl;
                        return;
                    }

                    std::cout << "Server//\t����� ��������: " << message << std::endl;
                    handleMessage(socket, message);

                    if (socket->is_open() && activeConnections.find(socket) != activeConnections.end() &&
                        !serverGame->isRoundCancelled())
                    {
                        std::cout << "Server//\t�������� ���������� ��������� ��� ������: " << socket.get() << std::endl;
                        (*readOperation)();
                    }
                }
                else {
                    std::cerr << "Server//\t������ ��������� ������: " << ec.message() << std::endl;
                    handleDisconnect(socket);
                }
            });
        };

    (*readOperation)();
}



void Server::broadcastGameState() {
    std::cout << "Server//\t������ ��������� �����..." << std::endl;
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
    std::cout << "Server//\t����� ID " << playerID << " ������ �������� � ������." << std::endl;
}

void Server::handleMessage(std::shared_ptr<tcp::socket> socket, const std::string& message) {
    std::string trimmedMessage = message; // ����� �������� trim, ���� ���������
    std::string response;

    if (trimmedMessage.rfind("REGISTRATION ", 0) == 0) {
        std::string data = trimmedMessage.substr(13);
        response = registrationHandler->processRegistration(data);
    }
    else if (trimmedMessage.rfind("ENTRY ", 0) == 0) {
        std::string data = trimmedMessage.substr(6);
        std::string entryResponse = entryHandler->processEntry(data);

        if (entryResponse.rfind("���� �������!", 0) != 0) {
            response = entryResponse; // ������ �����
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
                response = "������: ����� � ����� ������ ��� ���������!\n";
                sendMessage(socket, response);
                return;
            }

            if (playerManager->addPlayer(newPlayer)) {
                response = "���� �������! TABLE\n";
                sendMessage(socket, response);
                registerPlayerConnection(playerID, socket);
                auto gamePlayer = playerManager->getPlayer(playerID);
                if (gamePlayer) {
                    gamePlayer->SetActive(false);
                    serverGame->addPlayer(gamePlayer);
                }
                else {
                    std::cerr << "Server//\t������: �� ������� �������� ������ ��� ���������� � serverGame!\n";
                }
                std::cout << "Server//\t����� ID " << playerID << " �������� � �������� � ������." << std::endl;
                // �� ���������� �������������� ����� �����.
                return;
            }
            else {
                response = "������: ���� ���������!\n";
            }
        }
    }
    else if (trimmedMessage.rfind("START_GAME", 0) == 0) {
        response = "���� ��������!\n";
    }
    else if (trimmedMessage.rfind("ACTIONS: ", 0) == 0) {
        // ���� ��������� ���������� � "ACTIONS:", ����� ������������ ��� ����� ������.
      
        std::string actionPrefix = "ACTIONS: ";
        std::string processedMessage = trimmedMessage.substr(actionPrefix.size());

       // std::cout << "AAAAAAAAAAAAAAAAAAAAAAA" << processedMessage << " SSSSSSSSSSSSSSSSSSSSSSSS" << std::endl;
        int playerID = connectionManager->getPlayerIDBySocket(socket);
        if (playerID != -1) {
            serverGame->processPlayerAction(playerID, processedMessage);
        }
        else {
            std::cerr << "������: �� ������� ����� ������ �� ������!" << std::endl;
        }
        // �� ���������� ����� �������.
        return;
    }
    else if (trimmedMessage.rfind("EXIT", 0) == 0) {
        handleDisconnect(socket);
        return;
    }
    else {
        response = "����������� �������: " + trimmedMessage + "\n";
    }

    sendMessage(socket, response);
}


void Server::sendMessage(std::shared_ptr<tcp::socket> socket, const std::string& message) {
    // ���� ����� ������, ������� �����
    if (!socket->is_open()) {
        std::cerr << "������: ����� ������, ������� �� ����������.\n";
        return;
    }

    auto safeResponse = std::make_shared<std::string>(message);
    boost::asio::async_write(*socket, boost::asio::buffer(*safeResponse),
        [this, socket, safeResponse](boost::system::error_code ec, std::size_t /*bytesTransferred*/) {
            if (ec) {
                std::cerr << "������ �������� ������: " << ec.message() << std::endl;
                // ��������� � ������� �����
                handleDisconnect(socket);
            }
            else {
                std::cout << "����� ��������� �������: " << *safeResponse << std::endl;
            }
        });
}



void Server::broadcastMessage(const std::string& message) {
    std::cout << "Server//\t����� ��������� �������: " << message << std::endl;
    for (auto& socket : activeConnections) {
        sendMessage(socket, message);
    }
}
void Server::handleDisconnect(std::shared_ptr<tcp::socket> socket) {
    // ���������� �������� ������
    try {
        if (socket->is_open()) {
            boost::system::error_code ec;
            socket->shutdown(tcp::socket::shutdown_both, ec);
            if (ec) {
                std::cerr << "������ ��� shutdown ������: " << ec.message() << std::endl;
            }
            socket->cancel(ec); // �������� ��� ��������� ��������
            socket->close(ec);
            if (ec) {
                std::cerr << "������ ��� �������� ������: " << ec.message() << std::endl;
            }
        }
    }
    catch (const std::exception& ex) {
        std::cerr << "���������� ��� �������� ������: " << ex.what() << std::endl;
    }

    auto it = socketToPlayerID.find(socket);
    if (it == socketToPlayerID.end()) {
        std::cerr << "������: ���������� ����������� �����.\n";
        return;
    }

    int playerID = it->second;
    std::cout << "����� � ID " << playerID << " ����������." << std::endl;
    
    std::string username = playerManager->getPlayer(playerID)->GetName(); // �������� ��� ������
    int balance = playerManager->getPlayer(playerID)->GetBalance(); // �������� ��� ������� ������

    //  ��������� ������ ������ � ����
    registrationHandler->updateBalance(username, balance);

    try {


        // ������� ������ �� PlayerManager
        auto player = playerManager->getPlayer(playerID);
        if (player) {
            playerManager->removePlayer(playerID);
            std::cout << "����� � ID " << playerID << " ����� �� PlayerManager.\n";
        }
        else {
            std::cerr << "������: ����� � ID " << playerID << " �� ������.\n";
        }


        // ������� ������ �� ConnectionManager
        connectionManager->handleDisconnect(playerID);
        std::cout << "����� � ID " << playerID << " ����� �� ConnectionManager.\n";

        if (serverGame) {
            serverGame->removePlayer(playerID);
            std::cout << "����� � ID " << playerID << " ����� �� ServerGame.\n";
        }
        else {
            std::cerr << "������: ��������� ������ �� ����������������.\n";
        }

        // ������� ����� �� ���������� mapping �����->ID
        socketToPlayerID.erase(it);
        // ������� ����� �� ��������� activeConnections
        activeConnections.erase(socket);
    }
    catch (const std::exception& e) {
        std::cerr << "������ ��� ��������� ������� ����������: " << e.what() << std::endl;
    }

}
