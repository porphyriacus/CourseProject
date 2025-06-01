#include "ServerGame.h"

#include <exception>

class RoundCancelledException : public std::exception {
public:
    const char* what() const noexcept override {
        return "Round Cancelled";
    }
};


//   • broadcastCallback_ – для рассылки сообщений всем игрокам
//   • sendMessageToPlayerCallback_ – для отправки персональных сообщений

ServerGame::ServerGame(std::unique_ptr<ITable> table,
        std::unique_ptr<BettingSystem> bettingSystem,
        std::unique_ptr<TexasHoldemRules> rules,
        std::function<void(const std::string&)> broadcastCallback,
        std::function<void(int, const std::string&)> sendMessageToPlayerCallback)
        : table_(std::move(table)),
        bettingSystem_(std::move(bettingSystem)),
        rules_(std::move(rules)),
        broadcastCallback_(broadcastCallback),
        sendMessageToPlayerCallback_(sendMessageToPlayerCallback),
        gameStarted_(false),
        stopRequested_(false),
        timerResetRequested_(false)
    {
        // Используем PreFlopRound как первый раунд.
       
        rounds_.push_back(std::make_unique<PreFlopRound>());
        timerThread_ = std::thread(&ServerGame::timerLoop, this); // создаем отдельны игровой поток 
    }
     void ServerGame::stop() {
        stopRequested_ = true;
        timerCv_.notify_all();
        if (timerThread_.joinable())
            timerThread_.join();
        if (gameThread_.joinable())
            gameThread_.join();
    }
     ServerGame::~ServerGame() {
        stop();
    }

    // Метод, который сервер вызывает при получении действия от игрока (например, через сетевой компонент).
    // Действие добавляется в очередь для обработки в bettingRound.
    void ServerGame::processPlayerAction(int playerID, const std::string& action) {
        {
            std::lock_guard<std::mutex> lock(actionsMutex_);
            actionsQueue_.push_back({ playerID, action });
        }
        actionsCv_.notify_all();
    }

    // Добавление нового игрока
    void ServerGame::addPlayer(std::shared_ptr<IPlayer> player) {
        std::lock_guard<std::mutex> lock(gameMutex_);
        auto players = table_->getPlayers();
        if (std::any_of(players.begin(), players.end(),
            [&](const std::shared_ptr<IPlayer>& p) { return p->GetID() == player->GetID(); })) {
            std::cerr << "Ошибка: Игрок с ID " << player->GetID() << " уже существует!\n";
            return;
        }
        if (players.size() < 5) {
            if (!gameStarted_) {
                std::cout << " -----------11111111111111111111111111111111111111111111" << std::endl;
                player->SetActive(true);
            }
            else {
                player->SetActive(false);
            }
            std::string isAct = (player->IsActive()) ? "Игрок может учавствовать в игре" : "Игрок добавлен, необходимо дождаться конца текущей игры";
            std::cout<< isAct << std::endl;
            table_->addPlayer(player);


            if (!gameStarted_) {
                player->SetActive(true);
                std::lock_guard<std::mutex> lock(timerMutex_);
                timerResetRequested_ = true;
                timerCv_.notify_one();
                broadcastState();
               
            }
            broadcastState();
        }
        else {
            broadcastCallback_("Игра заполнена. Игрок " + player->GetName() + " не может быть добавлен.\n");
        }
    }
 
    void ServerGame::removePlayer(int playerID) {
        std::lock_guard<std::mutex> lock(gameMutex_);
        table_->removePlayer(playerID);
        auto players = table_->getPlayers();
        if (!players.empty()) {
            broadcastState();
            broadcastCallback_("Игрок с ID " + std::to_string(playerID) + " покинул игру.\n");
            if (table_->getActivePlayers().size() < 2) {
                cancelRound();
            }
        }
        else {
            broadcastCallback_("Ошибка: после удаления игроков стол пуст!\n");
        }
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    void ServerGame::cancelRound() {
        roundCancelled_ = true; // Сигнализируем о необходимости отмены
        // Уведомляем любые ожидания (например, в commonBettingRound, где ожидается ответ)
        actionsCv_.notify_all();
    }
    void ServerGame::checkActivePlayerCount() {
        auto activePlayers = table_->getActivePlayers();
        if (activePlayers.size() < 2) {
            // Если активных меньше двух, сразу отменяем раунд
            cancelRound();
        }
    }


    void ServerGame::sendTableCards() {
        auto cards = table_->getCommunityCards();
        std::ostringstream oss;
        oss << "TABLE_CARDS: ";
        for (const auto& card : cards) {
            oss << card->getCardID() << ": ";
        }
        oss << '\n';
        std::string message = oss.str();
        broadcastCallback_(message);
    }
    // Отправка карт (карманных) каждому игроку с их текстовыми идентификаторами.
    void ServerGame::sendPlayerCards() {
        auto players = table_->getActivePlayers();
        for (const auto& player : players) {
            std::ostringstream oss;
            oss << "HANDS_CARDS: ";
            oss << player->GetHand();
            oss << '\n';
            std::string message = oss.str();
            sendMessageToPlayerCallback_(player->GetID(), message);
        }
    }
    // Вывод состояния стола: формируется строка с именами игроков, балансами и статусами.
    void ServerGame::broadcastState() {
        std::ostringstream oss;
        oss << "Состояние стола:";

        auto players = table_->getPlayers();
        if (players.empty()) {
            std::cerr << "Ошибка: стол пуст! Нет игроков!\n";
            return;
        }

        for (const auto& p : players) { //  Теперь `p` - shared_ptr
            oss << "Игрок ID " << p->GetID() << ": " << p->GetName()
                << ", Баланс:" << p->GetBalance()
                << ", Статус: " << (p->IsActive() ? "Активен" : "Неактивен") << "|";
        }

        oss << '\n';
        std::string gameState = oss.str();
        std::cout << "Отправка состояния: " << gameState << std::endl;
        broadcastCallback_(gameState);
    }
    void ServerGame::sendActions(std::unordered_map<std::shared_ptr<IPlayer>, int>& roundContributions, int currentBet, std::shared_ptr<IPlayer> pl) {

        std::ostringstream oss;
        int pid = pl->GetID();
        int plcont = roundContributions[pl];
        oss << "ВАШ ХОД: Текущая обязательная ставка = " << std::to_string(currentBet);
        oss << ", Ваш вклад = " << std::to_string(plcont);
        oss << ". Действия: CHECK, ";

        // Рассчитываем минимальный и максимальный рейз:
        int minRaise = currentBet; // минимальное повышение начинается от текущей ставки
        int maxRaise = currentBet + pl->GetBalance(); // стандартно максимум = текущая ставка + баланс

        // Если ровно два активных игрока, ограничиваем максимум до (currentBet + минимальный баланс)
        auto activePlayers = table_->getActivePlayers();
        if (activePlayers.size() == 2) {
            int minBalance = bettingSystem_->getMinBet(); // либо, если функция getMinActivePlayerBalance() доступна,
            // используем её для определения минимального баланса
            maxRaise = currentBet + minBalance;
        }

        // Если игрок может сделать CALL, добавляем опции CALL и RAISE с указанным диапазоном
        if (pl->CanBet(currentBet - plcont)) {
            oss << "CALL, RAISE <" << std::to_string(minRaise) << ", " << std::to_string(maxRaise) << ">, ";
        }
        oss << "FOLD.\n";

        std::string message = oss.str();
        std::cout << "Отправка действий игроку " << pid << ", ставка = " << currentBet
            << ", вклад = " << roundContributions[pl] << std::endl;
        sendMessageToPlayerCallback_(pid, message);
        std::cout << "ServerGame//\tMessage was sent to player " << pid << "\n" << std::endl;

    }
 

    void ServerGame::handlePlayerAction(int pid, const std::string& action) {
        {
            std::lock_guard<std::mutex> lock(actionsMutex_);
            actionsQueue_.emplace_back(pid, action);  // Добавляем действие в очередь
        }
        actionsCv_.notify_all();  // Сообщаем серверу, что появилось новое действие
    }


    
    void ServerGame::ccommonBettingRound(std::vector<std::shared_ptr<IPlayer>>& activePlayers,
        std::unordered_map<std::shared_ptr<IPlayer>, int>& roundContributions) {
        //начальное значение обязательной ставки.
        int currentBet = bettingSystem_->getCurrentBet();
        bool roundInProgress = true;
        while (roundInProgress) {
            // Проверка состояния активных игроков:
            checkActivePlayerCount();
            if (roundCancelled_) {
                throw RoundCancelledException();
            }
            roundInProgress = false;  // предполагаем, что ставки сравнялись

            activePlayers = table_->getActivePlayers();
            if (activePlayers.size() < 2) {
                return;
            }
            for (auto& player : activePlayers) {
                if (!player->IsActive()) continue;
                if (roundContributions[player] < currentBet) {
                    roundInProgress = true;
                    int pid = player->GetID();
                    sendActions(roundContributions, currentBet, player);
                    broadcastCallback_("ACTION_EXPECTED_FROM: " + std::to_string(pid) + ".\n");

                    bool responded = false;

                    std::string action;
                    {
                        std::unique_lock<std::mutex> lock(actionsMutex_);
                        responded = actionsCv_.wait_for(lock, std::chrono::seconds(30),
                            [this, pid]() -> bool {
                                return std::any_of(actionsQueue_.begin(), actionsQueue_.end(),
                                    [pid](const std::pair<int, std::string>& act) {
                                        return act.first == pid;
                                    });
                            });
                        if (responded) {
                            auto it = std::find_if(actionsQueue_.begin(), actionsQueue_.end(),
                                [pid](const std::pair<int, std::string>& act) { return act.first == pid; });
                            if (it != actionsQueue_.end()) {
                                action = it->second;
                                actionsQueue_.erase(it);
                            }
                        }
                    }

                    // Проверяем состояние снова
                    checkActivePlayerCount();
                    if (roundCancelled_) { throw RoundCancelledException(); }

                    if (!player->IsActive()) continue;
                    if (!responded) {
                        broadcastCallback_(player->GetName() + " не ответил вовремя и выбыл.\n");
                        bettingSystem_->fold(player);
                        player->SetActive(false);
                        checkActivePlayerCount();
                        if (table_->getActivePlayers().size() < 2) return;
                        continue;
                    }
                    if (action.find("FOLD") == 0) {
                        broadcastCallback_("MOVE: " + player->GetName() + " выбрал FOLD.\n");
                        bettingSystem_->fold(player);
                        player->SetActive(false);

                        if (table_->getActivePlayers().size() < 2) return;
                    }
                    else if (action.find("CALL") == 0 || action.find("CHECK") == 0) {
                        int diff = currentBet - roundContributions[player];
                        roundContributions[player] += diff;
                        bettingSystem_->call(player);
                        broadcastCallback_("MOVE: " + player->GetName() + " делает " +
                            (diff > 0 ? "CALL" : "CHECK") + ".\n");
                    }
                    else if (action.find("RAISE") == 0 || action.find("BET") == 0) {
                        try {
                            size_t spacePos = action.find(" ");
                            if (spacePos == std::string::npos)
                                throw std::invalid_argument("Неверный формат команды RAISE/BET.");
                            int newBet = std::stoi(action.substr(spacePos + 1));
                            if (newBet <= currentBet) {
                                sendMessageToPlayerCallback_(pid, "Ставка должна быть больше текущей (" + std::to_string(currentBet) + ").\n");
                                roundInProgress = true;
                                continue;
                            }
                            int diff = currentBet - roundContributions[player];
                            roundContributions[player] += diff + (newBet - currentBet);
                            bettingSystem_->raise(player, diff + (newBet - currentBet));
                            broadcastCallback_("MOVE: " + player->GetName() + " поднимает ставку до " + std::to_string(newBet) + ".\n");
                            currentBet = newBet;
                        }
                        catch (const std::exception& ex) {
                            sendMessageToPlayerCallback_(pid, std::string("Ошибка обработки рейза: ") + ex.what() + "\n");
                            roundInProgress = true;
                            continue;
                        }
                    }
                    else {
                        sendMessageToPlayerCallback_(pid, "Неверное действие. Попробуйте ещё раз.\n");
                        roundInProgress = true;
                        continue;
                    }
                }
                // Пауза после каждой итерации по игроку
                std::this_thread::sleep_for(std::chrono::seconds(2));
            }
            broadcastState();
        }
        broadcastCallback_("CURRENT_BET: " + std::to_string(currentBet) +
            ". POT: " + std::to_string(bettingSystem_->getPot()) + "\n");
    }


    void ServerGame::preflopBettingRound(std::vector<std::shared_ptr<IPlayer>>& activePlayers,
        std::unordered_map<std::shared_ptr<IPlayer>, int>& roundContributions) {
        // Проверка количества игроков

        activePlayers = table_->getActivePlayers();
        if (activePlayers.size() < 2) {
            std::cerr << "Ошибка: недостаточно игроков для префлопа! Размер = " << activePlayers.size() << std::endl;
            return;
        }

        std::shared_ptr<IPlayer> smallBlindPlayer = activePlayers[0];
        std::shared_ptr<IPlayer> bigBlindPlayer = activePlayers[1];
       

        std::cout << '\n' << smallBlindPlayer->GetBalance() << '\n'<< bigBlindPlayer->GetBalance() <<std::endl;
        // Проверка существования игроков
        if (!smallBlindPlayer || !bigBlindPlayer) {
            std::cerr << "Ошибка: Игроки на блайндах имеют `nullptr`!" << std::endl;
            return;
        }

        int smallBlind = bettingSystem_->getSmallBlind();

        std::cout << '\n' << smallBlind<< '\n'  << std::endl;


        bettingSystem_->placeBet(smallBlindPlayer, smallBlind);
        roundContributions[smallBlindPlayer] = smallBlind;
        broadcastCallback_("MOVE: " + smallBlindPlayer->GetName() + " ставит МАЛЫЙ БЛАЙНД: " + std::to_string(smallBlind) + "\n");
        broadcastState();
        std::this_thread::sleep_for(std::chrono::seconds(1));
        int bigBlind = bettingSystem_->getBigBlind();
        bettingSystem_->placeBet(bigBlindPlayer, bigBlind);
        roundContributions[bigBlindPlayer] = bigBlind;
        broadcastCallback_("MOVE: " + bigBlindPlayer->GetName() + " ставит БОЛЬШОЙ БЛАЙНД: " + std::to_string(bigBlind) + "\n");
        broadcastState();
        std::this_thread::sleep_for(std::chrono::seconds(3));

    }

    void ServerGame::commonBettingRound(std::vector<std::shared_ptr<IPlayer>>& activePlayers,
        std::unordered_map<std::shared_ptr<IPlayer>, int>& roundContributions) {
        int currentBet = bettingSystem_->getCurrentBet();
        bool firstRound = true;
        bool roundInProgress = true;
        while (roundInProgress) {
            // Проверка состояния активных игроков:
            checkActivePlayerCount();
            if (roundCancelled_) {
                throw RoundCancelledException();
            }
            roundInProgress = false;  // предполагаем, что ставки сравнялись

            activePlayers = table_->getActivePlayers();
            if (activePlayers.size() < 2) {
                return;
            }
            for (auto& player : activePlayers) {
                if (!player->IsActive()) continue;
                if (roundContributions[player] < currentBet || firstRound) {
                    roundInProgress = true;
                    int pid = player->GetID();
                    sendActions(roundContributions, currentBet, player);
                    broadcastCallback_("ACTION_EXPECTED_FROM: " + std::to_string(pid) + ".\n");

                    bool responded = false;

                    std::string action;
                    {
                        std::unique_lock<std::mutex> lock(actionsMutex_);
                        responded = actionsCv_.wait_for(lock, std::chrono::seconds(30),
                            [this, pid]() -> bool {
                                return std::any_of(actionsQueue_.begin(), actionsQueue_.end(),
                                    [pid](const std::pair<int, std::string>& act) {
                                        return act.first == pid;
                                    });
                            });
                        if (responded) {
                            auto it = std::find_if(actionsQueue_.begin(), actionsQueue_.end(),
                                [pid](const std::pair<int, std::string>& act) { return act.first == pid; });
                            if (it != actionsQueue_.end()) {
                                action = it->second;
                                actionsQueue_.erase(it);
                            }
                        }
                    }

                    // Проверяем состояние снова
                    checkActivePlayerCount();
                    if (roundCancelled_) { throw RoundCancelledException(); }

                    if (!player->IsActive()) continue;
                    if (!responded) {
                        broadcastCallback_(player->GetName() + " не ответил вовремя и выбыл.\n");
                        bettingSystem_->fold(player);
                        player->SetActive(false);
                        checkActivePlayerCount();
                        if (table_->getActivePlayers().size() < 2) return;
                        continue;
                    }
                    if (action.find("FOLD") == 0) {
                        broadcastCallback_("MOVE: " + player->GetName() + " выбрал FOLD.\n");
                        bettingSystem_->fold(player);
                        player->SetActive(false);
                        checkActivePlayerCount();
                        if (table_->getActivePlayers().size() < 2) return;
                    }
                    else if (action.find("CALL") == 0 || action.find("CHECK") == 0) {
                        int diff = currentBet - roundContributions[player];
                        roundContributions[player] += diff;
                        bettingSystem_->call(player);
                        broadcastCallback_("MOVE: " + player->GetName() + " делает " +
                            (diff > 0 ? "CALL" : "CHECK") + ".\n");
                    }
                    else if (action.find("RAISE") == 0 || action.find("BET") == 0) {
                        try {
                            size_t spacePos = action.find(" ");
                            if (spacePos == std::string::npos)
                                throw std::invalid_argument("Неверный формат команды RAISE/BET.");
                            int newBet = std::stoi(action.substr(spacePos + 1));
                            if (newBet <= currentBet) {
                                sendMessageToPlayerCallback_(pid, "Ставка должна быть больше текущей (" + std::to_string(currentBet) + ").\n");
                                roundInProgress = true;
                                continue;
                            }
                            int diff = currentBet - roundContributions[player];
                            roundContributions[player] += diff + (newBet - currentBet);
                            bettingSystem_->raise(player, diff + (newBet - currentBet));
                            broadcastCallback_("MOVE: " + player->GetName() + " поднимает ставку до " + std::to_string(newBet) + ".\n");
                            currentBet = newBet;
                        }
                        catch (const std::exception& ex) {
                            sendMessageToPlayerCallback_(pid, std::string("Ошибка обработки рейза: ") + ex.what() + "\n");
                            roundInProgress = true;
                            continue;
                        }
                    }
                    else {
                        sendMessageToPlayerCallback_(pid, "Неверное действие. Попробуйте ещё раз.\n");
                        roundInProgress = true;
                        continue;
                    }
                }
                // Пауза после каждой итерации по игроку
                std::this_thread::sleep_for(std::chrono::seconds(2));
            }
            broadcastState();
            firstRound = false;
        }
        broadcastCallback_("CURRENT_BET: " + std::to_string(currentBet) +
            ". POT: " + std::to_string(bettingSystem_->getPot()) + "\n");
    }



    /*
    // 2. Метод для общего раунда ставок (подходит как для префлопа после блайндов, так и для остальных улиц)
    //    Цикл продолжается до тех пор, пока хотя бы у одного активного игрока вклад меньше текущей обязательной ставки.
    void ServerGame::commonBettingRound(std::vector<std::shared_ptr<IPlayer>>& activePlayers,
        std::unordered_map<std::shared_ptr<IPlayer>, int>& roundContributions) {

        
        //начальное значение обязательной ставки.
        int currentBet = bettingSystem_->getCurrentBet();
        bool firstRound = true;
        bool roundInProgress = true;
        while (roundInProgress) {
            roundInProgress = false;  // предполагаем что все сравняли ставки

            // проходим по списку активных игроков
            for (auto& player : activePlayers) {
                // пропускаем неактивных
                if (!player->IsActive()) continue;

                // если вклад игрока ниже обязательной ставки, запрашиваем действие
                if (roundContributions[player] < currentBet  || firstRound) {
                    roundInProgress = true;  // найдены игроки, которым надо действовать
                    int pid = player->GetID();
                    sendActions(roundContributions, currentBet, player);
                    broadcastCallback_("ACTION_EXPECTED_FROM: " + std::to_string(player->GetID()) + ".\n");

                    // Ожидаем ответ от игрока (например, в течение 20 секунд)
                    bool responded = false;
                    std::string action;
                    {
                        std::unique_lock<std::mutex> lock(actionsMutex_);
                        responded = actionsCv_.wait_for(lock, std::chrono::seconds(30),
                            [this, pid]() -> bool {
                                for (const auto& act : actionsQueue_) {
                                    if (act.first == pid)
                                        return true;
                                }
                                return false;
                            }
                        );
                        if (responded) {
                            auto it = std::find_if(actionsQueue_.begin(), actionsQueue_.end(),
                                [pid](const std::pair<int, std::string>& act) {
                                    return act.first == pid;
                                }
                            );
                            if (it != actionsQueue_.end()) {
                                action = it->second;
                                actionsQueue_.erase(it);
                            }
                        }
                    }

                    // Если игрок не ответил – удаляем его.
                    if (!responded) {
                        broadcastCallback_(player->GetName() + " не ответил вовремя и выбыл.\n");
                        bettingSystem_->fold(player);
                       // player->Fold();
                        broadcastCallback_("MOVE: " + player->GetName() + " не ответил вовремя.\n");
                        player->SetActive(false);

                        // Проверяем, остался ли только один активный игрок
                        int remainingCount = std::count_if(activePlayers.begin(), activePlayers.end(),
                            [](std::shared_ptr<IPlayer> p) { return p->IsActive(); });

                        if (remainingCount == 1) { // Остался только один активный игрок – он победил
                            auto winner = std::find_if(activePlayers.begin(), activePlayers.end(),
                                [](std::shared_ptr<IPlayer> p) { return p->IsActive(); });

                            if (winner != activePlayers.end()) {
                                broadcastCallback_("WINNER: " + (*winner)->GetName() + " ID:" + std::to_string((*winner)->GetID()) + "\n");
                                (*winner)->addToBalance(bettingSystem_->getPot());
                            }
                            return; //
                        }
                        continue;
                    }

                    // Обрабатываем действие игрока.
                    if (action.find("FOLD") == 0) {
                        broadcastCallback_("MOVE: " + player->GetName() + " выбрал FOLD.\n");
                        bettingSystem_->fold(player);
                        player->SetActive(false);

                        // Проверяем, остался ли только один активный игрок
                        int remainingCount = std::count_if(activePlayers.begin(), activePlayers.end(),
                            [](std::shared_ptr<IPlayer> p) { return p->IsActive(); });

                        if (remainingCount == 1) { // Остался только один активный игрок – он победил
                            auto winner = std::find_if(activePlayers.begin(), activePlayers.end(),
                                [](std::shared_ptr<IPlayer> p) { return p->IsActive(); });

                            if (winner != activePlayers.end()) {
                                broadcastCallback_("WINNER: " + (*winner)->GetName() + " ID:" + std::to_string((*winner)->GetID()) + "\n");
                                (*winner)->addToBalance(bettingSystem_->getPot());
                            }
                            return; //
                        }
                    }
                    else if (action.find("CALL") == 0 || action.find("CHECK") == 0) {
                        int diff = currentBet - roundContributions[player];
                        roundContributions[player] += diff;
                        bettingSystem_->call(player);
                        broadcastCallback_("MOVE: " + player->GetName() + " делает " + (diff > 0 ? "CALL" : "CHECK") + ".\n");
                    }
                    else if (action.find("RAISE") == 0 || action.find("BET") == 0) {
                        try {
                            size_t spacePos = action.find(" ");
                            if (spacePos == std::string::npos)
                                throw std::invalid_argument("Неверный формат команды RAISE/BET.");
                            int newBet = std::stoi(action.substr(spacePos + 1));
                            if (newBet <= currentBet) {
                                sendMessageToPlayerCallback_(pid, "Ставка должна быть больше текущей (" + std::to_string(currentBet) + ").\n");
                                // Если рейз некорректен, даем шанс повторить ход.
                                roundInProgress = true;
                                continue;
                            }
                            int diff = currentBet - roundContributions[player];
                            roundContributions[player] += diff + (newBet - currentBet);
                            bettingSystem_->raise(player, newBet);
                            broadcastCallback_("MOVE: " + player->GetName() + " поднимает ставку до " + std::to_string(newBet) + ".\n");
                            currentBet = newBet;  // обновляем текущую обязательную ставку
                        }
                        catch (const std::exception& ex) {
                            sendMessageToPlayerCallback_(pid, std::string("Ошибка обработки рейза: ") + ex.what() + "\n");
                            roundInProgress = true;
                            continue;
                        }
                    }
                    else {
                        sendMessageToPlayerCallback_(pid, "Неверное действие. Попробуйте ещё раз.\n");
                        roundInProgress = true;
                        continue;
                    }
                }  
                std::this_thread::sleep_for(std::chrono::seconds(3));
            }  
            broadcastState();
            firstRound = false;
            // После прохода по всем игрокам — если хотя бы один игрок имел вклад ниже currentBet,
            // цикл продолжается, чтобы запросить недостающий вклад.
        }  // while
        broadcastCallback_("CURRENT_BET: " + std::to_string(currentBet) +
            ". POT: " + std::to_string(bettingSystem_->getPot()) + "\n");
      
    }

    */

    
    //сначала публикуются блайнды, затем выполняется общий раунд ставок
    void ServerGame::fullPreflopRound(std::vector<std::shared_ptr<IPlayer>>& activePlayers,
        std::unordered_map<std::shared_ptr<IPlayer>, int>& roundContributions) {
        // Этап 1: Публикация обязательных ставок (блайндов)
        
        std::this_thread::sleep_for(std::chrono::seconds(10));
        preflopBettingRound(activePlayers, roundContributions);
        //broadcastState();
        // Этап 2: Полноценный раунд ставок, когда все игроки могут уравнять ставку или повысить её.
        ccommonBettingRound(activePlayers, roundContributions);
    }
    
    void  ServerGame::RoundEnd() {
        bettingSystem_->resetPot();

        auto pl = table_->getPlayers();
        for (auto p : pl) {
            p->ClearHand();
        }

        table_ -> resetTable();
        gameStarted_ = false;
        broadcastState();
        std::this_thread::sleep_for(std::chrono::seconds(10));
    
        //timerLoop();
    }

    void ServerGame::requestStop() {
        stopRequested_ = true;
        timerCv_.notify_all();
    }

    // Метод для корректного запуска игрового цикла (нового раунда)
    void ServerGame::startGameRounds() {
        // Перед запуском нового раунда нужно сбросить флаг остановки
        stopRequested_ = false;
        if (gameThread_.joinable()) {
            gameThread_.join();
        }
        // Запускаем новый игровой раунд в отдельном потоке
        gameThread_ = std::thread(&ServerGame::gameRound, this);
        // Если раунд должен работать асинхронно, можно не вызывать join() здесь
    }
    void ServerGame::timerLoop() {
        std::unique_lock<std::mutex> lock(timerMutex_);
        while (!stopRequested_) {
            bool reset = timerCv_.wait_for(lock, std::chrono::seconds(30), [this]() { return timerResetRequested_; });
            if (stopRequested_) break;
            if (!reset) {
                if (table_->getPlayers().size() >= 2 && !gameStarted_) {
                    broadcastCallback_("Таймер истек. Игра запускается автоматически.\n");
                    for (auto& player : table_->getPlayers()) {
                        if (player->GetBalance() < 20) {
                            removePlayer(player ->GetID());
                            std::cout << "Player " << player->GetName() << " бомж без денег чмоки поки" << std::endl;
                        }
                        else {
                            player->SetActive(true);
                            std::cout << "Player " << player->GetName() << " становится активным для нового раунда" << std::endl;
                        }
                       
                    }
                   // gameStarted_ = true;
                    lock.unlock();
                    startGameRounds();
                    lock.lock();
                  // gameStarted_ = false;
                }
            }
            else {
                timerResetRequested_ = false;
                broadcastCallback_("Новый игрок добавлен. Таймер сброшен.\n");
              
            }
        }
    }


    // Основной игровой раунд.
    // В текущем раунде участвуют только активные игроки.
    // Новые игроки, заходящие в процессе, добавляются в table_->getPlayers() с состоянием Inactive.
    void ServerGame::gameRound() {
        try {
            roundCancelled_ = false;
            gameStarted_ = true;
            // Получаем активных игроков
            auto players = table_->getActivePlayers();
            if (players.empty()) {
                broadcastCallback_("Ошибка: игроков нет!\n");
                return;
            }
            broadcastCallback_("START: Новая рука начинается.\n");
            std::this_thread::sleep_for(std::chrono::seconds(5));
            players = table_->getActivePlayers();
            // Раздача стартовых карт для активных игроков
            for (const auto& p : players) {
                try {
                    if (p->IsActive()) {
                        table_->dealStartingCards(p);
                    }
                   
                }
                catch (const std::exception& ex) {
                    std::ostringstream oss;
                    oss << "Ошибка раздачи карт для " << p->GetName() << ": " << ex.what() << "\n";
                    broadcastCallback_(oss.str());
                }
            }
            broadcastCallback_("GAME_STATE: Префлоп: ставки начинаются.\n");
            std::this_thread::sleep_for(std::chrono::seconds(3));
            sendPlayerCards();
            // Получаем обновленный список активных игроков
            players = table_->getActivePlayers();
            if (players.size() < 2) {
                broadcastCallback_("Ошибка: недостаточно игроков для префлопа.\n");
                return;
            }
           
            // Инициализируем взносы игроков в текущем раунде
            std::unordered_map<std::shared_ptr<IPlayer>, int> roundContributions;
            for (auto& player : players) {
                roundContributions[player] = 0;
            }

            std::this_thread::sleep_for(std::chrono::seconds(5));
            broadcastState();
            fullPreflopRound(players, roundContributions);

            players = table_->getActivePlayers();
            if (players.size() < 2) {
                broadcastCallback_("WINNER: " + players[0]->GetName() + " ID:" + std::to_string(players[0]->GetID()) + "\n");
                players[0]->addToBalance(bettingSystem_->getPot());
                RoundEnd();
                return;
            }

            std::this_thread::sleep_for(std::chrono::seconds(3));

            // Флоп: 3 карты
            table_->dealCommunityCard();
            table_->dealCommunityCard();
            table_->dealCommunityCard();
            broadcastCallback_("GAME_STATE: Флоп открыт.\n");
            std::this_thread::sleep_for(std::chrono::seconds(2));
            sendTableCards();
            std::this_thread::sleep_for(std::chrono::seconds(2));
            broadcastState();
            std::this_thread::sleep_for(std::chrono::seconds(2));
            commonBettingRound(players, roundContributions);
            broadcastState();

            players = table_->getActivePlayers();
            if (players.size() < 2) {
                broadcastCallback_("WINNER: " + players[0]->GetName() + " ID:" + std::to_string(players[0]->GetID()) + "\n");
                players[0]->addToBalance(bettingSystem_->getPot());
                RoundEnd();
                return;
            }
                

            std::this_thread::sleep_for(std::chrono::seconds(3));

            // Тёрн
            table_->dealCommunityCard();
            broadcastCallback_("GAME_STATE: Тёрн открыт.\n");
            std::this_thread::sleep_for(std::chrono::seconds(2));
            sendTableCards();
            std::this_thread::sleep_for(std::chrono::seconds(2));
            broadcastState();
            std::this_thread::sleep_for(std::chrono::seconds(2));
            commonBettingRound(players, roundContributions);

            players = table_->getActivePlayers();
            if (players.size() < 2) {
                broadcastCallback_("WINNER: " + players[0]->GetName() + " ID:" + std::to_string(players[0]->GetID()) + "\n");
                players[0]->addToBalance(bettingSystem_->getPot());
                RoundEnd();
                return;
            }


            std::this_thread::sleep_for(std::chrono::seconds(3));

            // Ривер
            table_->dealCommunityCard();
            broadcastCallback_("GAME_STATE: Ривер открыт.\n");
            std::this_thread::sleep_for(std::chrono::seconds(2));
            sendTableCards();
            std::this_thread::sleep_for(std::chrono::seconds(2));
            broadcastState();
            std::this_thread::sleep_for(std::chrono::seconds(2));
            commonBettingRound(players, roundContributions);

            players = table_->getActivePlayers();
            if (players.size() < 2) {
                broadcastCallback_("WINNER: " + players[0]->GetName() + " ID:" + std::to_string(players[0]->GetID()) + "\n");
                players[0]->addToBalance(bettingSystem_->getPot());
                RoundEnd();
                return;
            }

            // Определяем оценки рук для всех игроков.
            std::vector<HandEvaluation> evaluations;
            auto allPlayers = table_->getActivePlayers();
            const auto& communityCards = table_->getCommunityCards();
            std::unordered_map<int, HandEvaluation> playerEvaluations;

            for (const auto& player : allPlayers) {
                
                HandEvaluation eval = rules_->EvaluateHand(player->GetFullHand(communityCards));
                evaluations.push_back(eval);
                playerEvaluations[player->GetID()] = eval;
                player->ClearHand();
            }

            // Определяем победителей по оценке рук.
            // Предполагается, что rules_->determineWinners() возвращает индексы в массиве 'evaluations' соответствующие победителям.
            std::vector<int> winnerIndices = rules_->determineWinners(evaluations);
            std::vector<std::shared_ptr<IPlayer>> winners;
            for (int index : winnerIndices) {
                winners.push_back(allPlayers[index]);
            }

            // Получаем сегменты банка (side pots)
            auto sidePots = bettingSystem_->getSidePots();

            // Распределяем банк между победителями
            auto winnings = PotDistributor::distribute(sidePots, winners);

            // Обновляем баланс победителей и отправляем сообщение каждому.
            for (const auto& winner : winners) {
                int playerID = winner->GetID();
                int winAmount = winnings[winner];
                // Добавляем выигрыш к балансу игрока
                winner->addToBalance(winAmount);
                // Отправляем сообщение игроку
                sendMessageToPlayerCallback_(playerID, "SHOWDOWN: Вы выиграли " + std::to_string(winAmount) + "!\n");
                broadcastCallback_("WINNER: " + winner->GetName() + " ID:" + std::to_string(playerID) + "\n");
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }

            broadcastCallback_("GAME_STATE: Шоудаун завершен. Рука окончена.\n");
            table_->rotatePlayers();

            // После завершения раунда возвращаем всех игроков в статус Active для следующего раунда.
            for (auto& player : table_->getPlayers()) {
                player->SetActive(true);
            }

            RoundEnd();
           
        }
       
        catch (const RoundCancelledException& ex) {
            broadcastCallback_("Раунд отменён: " + std::string(ex.what()) + "\n");
            auto players = table_->getActivePlayers();
            if (players.size() != 0) {
                broadcastCallback_("WINNER: " + players[0]->GetName() + " ID:" + std::to_string(players[0]->GetID()) + "\n");
                players[0]->addToBalance(bettingSystem_->getPot());
            }
            RoundEnd();  
           
        }
        catch (const std::exception& ex) {
            std::cerr << "Ошибка в gameRound: " << ex.what() << std::endl;
            broadcastCallback_("Игра завершилась из-за ошибки.\n");
        }

    }
    // Внешний метод для отправки актуального состояния (вызывается сервером)
    void ServerGame::broadcastGameState() {
        broadcastState();
    }

    /*
    #include <windows.h>
    void ServerGame::terminateGameThread() {
        HANDLE threadHandle = OpenThread(THREAD_TERMINATE, FALSE, GetThreadId(gameThread_.native_handle()));
        if (threadHandle) {
            TerminateThread(threadHandle, 0);
            CloseHandle(threadHandle); // Закрываем дескриптор
        }
    }
    // В классе ServerGame добавьте:
 

    // Метод, который устанавливает флаг остановки
    void ServerGame::requestStop() {
        stopRequested_ = true;
        // Если используется условная переменная для ожидания, уведомите её
        timerCv_.notify_all();
    }
    // Корректный запуск игрового цикла:
    void ServerGame::startGameRounds() {
        // Перед запуском нового раунда устанавливаем флаг остановки в false
        stopRequested_ = false;
        if (gameThread_.joinable()) {
            //  можно установить stopRequested_ = true и дождаться завершения
            gameThread_.join();
        }

        gameThread_ = std::thread(&ServerGame::gameRound, this);
        // Optionally, можно не сразу вызывать join(), 
        // если игровой цикл должен работать асинхронно
    }

    void ServerGame::timerLoop() {
        std::unique_lock<std::mutex> lock(timerMutex_);
        while (!stopRequested_) {
            bool reset = timerCv_.wait_for(lock, std::chrono::seconds(20), [this]() {
                return timerResetRequested_;
                });
            if (stopRequested_) break;
            if (!reset) {
                if (table_->getPlayers().size() >= 2 && !gameStarted_) {
                    broadcastCallback_("Таймер истек. Игра запускается автоматически.\n");
                    for (auto& player : table_->getPlayers()) {
                        player->SetActive(true);
                        std::cout << "Player " << player->GetName() << " is active now" << std::endl;
                    }
                    gameStarted_ = true;
                    lock.unlock();
                    startGameRounds();
                    lock.lock();
                    gameStarted_ = false;
                }
            }
            else {
                timerResetRequested_ = false;
                broadcastCallback_("Новый игрок добавлен. Таймер сброшен.\n");
            }
        }
    }
     
    void ServerGame::gameRound() {
        auto players = table_->getActivePlayers();
        if (players.empty()) {
            broadcastCallback_("Ошибка: игроков нет!\n");
            return;
        }
        broadcastCallback_("START: Новая рука начинается.\n");/////////////////////////////////////

        // Раздача стартовых карт
        for (const auto& p : players) {
            try {
                table_->dealStartingCards(p);
            }
            catch (const std::exception& ex) {
                std::ostringstream oss;
                oss << "Ошибка раздачи карт для";
                oss << p->GetName();
                oss << ": ";
                oss << ex.what();
                oss << '\n';
                std::string message = oss.str();
                broadcastCallback_(message);
            }
        }
        sendPlayerCards();
        std::vector<std::shared_ptr<IPlayer>> activePlayers = table_->getActivePlayers();
        if (activePlayers.size() < 2) {
            broadcastCallback_("Ошибка: недостаточно игроков для префлопа.\n");
            return;
        }
        // Инициализируем взносы игроков в текущем раунде.
        std::unordered_map<std::shared_ptr<IPlayer>, int> roundContributions;
        for (auto& player : activePlayers) {
            roundContributions[player] = 0;
        
        std::this_thread::sleep_for(std::chrono::seconds(10));
        broadcastState();
        // Префлоп: публикуем ставки (blinds) и даем возможность уравнять
        fullPreflopRound(activePlayers, roundContributions);///////////////////////////////////////////
        
        activePlayers = table_->getActivePlayers();
        if (activePlayers.size() < 2)
            return;

        
        // Флоп (3 карты)
        table_->dealCommunityCard();
        table_->dealCommunityCard();
        table_->dealCommunityCard();
        broadcastCallback_("GAME_STATE: Флоп открыт.\n"); //////////////////////////////////////////
        std::this_thread::sleep_for(std::chrono::seconds(2));
        sendTableCards();
        std::this_thread::sleep_for(std::chrono::seconds(2));
        broadcastState();
        std::this_thread::sleep_for(std::chrono::seconds(2));
        commonBettingRound(activePlayers, roundContributions);
        broadcastState();
        activePlayers = table_->getActivePlayers();
        if (activePlayers.size() < 2)
            return;

        std::this_thread::sleep_for(std::chrono::seconds(2));

        // Тёрн
        table_->dealCommunityCard();
        sendTableCards();
        broadcastCallback_("GAME_STATE: Тёрн открыт.\n");///////////////////////////
        broadcastState();
        commonBettingRound(activePlayers, roundContributions);
        activePlayers = table_->getActivePlayers();
        if (activePlayers.size() < 2)
            return;

        std::this_thread::sleep_for(std::chrono::seconds(2));
        // Ривер
        table_->dealCommunityCard();
        sendTableCards();
        broadcastCallback_("GAME_STATE: Ривер открыт.\n");///////////////////////////
        broadcastState();
        commonBettingRound(activePlayers, roundContributions);

        activePlayers = table_->getActivePlayers();
        if (activePlayers.size() < 2)
            return;

        std::vector<HandEvaluation> evaluations;
        auto playerss = table_->getPlayers();
        const auto& communityCards = table_->getCommunityCards();

        std::unordered_map<int, HandEvaluation> playerEvaluations; //  Привязываем ID игрока к его оценке руки

        for (const auto& player : playerss) {
           
            HandEvaluation eval = rules_->EvaluateHand(player->GetFullHand(communityCards)); //  Получаем 7 карт
            evaluations.push_back(eval);
            playerEvaluations[player->GetID()] = eval; //  Сохраняем для дальнейшего распределения

          //  player->ClearHand();
        }



        // 2. определяем победителей по их комбинациям
        std::vector<int> winnerIndices = rules_->determineWinners(evaluations);

        // 3. Преобразуем индексы победителей в `shared_ptr<IPlayer>`
        std::vector<std::shared_ptr<IPlayer>> winners;
        for (int index : winnerIndices) {
            winners.push_back(players[index]); //  Теперь передаём в `distribute()` `shared_ptr<IPlayer>`
        }
        // 3. получаем сайд-поты
        auto sidePots = bettingSystem_->getSidePots();

        // 2. распределяем банк
        auto winnings = PotDistributor::distribute(sidePots, winners);

        // 3. отправляем каждому победителю его выигрыш
        for (const auto& winner : winners) {
            int playerID = winner->GetID(); //  Получаем ID игрока
            int winAmount = winnings[winner]; //  Теперь передаём `shared_ptr<IPlayer>`
      
            int win = winnings[winner];
           // winner->addToBalance(win);
            std::cout << "-----------------------------------------" << win << std::endl;

          sendMessageToPlayerCallback_(playerID, "SHOWDOWN: Вы выиграли " + std::to_string(winAmount) + "!\n");
           broadcastCallback_("WINNER: " + player -> GetName() + " ID:" + std::to_string(player->GetID()) + "\n");/////// сделать через стрим, 
                                                                                                  //чтобы если несколько победителей не дублировать

            std::this_thread::sleep_for(std::chrono::milliseconds(50));

        }


       // RoundEnd();
        broadcastCallback_("GAME_STATE: Шоудаун завершен. Рука окончена.\n");

        table_->rotatePlayers();
        timerLoop();
    }
    

    */

 /*   bool isBetSame(std::unordered_map<std::shared_ptr<IPlayer>, int> roundContributions) {
        bool a = true;
        int bet = roundContributions.begin()->second;
        for (const auto& pair : roundContributions) {
            if (bet != pair.second) {
                a = false;
                break;
            }
        }
        return a;
    }

    int maxBet(const std::unordered_map<std::shared_ptr<IPlayer>, int>& roundContributions) {
        if (roundContributions.empty()) {
            return -1; 
        }

        auto maxElem = std::max_element(roundContributions.begin(), roundContributions.end(),
            [](const auto& a, const auto& b) { return a.second < b.second; });

        int maxValue = maxElem->second;

        int count = std::count_if(roundContributions.begin(), roundContributions.end(),
            [maxValue](const auto& elem) { return elem.second == maxValue; });
        if (count > 1) {
            return -1; 
        }
        std::cout << "Максимальный вклад: " << maxValue << std::endl;
        return maxElem->first->GetID();
    }*/


   
      /*  auto activePlayers = table_->getActivePlayers();
        if (activePlayers.size() < 2) {
            std::ostringstream oss;
            oss << "Warrning! Недостаточно игроков для ставки.";
            oss << '\n';
            std::string message = oss.str();
            broadcastCallback_(message);
            return;
        }

        // Для каждого игрока храним внесённый ими вклад в этом раунде.
        std::unordered_map<std::shared_ptr<IPlayer>, int> roundContributions;
        for (const auto& player : activePlayers) {
            roundContributions[player] = 0;
        }


        int currentBet = bettingSystem_-> getCurrentBet();
        size_t turnIndex = 0;
        bool bettingInProgress = true;

        // Цикл ставок продолжается до тех пор, пока все активные игроки не сравняют вклад.
        while (bettingInProgress && activePlayers.size() >= 2) {
            auto &currentPlayer = activePlayers[turnIndex];
            int pid = currentPlayer->GetID();

            sendActions(roundContributions, currentBet, currentPlayer);

            broadcastCallback_("Ожидается действие от " + std::to_string(currentPlayer->GetID()) + ".\n");

            bool actionReceived = false;
            std::string action;

            {
                std::unique_lock<std::mutex> lock(actionsMutex_);
                actionReceived = actionsCv_.wait_for(lock, std::chrono::seconds(30), [this, pid]() -> bool {
                    for (const auto& act : actionsQueue_) {
                        if (act.first == pid) return true;
                    }
                    return false;
                    });

                if (actionReceived) {
                    auto it = std::find_if(actionsQueue_.begin(), actionsQueue_.end(),
                        [&](const std::pair<int, std::string>& act) { return act.first == pid; });

                    if (it != actionsQueue_.end()) {
                        action = it->second;
                        actionsQueue_.erase(it);
                    }
                }
            }

        if (!actionReceived) {
            // Игрок не ответил – удаляем его.
            broadcastCallback_("Игрок " + currentPlayer->GetName() +
                " не ответил вовремя и выбыл.\n");
            removePlayer(pid);
            activePlayers = table_->getActivePlayers();
            if (activePlayers.size() < 2)
                break;
            if (turnIndex >= activePlayers.size())
                turnIndex = 0;
            continue;
        }

        // Обработка действия (попытка распарсить команду).
        if (action.find("FOLD") == 0) {
            broadcastCallback_(currentPlayer->GetName() + " выбрал FOLD.\n");
            bettingSystem_->fold(currentPlayer);
           // currentPlayer->SetActive(false);
            activePlayers = table_->getActivePlayers();
            if (activePlayers.size() < 2)
                break;
            if (turnIndex >= activePlayers.size())
                turnIndex = 0;
            continue;
        }
        else if (action.find("CALL") == 0 || action.find("CHECK") == 0) {

            int toCall = currentBet - roundContributions[currentPlayer];
            roundContributions[currentPlayer] += toCall;
            bettingSystem_->call(currentPlayer);
            broadcastCallback_(currentPlayer->GetName() + " выбирает " + (toCall > 0 ? "CALL" : "CHECK") + ".\n");
        }

        else if (action.find("RAISE") == 0 || action.find("BET") == 0) {
            try {
                size_t spacePos = action.find(" ");
                if (spacePos == std::string::npos)
                    throw std::invalid_argument("Неверный формат команды RAISE/BET.");
                int newBet = std::stoi(action.substr(spacePos + 1));
                if (newBet <= currentBet) {
                    sendMessageToPlayerCallback_(pid, "Ставка должна быть больше текущей (" + std::to_string(currentBet) + ").\n");
                    continue; // Повторяем ход данного игрока.
                }
                int callAmt = currentBet - roundContributions[pid];
                roundContributions[pid] += callAmt + (newBet - currentBet);
                currentBet = newBet;
                bettingSystem_->raise(currentPlayer, newBet);
                broadcastCallback_(currentPlayer->GetName() + " делает рейз до " + std::to_string(newBet) + ".\n");
                // После рейза необходимо дать возможность всем игрокам снова реагировать.
                turnIndex = (turnIndex + 1) % activePlayers.size();
                continue;
            }
            catch (const std::exception& ex) {
                sendMessageToPlayerCallback_(pid, std::string("Ошибка обработки рейза: ") + ex.what() + "\n");
                continue; // Позволяем игроку повторить ход.
            }
        }
        else {
            sendMessageToPlayerCallback_(pid, "Неверное действие. Попробуйте ещё раз.\n");
            continue;
        }
        // Переходим к следующему игроку.
        turnIndex = (turnIndex + 1) % activePlayers.size();

        // Проверяем, все ли активные игроки сравняли ставки.
        bool roundComplete = true;
        for (const auto& p : activePlayers) {
            if (roundContributions[p->GetID()] != currentBet) {
                roundComplete = false;
                break;
            }
        }
        if (roundComplete)
        bettingInProgress = false;
                }
        broadcastCallback_("Ставки завершены. Текущая ставка: " + std::to_string(currentBet) +
            ". Пот: " + std::to_string(bettingSystem_->getPot()) + "\n");


        };

            */
    


    /*
    
        void ServerGame::timerLoop() {
        std::unique_lock<std::mutex> lock(timerMutex_); // только один поток может изменить таймер и это поток игры

        while (!stopRequested_) {
            bool reset = timerCv_.wait_for(lock, std::chrono::seconds(20), [this]() { return timerResetRequested_; });
            if (stopRequested_) break;
            if (!reset) {
                if (table_->getPlayers().size() >= 2 && !gameStarted_) {
                    broadcastCallback_("Таймер истек. Игра запускается автоматически.\n");
                    for (auto& player : table_->getPlayers()) {
                        player->SetActive(true);
                        std::cout << "Player " << player->GetName() << "is active now" << std::endl;

                    }
                    startGameRounds();
                    gameStarted_ = true;
                    break;
                }
            }
            else {
                timerResetRequested_ = false;
                broadcastCallback_("Новый игрок добавлен. Таймер сброшен.\n");
            }
        }
    }
    */



 
 
  

    /*
    * 
    
    
    // Основной цикл игры (начало руки).
    void ServerGame::gameRound() {
        auto players = table_->getActivePlayers();
        if (players.empty()) {
            broadcastCallback_("Ошибка: игроков нет!\n");
            return;
        }
        broadcastCallback_("START: Новая рука начинается.\n");

        // Префлоп
        for (const auto& p : players) {
            try {
                table_->dealStartingCards(p);
            }
            catch (const std::exception& ex) {
                std::ostringstream oss;
                oss << "Ошибка раздачи карт для";
                oss << p->GetName();
                oss << ": ";
                oss << ex.what();
                oss << '\n';
                std::string message = oss.str();
                broadcastCallback_(message);
            }
        }
        sendPlayerCards();
        std::this_thread::sleep_for(std::chrono::seconds(10));
        broadcastState();
        broadcastCallback_("GAME_STATE: Префлоп: ставки начинаются.\n");
        std::this_thread::sleep_for(std::chrono::seconds(10));
        bettingRound();
        if (table_->getPlayers().size() < 2) return;

        // Флоп (3 карты)
        table_->dealCommunityCard();
        table_->dealCommunityCard();
        table_->dealCommunityCard();
        broadcastCallback_("GAME_STATE: Флоп открыт.\n");
        broadcastState();
        bettingRound();
        if (table_->getPlayers().size() < 2) return;

        // Тёрн (1 карта)
        table_->dealCommunityCard();
        broadcastCallback_("GAME_STATE: Тёрн открыт.\n");
        broadcastState();
        bettingRound();
        if (table_->getPlayers().size() < 2) return;

        // Ривер (1 карта)
        table_->dealCommunityCard();
        broadcastCallback_("GAME_STATE: Ривер открыт.\n");
        broadcastState();
        bettingRound();

        std::vector<HandEvaluation> evaluations;
        auto playerss = table_->getPlayers();
        const auto& communityCards = table_->getCommunityCards();

        std::unordered_map<int, HandEvaluation> playerEvaluations; //  Привязываем ID игрока к его оценке руки

        for (const auto& player : playerss) {
            HandEvaluation eval = rules_->EvaluateHand(player->GetFullHand(communityCards)); //  Получаем 7 карт
            evaluations.push_back(eval);
            playerEvaluations[player->GetID()] = eval; //  Сохраняем для дальнейшего распределения
        }


        // 2. Определяем победителей по их комбинациям
        std::vector<int> winnerIndices = rules_->determineWinners(evaluations);

        // 3. Преобразуем индексы победителей в `shared_ptr<IPlayer>`
        std::vector<std::shared_ptr<IPlayer>> winners;
        for (int index : winnerIndices) {
            winners.push_back(players[index]); //  Теперь передаём в `distribute()` `shared_ptr<IPlayer>`
        }
        // 3. Получаем сайд-поты
        auto sidePots = bettingSystem_->getSidePots();

        // 2. Распределяем банк
        auto winnings = PotDistributor::distribute(sidePots, winners);

        // 3. Отправляем каждому победителю его выигрыш
        for (const auto& winner : winners) {
            int playerID = winner->GetID(); //  Получаем ID игрока
            int winAmount = winnings[winner]; //  Теперь передаём `shared_ptr<IPlayer>`
            sendMessageToPlayerCallback_(playerID, "SHOWDOWN: Вы выиграли " + std::to_string(winAmount) + "!\n");
        }

        broadcastCallback_("GAME_STATE: Шоудаун завершен. Рука окончена.\n");
    }

    

    * 
    void playHand(); 
    void startGame(); // проверяет количество игроков, запускает playHand() и устанавливает gameStarted_ = true
    void advanceNextRound();
    void processTurn();

    * 
    * 
       // Основной метод для игры (одна рука).
    // Реализует полную последовательность: раздача массовых карт, циклические раунды ставок, раскрытие комьюнити?карт, шоудаун.
    void ServerGame::playHand() {
        // Обновляем состояние: сбрасываем стол и банк.
        table_->resetTable();
        bettingSystem_->resetPot();

        // Раздача карманных карт каждому игроку.
        auto players = table_->getPlayers();
        for (const auto& player : players) {
            try {
                table_->dealStartingCards(player);
                player->SetActive(true);
            }
            catch (const std::exception& ex) {
                broadcastCallback_("Ошибка раздачи карт для " + player->GetName() + ": " + ex.what() + "\n");
            }
        }
        // Отправляем каждому его карты.
        sendPlayerCards();
        broadcastState();
        if (table_->getPlayers().size() < 2) return;
        // Префлоп: цикл ставок.
        broadcastCallback_("Префлоп: ставки начинаются.\n");
        bettingRound();
        if (table_->getPlayers().size() < 2) return;


        // Флоп: раскрываем 3 комьюнити карты.
        table_->dealCommunityCard();
        table_->dealCommunityCard();
        table_->dealCommunityCard();
        broadcastCallback_("Флоп открыт.\n");
        broadcastState();
        bettingRound();
        if (table_->getPlayers().size() < 2) return;

        // Тёрн: одна карта.
        table_->dealCommunityCard();
        broadcastCallback_("Тёрн открыт.\n");
        broadcastState();
        bettingRound();
        if (table_->getPlayers().size() < 2) return;

        // Ривер: последняя комьюнити карта.
        table_->dealCommunityCard();
        broadcastCallback_("Ривер открыт.\n");
        broadcastState();
        bettingRound();

        std::vector<HandEvaluation> evaluations;
        auto playerss = table_->getPlayers();
        const auto& communityCards = table_->getCommunityCards();

        std::unordered_map<int, HandEvaluation> playerEvaluations; //  Привязываем ID игрока к его оценке руки

        for (const auto& player : playerss) {
            HandEvaluation eval = rules_->EvaluateHand(player->GetFullHand(communityCards)); //  Получаем 7 карт
            evaluations.push_back(eval);
            playerEvaluations[player->GetID()] = eval; //  Сохраняем для дальнейшего распределения
        }


        // 2. Определяем победителей по их комбинациям
        std::vector<int> winnerIndices = rules_->determineWinners(evaluations);

        // 3. Преобразуем индексы победителей в `shared_ptr<IPlayer>`
        std::vector<std::shared_ptr<IPlayer>> winners;
        for (int index : winnerIndices) {
            winners.push_back(players[index]); //  Теперь передаём в `distribute()` `shared_ptr<IPlayer>`
        }
        // 3. Получаем сайд-поты
        auto sidePots = bettingSystem_->getSidePots();

        // 2. Распределяем банк
        auto winnings = PotDistributor::distribute(sidePots, winners);

        // 3. Отправляем каждому победителю его выигрыш
        for (const auto& winner : winners) {
            int playerID = winner->GetID(); //  Получаем ID игрока
            int winAmount = winnings[winner]; //  Теперь передаём `shared_ptr<IPlayer>`
            sendMessageToPlayerCallback_(playerID, "SHOWDOWN: Вы выиграли " + std::to_string(winAmount) + "!\n");
        }

        broadcastCallback_("Шоудаун завершен. Рука окончена.\n");
    }

    void ServerGame::startGame() {
        std::lock_guard<std::mutex> lock(gameMutex_);

        // Проверяем, достаточно ли игроков для начала игры
        if (table_->getPlayers().size() < 2) {
            broadcastCallback_("Нужно минимум 2 игрока для начала игры.\n");
            return;
        }

        // Если игра ещё не запущена, начинаем новую руку
        if (!gameStarted_) {
            broadcastCallback_("Игра начинается!\n");

            gameStarted_ = true;
            playHand();  //  Запускаем полноценный раунд игры
            gameStarted_ = false; //  Завершаем игру после руки
        }
    }

    void ServerGame::advanceNextRound() {
    }

    // Простейшая обработка хода – для примера просто проходит переход (в реальной логике сюда можно вложить обработку команд игроков)
    void ServerGame::processTurn() {
        advanceNextRound();
        broadcastState();
    }


    */