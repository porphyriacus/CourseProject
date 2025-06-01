#include "ServerGame.h"

#include <exception>

class RoundCancelledException : public std::exception {
public:
    const char* what() const noexcept override {
        return "Round Cancelled";
    }
};


//   � broadcastCallback_ � ��� �������� ��������� ���� �������
//   � sendMessageToPlayerCallback_ � ��� �������� ������������ ���������

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
        // ���������� PreFlopRound ��� ������ �����.
       
        rounds_.push_back(std::make_unique<PreFlopRound>());
        timerThread_ = std::thread(&ServerGame::timerLoop, this); // ������� �������� ������� ����� 
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

    // �����, ������� ������ �������� ��� ��������� �������� �� ������ (��������, ����� ������� ���������).
    // �������� ����������� � ������� ��� ��������� � bettingRound.
    void ServerGame::processPlayerAction(int playerID, const std::string& action) {
        {
            std::lock_guard<std::mutex> lock(actionsMutex_);
            actionsQueue_.push_back({ playerID, action });
        }
        actionsCv_.notify_all();
    }

    // ���������� ������ ������
    void ServerGame::addPlayer(std::shared_ptr<IPlayer> player) {
        std::lock_guard<std::mutex> lock(gameMutex_);
        auto players = table_->getPlayers();
        if (std::any_of(players.begin(), players.end(),
            [&](const std::shared_ptr<IPlayer>& p) { return p->GetID() == player->GetID(); })) {
            std::cerr << "������: ����� � ID " << player->GetID() << " ��� ����������!\n";
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
            std::string isAct = (player->IsActive()) ? "����� ����� ������������ � ����" : "����� ��������, ���������� ��������� ����� ������� ����";
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
            broadcastCallback_("���� ���������. ����� " + player->GetName() + " �� ����� ���� ��������.\n");
        }
    }
 
    void ServerGame::removePlayer(int playerID) {
        std::lock_guard<std::mutex> lock(gameMutex_);
        table_->removePlayer(playerID);
        auto players = table_->getPlayers();
        if (!players.empty()) {
            broadcastState();
            broadcastCallback_("����� � ID " + std::to_string(playerID) + " ������� ����.\n");
            if (table_->getActivePlayers().size() < 2) {
                cancelRound();
            }
        }
        else {
            broadcastCallback_("������: ����� �������� ������� ���� ����!\n");
        }
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    void ServerGame::cancelRound() {
        roundCancelled_ = true; // ������������� � ������������� ������
        // ���������� ����� �������� (��������, � commonBettingRound, ��� ��������� �����)
        actionsCv_.notify_all();
    }
    void ServerGame::checkActivePlayerCount() {
        auto activePlayers = table_->getActivePlayers();
        if (activePlayers.size() < 2) {
            // ���� �������� ������ ����, ����� �������� �����
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
    // �������� ���� (���������) ������� ������ � �� ���������� ����������������.
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
    // ����� ��������� �����: ����������� ������ � ������� �������, ��������� � ���������.
    void ServerGame::broadcastState() {
        std::ostringstream oss;
        oss << "��������� �����:";

        auto players = table_->getPlayers();
        if (players.empty()) {
            std::cerr << "������: ���� ����! ��� �������!\n";
            return;
        }

        for (const auto& p : players) { //  ������ `p` - shared_ptr
            oss << "����� ID " << p->GetID() << ": " << p->GetName()
                << ", ������:" << p->GetBalance()
                << ", ������: " << (p->IsActive() ? "�������" : "���������") << "|";
        }

        oss << '\n';
        std::string gameState = oss.str();
        std::cout << "�������� ���������: " << gameState << std::endl;
        broadcastCallback_(gameState);
    }
    void ServerGame::sendActions(std::unordered_map<std::shared_ptr<IPlayer>, int>& roundContributions, int currentBet, std::shared_ptr<IPlayer> pl) {

        std::ostringstream oss;
        int pid = pl->GetID();
        int plcont = roundContributions[pl];
        oss << "��� ���: ������� ������������ ������ = " << std::to_string(currentBet);
        oss << ", ��� ����� = " << std::to_string(plcont);
        oss << ". ��������: CHECK, ";

        // ������������ ����������� � ������������ ����:
        int minRaise = currentBet; // ����������� ��������� ���������� �� ������� ������
        int maxRaise = currentBet + pl->GetBalance(); // ���������� �������� = ������� ������ + ������

        // ���� ����� ��� �������� ������, ������������ �������� �� (currentBet + ����������� ������)
        auto activePlayers = table_->getActivePlayers();
        if (activePlayers.size() == 2) {
            int minBalance = bettingSystem_->getMinBet(); // ����, ���� ������� getMinActivePlayerBalance() ��������,
            // ���������� � ��� ����������� ������������ �������
            maxRaise = currentBet + minBalance;
        }

        // ���� ����� ����� ������� CALL, ��������� ����� CALL � RAISE � ��������� ����������
        if (pl->CanBet(currentBet - plcont)) {
            oss << "CALL, RAISE <" << std::to_string(minRaise) << ", " << std::to_string(maxRaise) << ">, ";
        }
        oss << "FOLD.\n";

        std::string message = oss.str();
        std::cout << "�������� �������� ������ " << pid << ", ������ = " << currentBet
            << ", ����� = " << roundContributions[pl] << std::endl;
        sendMessageToPlayerCallback_(pid, message);
        std::cout << "ServerGame//\tMessage was sent to player " << pid << "\n" << std::endl;

    }
 

    void ServerGame::handlePlayerAction(int pid, const std::string& action) {
        {
            std::lock_guard<std::mutex> lock(actionsMutex_);
            actionsQueue_.emplace_back(pid, action);  // ��������� �������� � �������
        }
        actionsCv_.notify_all();  // �������� �������, ��� ��������� ����� ��������
    }


    
    void ServerGame::ccommonBettingRound(std::vector<std::shared_ptr<IPlayer>>& activePlayers,
        std::unordered_map<std::shared_ptr<IPlayer>, int>& roundContributions) {
        //��������� �������� ������������ ������.
        int currentBet = bettingSystem_->getCurrentBet();
        bool roundInProgress = true;
        while (roundInProgress) {
            // �������� ��������� �������� �������:
            checkActivePlayerCount();
            if (roundCancelled_) {
                throw RoundCancelledException();
            }
            roundInProgress = false;  // ������������, ��� ������ ����������

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

                    // ��������� ��������� �����
                    checkActivePlayerCount();
                    if (roundCancelled_) { throw RoundCancelledException(); }

                    if (!player->IsActive()) continue;
                    if (!responded) {
                        broadcastCallback_(player->GetName() + " �� ������� ������� � �����.\n");
                        bettingSystem_->fold(player);
                        player->SetActive(false);
                        checkActivePlayerCount();
                        if (table_->getActivePlayers().size() < 2) return;
                        continue;
                    }
                    if (action.find("FOLD") == 0) {
                        broadcastCallback_("MOVE: " + player->GetName() + " ������ FOLD.\n");
                        bettingSystem_->fold(player);
                        player->SetActive(false);

                        if (table_->getActivePlayers().size() < 2) return;
                    }
                    else if (action.find("CALL") == 0 || action.find("CHECK") == 0) {
                        int diff = currentBet - roundContributions[player];
                        roundContributions[player] += diff;
                        bettingSystem_->call(player);
                        broadcastCallback_("MOVE: " + player->GetName() + " ������ " +
                            (diff > 0 ? "CALL" : "CHECK") + ".\n");
                    }
                    else if (action.find("RAISE") == 0 || action.find("BET") == 0) {
                        try {
                            size_t spacePos = action.find(" ");
                            if (spacePos == std::string::npos)
                                throw std::invalid_argument("�������� ������ ������� RAISE/BET.");
                            int newBet = std::stoi(action.substr(spacePos + 1));
                            if (newBet <= currentBet) {
                                sendMessageToPlayerCallback_(pid, "������ ������ ���� ������ ������� (" + std::to_string(currentBet) + ").\n");
                                roundInProgress = true;
                                continue;
                            }
                            int diff = currentBet - roundContributions[player];
                            roundContributions[player] += diff + (newBet - currentBet);
                            bettingSystem_->raise(player, diff + (newBet - currentBet));
                            broadcastCallback_("MOVE: " + player->GetName() + " ��������� ������ �� " + std::to_string(newBet) + ".\n");
                            currentBet = newBet;
                        }
                        catch (const std::exception& ex) {
                            sendMessageToPlayerCallback_(pid, std::string("������ ��������� �����: ") + ex.what() + "\n");
                            roundInProgress = true;
                            continue;
                        }
                    }
                    else {
                        sendMessageToPlayerCallback_(pid, "�������� ��������. ���������� ��� ���.\n");
                        roundInProgress = true;
                        continue;
                    }
                }
                // ����� ����� ������ �������� �� ������
                std::this_thread::sleep_for(std::chrono::seconds(2));
            }
            broadcastState();
        }
        broadcastCallback_("CURRENT_BET: " + std::to_string(currentBet) +
            ". POT: " + std::to_string(bettingSystem_->getPot()) + "\n");
    }


    void ServerGame::preflopBettingRound(std::vector<std::shared_ptr<IPlayer>>& activePlayers,
        std::unordered_map<std::shared_ptr<IPlayer>, int>& roundContributions) {
        // �������� ���������� �������

        activePlayers = table_->getActivePlayers();
        if (activePlayers.size() < 2) {
            std::cerr << "������: ������������ ������� ��� ��������! ������ = " << activePlayers.size() << std::endl;
            return;
        }

        std::shared_ptr<IPlayer> smallBlindPlayer = activePlayers[0];
        std::shared_ptr<IPlayer> bigBlindPlayer = activePlayers[1];
       

        std::cout << '\n' << smallBlindPlayer->GetBalance() << '\n'<< bigBlindPlayer->GetBalance() <<std::endl;
        // �������� ������������� �������
        if (!smallBlindPlayer || !bigBlindPlayer) {
            std::cerr << "������: ������ �� �������� ����� `nullptr`!" << std::endl;
            return;
        }

        int smallBlind = bettingSystem_->getSmallBlind();

        std::cout << '\n' << smallBlind<< '\n'  << std::endl;


        bettingSystem_->placeBet(smallBlindPlayer, smallBlind);
        roundContributions[smallBlindPlayer] = smallBlind;
        broadcastCallback_("MOVE: " + smallBlindPlayer->GetName() + " ������ ����� ������: " + std::to_string(smallBlind) + "\n");
        broadcastState();
        std::this_thread::sleep_for(std::chrono::seconds(1));
        int bigBlind = bettingSystem_->getBigBlind();
        bettingSystem_->placeBet(bigBlindPlayer, bigBlind);
        roundContributions[bigBlindPlayer] = bigBlind;
        broadcastCallback_("MOVE: " + bigBlindPlayer->GetName() + " ������ ������� ������: " + std::to_string(bigBlind) + "\n");
        broadcastState();
        std::this_thread::sleep_for(std::chrono::seconds(3));

    }

    void ServerGame::commonBettingRound(std::vector<std::shared_ptr<IPlayer>>& activePlayers,
        std::unordered_map<std::shared_ptr<IPlayer>, int>& roundContributions) {
        int currentBet = bettingSystem_->getCurrentBet();
        bool firstRound = true;
        bool roundInProgress = true;
        while (roundInProgress) {
            // �������� ��������� �������� �������:
            checkActivePlayerCount();
            if (roundCancelled_) {
                throw RoundCancelledException();
            }
            roundInProgress = false;  // ������������, ��� ������ ����������

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

                    // ��������� ��������� �����
                    checkActivePlayerCount();
                    if (roundCancelled_) { throw RoundCancelledException(); }

                    if (!player->IsActive()) continue;
                    if (!responded) {
                        broadcastCallback_(player->GetName() + " �� ������� ������� � �����.\n");
                        bettingSystem_->fold(player);
                        player->SetActive(false);
                        checkActivePlayerCount();
                        if (table_->getActivePlayers().size() < 2) return;
                        continue;
                    }
                    if (action.find("FOLD") == 0) {
                        broadcastCallback_("MOVE: " + player->GetName() + " ������ FOLD.\n");
                        bettingSystem_->fold(player);
                        player->SetActive(false);
                        checkActivePlayerCount();
                        if (table_->getActivePlayers().size() < 2) return;
                    }
                    else if (action.find("CALL") == 0 || action.find("CHECK") == 0) {
                        int diff = currentBet - roundContributions[player];
                        roundContributions[player] += diff;
                        bettingSystem_->call(player);
                        broadcastCallback_("MOVE: " + player->GetName() + " ������ " +
                            (diff > 0 ? "CALL" : "CHECK") + ".\n");
                    }
                    else if (action.find("RAISE") == 0 || action.find("BET") == 0) {
                        try {
                            size_t spacePos = action.find(" ");
                            if (spacePos == std::string::npos)
                                throw std::invalid_argument("�������� ������ ������� RAISE/BET.");
                            int newBet = std::stoi(action.substr(spacePos + 1));
                            if (newBet <= currentBet) {
                                sendMessageToPlayerCallback_(pid, "������ ������ ���� ������ ������� (" + std::to_string(currentBet) + ").\n");
                                roundInProgress = true;
                                continue;
                            }
                            int diff = currentBet - roundContributions[player];
                            roundContributions[player] += diff + (newBet - currentBet);
                            bettingSystem_->raise(player, diff + (newBet - currentBet));
                            broadcastCallback_("MOVE: " + player->GetName() + " ��������� ������ �� " + std::to_string(newBet) + ".\n");
                            currentBet = newBet;
                        }
                        catch (const std::exception& ex) {
                            sendMessageToPlayerCallback_(pid, std::string("������ ��������� �����: ") + ex.what() + "\n");
                            roundInProgress = true;
                            continue;
                        }
                    }
                    else {
                        sendMessageToPlayerCallback_(pid, "�������� ��������. ���������� ��� ���.\n");
                        roundInProgress = true;
                        continue;
                    }
                }
                // ����� ����� ������ �������� �� ������
                std::this_thread::sleep_for(std::chrono::seconds(2));
            }
            broadcastState();
            firstRound = false;
        }
        broadcastCallback_("CURRENT_BET: " + std::to_string(currentBet) +
            ". POT: " + std::to_string(bettingSystem_->getPot()) + "\n");
    }



    /*
    // 2. ����� ��� ������ ������ ������ (�������� ��� ��� �������� ����� ��������, ��� � ��� ��������� ����)
    //    ���� ������������ �� ��� ���, ���� ���� �� � ������ ��������� ������ ����� ������ ������� ������������ ������.
    void ServerGame::commonBettingRound(std::vector<std::shared_ptr<IPlayer>>& activePlayers,
        std::unordered_map<std::shared_ptr<IPlayer>, int>& roundContributions) {

        
        //��������� �������� ������������ ������.
        int currentBet = bettingSystem_->getCurrentBet();
        bool firstRound = true;
        bool roundInProgress = true;
        while (roundInProgress) {
            roundInProgress = false;  // ������������ ��� ��� �������� ������

            // �������� �� ������ �������� �������
            for (auto& player : activePlayers) {
                // ���������� ����������
                if (!player->IsActive()) continue;

                // ���� ����� ������ ���� ������������ ������, ����������� ��������
                if (roundContributions[player] < currentBet  || firstRound) {
                    roundInProgress = true;  // ������� ������, ������� ���� �����������
                    int pid = player->GetID();
                    sendActions(roundContributions, currentBet, player);
                    broadcastCallback_("ACTION_EXPECTED_FROM: " + std::to_string(player->GetID()) + ".\n");

                    // ������� ����� �� ������ (��������, � ������� 20 ������)
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

                    // ���� ����� �� ������� � ������� ���.
                    if (!responded) {
                        broadcastCallback_(player->GetName() + " �� ������� ������� � �����.\n");
                        bettingSystem_->fold(player);
                       // player->Fold();
                        broadcastCallback_("MOVE: " + player->GetName() + " �� ������� �������.\n");
                        player->SetActive(false);

                        // ���������, ������� �� ������ ���� �������� �����
                        int remainingCount = std::count_if(activePlayers.begin(), activePlayers.end(),
                            [](std::shared_ptr<IPlayer> p) { return p->IsActive(); });

                        if (remainingCount == 1) { // ������� ������ ���� �������� ����� � �� �������
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

                    // ������������ �������� ������.
                    if (action.find("FOLD") == 0) {
                        broadcastCallback_("MOVE: " + player->GetName() + " ������ FOLD.\n");
                        bettingSystem_->fold(player);
                        player->SetActive(false);

                        // ���������, ������� �� ������ ���� �������� �����
                        int remainingCount = std::count_if(activePlayers.begin(), activePlayers.end(),
                            [](std::shared_ptr<IPlayer> p) { return p->IsActive(); });

                        if (remainingCount == 1) { // ������� ������ ���� �������� ����� � �� �������
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
                        broadcastCallback_("MOVE: " + player->GetName() + " ������ " + (diff > 0 ? "CALL" : "CHECK") + ".\n");
                    }
                    else if (action.find("RAISE") == 0 || action.find("BET") == 0) {
                        try {
                            size_t spacePos = action.find(" ");
                            if (spacePos == std::string::npos)
                                throw std::invalid_argument("�������� ������ ������� RAISE/BET.");
                            int newBet = std::stoi(action.substr(spacePos + 1));
                            if (newBet <= currentBet) {
                                sendMessageToPlayerCallback_(pid, "������ ������ ���� ������ ������� (" + std::to_string(currentBet) + ").\n");
                                // ���� ���� �����������, ���� ���� ��������� ���.
                                roundInProgress = true;
                                continue;
                            }
                            int diff = currentBet - roundContributions[player];
                            roundContributions[player] += diff + (newBet - currentBet);
                            bettingSystem_->raise(player, newBet);
                            broadcastCallback_("MOVE: " + player->GetName() + " ��������� ������ �� " + std::to_string(newBet) + ".\n");
                            currentBet = newBet;  // ��������� ������� ������������ ������
                        }
                        catch (const std::exception& ex) {
                            sendMessageToPlayerCallback_(pid, std::string("������ ��������� �����: ") + ex.what() + "\n");
                            roundInProgress = true;
                            continue;
                        }
                    }
                    else {
                        sendMessageToPlayerCallback_(pid, "�������� ��������. ���������� ��� ���.\n");
                        roundInProgress = true;
                        continue;
                    }
                }  
                std::this_thread::sleep_for(std::chrono::seconds(3));
            }  
            broadcastState();
            firstRound = false;
            // ����� ������� �� ���� ������� � ���� ���� �� ���� ����� ���� ����� ���� currentBet,
            // ���� ������������, ����� ��������� ����������� �����.
        }  // while
        broadcastCallback_("CURRENT_BET: " + std::to_string(currentBet) +
            ". POT: " + std::to_string(bettingSystem_->getPot()) + "\n");
      
    }

    */

    
    //������� ����������� �������, ����� ����������� ����� ����� ������
    void ServerGame::fullPreflopRound(std::vector<std::shared_ptr<IPlayer>>& activePlayers,
        std::unordered_map<std::shared_ptr<IPlayer>, int>& roundContributions) {
        // ���� 1: ���������� ������������ ������ (��������)
        
        std::this_thread::sleep_for(std::chrono::seconds(10));
        preflopBettingRound(activePlayers, roundContributions);
        //broadcastState();
        // ���� 2: ����������� ����� ������, ����� ��� ������ ����� �������� ������ ��� �������� �.
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

    // ����� ��� ����������� ������� �������� ����� (������ ������)
    void ServerGame::startGameRounds() {
        // ����� �������� ������ ������ ����� �������� ���� ���������
        stopRequested_ = false;
        if (gameThread_.joinable()) {
            gameThread_.join();
        }
        // ��������� ����� ������� ����� � ��������� ������
        gameThread_ = std::thread(&ServerGame::gameRound, this);
        // ���� ����� ������ �������� ����������, ����� �� �������� join() �����
    }
    void ServerGame::timerLoop() {
        std::unique_lock<std::mutex> lock(timerMutex_);
        while (!stopRequested_) {
            bool reset = timerCv_.wait_for(lock, std::chrono::seconds(30), [this]() { return timerResetRequested_; });
            if (stopRequested_) break;
            if (!reset) {
                if (table_->getPlayers().size() >= 2 && !gameStarted_) {
                    broadcastCallback_("������ �����. ���� ����������� �������������.\n");
                    for (auto& player : table_->getPlayers()) {
                        if (player->GetBalance() < 20) {
                            removePlayer(player ->GetID());
                            std::cout << "Player " << player->GetName() << " ���� ��� ����� ����� ����" << std::endl;
                        }
                        else {
                            player->SetActive(true);
                            std::cout << "Player " << player->GetName() << " ���������� �������� ��� ������ ������" << std::endl;
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
                broadcastCallback_("����� ����� ��������. ������ �������.\n");
              
            }
        }
    }


    // �������� ������� �����.
    // � ������� ������ ��������� ������ �������� ������.
    // ����� ������, ��������� � ��������, ����������� � table_->getPlayers() � ���������� Inactive.
    void ServerGame::gameRound() {
        try {
            roundCancelled_ = false;
            gameStarted_ = true;
            // �������� �������� �������
            auto players = table_->getActivePlayers();
            if (players.empty()) {
                broadcastCallback_("������: ������� ���!\n");
                return;
            }
            broadcastCallback_("START: ����� ���� ����������.\n");
            std::this_thread::sleep_for(std::chrono::seconds(5));
            players = table_->getActivePlayers();
            // ������� ��������� ���� ��� �������� �������
            for (const auto& p : players) {
                try {
                    if (p->IsActive()) {
                        table_->dealStartingCards(p);
                    }
                   
                }
                catch (const std::exception& ex) {
                    std::ostringstream oss;
                    oss << "������ ������� ���� ��� " << p->GetName() << ": " << ex.what() << "\n";
                    broadcastCallback_(oss.str());
                }
            }
            broadcastCallback_("GAME_STATE: �������: ������ ����������.\n");
            std::this_thread::sleep_for(std::chrono::seconds(3));
            sendPlayerCards();
            // �������� ����������� ������ �������� �������
            players = table_->getActivePlayers();
            if (players.size() < 2) {
                broadcastCallback_("������: ������������ ������� ��� ��������.\n");
                return;
            }
           
            // �������������� ������ ������� � ������� ������
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

            // ����: 3 �����
            table_->dealCommunityCard();
            table_->dealCommunityCard();
            table_->dealCommunityCard();
            broadcastCallback_("GAME_STATE: ���� ������.\n");
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

            // Ҹ��
            table_->dealCommunityCard();
            broadcastCallback_("GAME_STATE: Ҹ�� ������.\n");
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

            // �����
            table_->dealCommunityCard();
            broadcastCallback_("GAME_STATE: ����� ������.\n");
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

            // ���������� ������ ��� ��� ���� �������.
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

            // ���������� ����������� �� ������ ���.
            // ��������������, ��� rules_->determineWinners() ���������� ������� � ������� 'evaluations' ��������������� �����������.
            std::vector<int> winnerIndices = rules_->determineWinners(evaluations);
            std::vector<std::shared_ptr<IPlayer>> winners;
            for (int index : winnerIndices) {
                winners.push_back(allPlayers[index]);
            }

            // �������� �������� ����� (side pots)
            auto sidePots = bettingSystem_->getSidePots();

            // ������������ ���� ����� ������������
            auto winnings = PotDistributor::distribute(sidePots, winners);

            // ��������� ������ ����������� � ���������� ��������� �������.
            for (const auto& winner : winners) {
                int playerID = winner->GetID();
                int winAmount = winnings[winner];
                // ��������� ������� � ������� ������
                winner->addToBalance(winAmount);
                // ���������� ��������� ������
                sendMessageToPlayerCallback_(playerID, "SHOWDOWN: �� �������� " + std::to_string(winAmount) + "!\n");
                broadcastCallback_("WINNER: " + winner->GetName() + " ID:" + std::to_string(playerID) + "\n");
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }

            broadcastCallback_("GAME_STATE: ������� ��������. ���� ��������.\n");
            table_->rotatePlayers();

            // ����� ���������� ������ ���������� ���� ������� � ������ Active ��� ���������� ������.
            for (auto& player : table_->getPlayers()) {
                player->SetActive(true);
            }

            RoundEnd();
           
        }
       
        catch (const RoundCancelledException& ex) {
            broadcastCallback_("����� ������: " + std::string(ex.what()) + "\n");
            auto players = table_->getActivePlayers();
            if (players.size() != 0) {
                broadcastCallback_("WINNER: " + players[0]->GetName() + " ID:" + std::to_string(players[0]->GetID()) + "\n");
                players[0]->addToBalance(bettingSystem_->getPot());
            }
            RoundEnd();  
           
        }
        catch (const std::exception& ex) {
            std::cerr << "������ � gameRound: " << ex.what() << std::endl;
            broadcastCallback_("���� ����������� ��-�� ������.\n");
        }

    }
    // ������� ����� ��� �������� ����������� ��������� (���������� ��������)
    void ServerGame::broadcastGameState() {
        broadcastState();
    }

    /*
    #include <windows.h>
    void ServerGame::terminateGameThread() {
        HANDLE threadHandle = OpenThread(THREAD_TERMINATE, FALSE, GetThreadId(gameThread_.native_handle()));
        if (threadHandle) {
            TerminateThread(threadHandle, 0);
            CloseHandle(threadHandle); // ��������� ����������
        }
    }
    // � ������ ServerGame ��������:
 

    // �����, ������� ������������� ���� ���������
    void ServerGame::requestStop() {
        stopRequested_ = true;
        // ���� ������������ �������� ���������� ��� ��������, ��������� �
        timerCv_.notify_all();
    }
    // ���������� ������ �������� �����:
    void ServerGame::startGameRounds() {
        // ����� �������� ������ ������ ������������� ���� ��������� � false
        stopRequested_ = false;
        if (gameThread_.joinable()) {
            //  ����� ���������� stopRequested_ = true � ��������� ����������
            gameThread_.join();
        }

        gameThread_ = std::thread(&ServerGame::gameRound, this);
        // Optionally, ����� �� ����� �������� join(), 
        // ���� ������� ���� ������ �������� ����������
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
                    broadcastCallback_("������ �����. ���� ����������� �������������.\n");
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
                broadcastCallback_("����� ����� ��������. ������ �������.\n");
            }
        }
    }
     
    void ServerGame::gameRound() {
        auto players = table_->getActivePlayers();
        if (players.empty()) {
            broadcastCallback_("������: ������� ���!\n");
            return;
        }
        broadcastCallback_("START: ����� ���� ����������.\n");/////////////////////////////////////

        // ������� ��������� ����
        for (const auto& p : players) {
            try {
                table_->dealStartingCards(p);
            }
            catch (const std::exception& ex) {
                std::ostringstream oss;
                oss << "������ ������� ���� ���";
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
            broadcastCallback_("������: ������������ ������� ��� ��������.\n");
            return;
        }
        // �������������� ������ ������� � ������� ������.
        std::unordered_map<std::shared_ptr<IPlayer>, int> roundContributions;
        for (auto& player : activePlayers) {
            roundContributions[player] = 0;
        
        std::this_thread::sleep_for(std::chrono::seconds(10));
        broadcastState();
        // �������: ��������� ������ (blinds) � ���� ����������� ��������
        fullPreflopRound(activePlayers, roundContributions);///////////////////////////////////////////
        
        activePlayers = table_->getActivePlayers();
        if (activePlayers.size() < 2)
            return;

        
        // ���� (3 �����)
        table_->dealCommunityCard();
        table_->dealCommunityCard();
        table_->dealCommunityCard();
        broadcastCallback_("GAME_STATE: ���� ������.\n"); //////////////////////////////////////////
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

        // Ҹ��
        table_->dealCommunityCard();
        sendTableCards();
        broadcastCallback_("GAME_STATE: Ҹ�� ������.\n");///////////////////////////
        broadcastState();
        commonBettingRound(activePlayers, roundContributions);
        activePlayers = table_->getActivePlayers();
        if (activePlayers.size() < 2)
            return;

        std::this_thread::sleep_for(std::chrono::seconds(2));
        // �����
        table_->dealCommunityCard();
        sendTableCards();
        broadcastCallback_("GAME_STATE: ����� ������.\n");///////////////////////////
        broadcastState();
        commonBettingRound(activePlayers, roundContributions);

        activePlayers = table_->getActivePlayers();
        if (activePlayers.size() < 2)
            return;

        std::vector<HandEvaluation> evaluations;
        auto playerss = table_->getPlayers();
        const auto& communityCards = table_->getCommunityCards();

        std::unordered_map<int, HandEvaluation> playerEvaluations; //  ����������� ID ������ � ��� ������ ����

        for (const auto& player : playerss) {
           
            HandEvaluation eval = rules_->EvaluateHand(player->GetFullHand(communityCards)); //  �������� 7 ����
            evaluations.push_back(eval);
            playerEvaluations[player->GetID()] = eval; //  ��������� ��� ����������� �������������

          //  player->ClearHand();
        }



        // 2. ���������� ����������� �� �� �����������
        std::vector<int> winnerIndices = rules_->determineWinners(evaluations);

        // 3. ����������� ������� ����������� � `shared_ptr<IPlayer>`
        std::vector<std::shared_ptr<IPlayer>> winners;
        for (int index : winnerIndices) {
            winners.push_back(players[index]); //  ������ ������� � `distribute()` `shared_ptr<IPlayer>`
        }
        // 3. �������� ����-����
        auto sidePots = bettingSystem_->getSidePots();

        // 2. ������������ ����
        auto winnings = PotDistributor::distribute(sidePots, winners);

        // 3. ���������� ������� ���������� ��� �������
        for (const auto& winner : winners) {
            int playerID = winner->GetID(); //  �������� ID ������
            int winAmount = winnings[winner]; //  ������ ������� `shared_ptr<IPlayer>`
      
            int win = winnings[winner];
           // winner->addToBalance(win);
            std::cout << "-----------------------------------------" << win << std::endl;

          sendMessageToPlayerCallback_(playerID, "SHOWDOWN: �� �������� " + std::to_string(winAmount) + "!\n");
           broadcastCallback_("WINNER: " + player -> GetName() + " ID:" + std::to_string(player->GetID()) + "\n");/////// ������� ����� �����, 
                                                                                                  //����� ���� ��������� ����������� �� �����������

            std::this_thread::sleep_for(std::chrono::milliseconds(50));

        }


       // RoundEnd();
        broadcastCallback_("GAME_STATE: ������� ��������. ���� ��������.\n");

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
        std::cout << "������������ �����: " << maxValue << std::endl;
        return maxElem->first->GetID();
    }*/


   
      /*  auto activePlayers = table_->getActivePlayers();
        if (activePlayers.size() < 2) {
            std::ostringstream oss;
            oss << "Warrning! ������������ ������� ��� ������.";
            oss << '\n';
            std::string message = oss.str();
            broadcastCallback_(message);
            return;
        }

        // ��� ������� ������ ������ �������� ��� ����� � ���� ������.
        std::unordered_map<std::shared_ptr<IPlayer>, int> roundContributions;
        for (const auto& player : activePlayers) {
            roundContributions[player] = 0;
        }


        int currentBet = bettingSystem_-> getCurrentBet();
        size_t turnIndex = 0;
        bool bettingInProgress = true;

        // ���� ������ ������������ �� ��� ���, ���� ��� �������� ������ �� �������� �����.
        while (bettingInProgress && activePlayers.size() >= 2) {
            auto &currentPlayer = activePlayers[turnIndex];
            int pid = currentPlayer->GetID();

            sendActions(roundContributions, currentBet, currentPlayer);

            broadcastCallback_("��������� �������� �� " + std::to_string(currentPlayer->GetID()) + ".\n");

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
            // ����� �� ������� � ������� ���.
            broadcastCallback_("����� " + currentPlayer->GetName() +
                " �� ������� ������� � �����.\n");
            removePlayer(pid);
            activePlayers = table_->getActivePlayers();
            if (activePlayers.size() < 2)
                break;
            if (turnIndex >= activePlayers.size())
                turnIndex = 0;
            continue;
        }

        // ��������� �������� (������� ���������� �������).
        if (action.find("FOLD") == 0) {
            broadcastCallback_(currentPlayer->GetName() + " ������ FOLD.\n");
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
            broadcastCallback_(currentPlayer->GetName() + " �������� " + (toCall > 0 ? "CALL" : "CHECK") + ".\n");
        }

        else if (action.find("RAISE") == 0 || action.find("BET") == 0) {
            try {
                size_t spacePos = action.find(" ");
                if (spacePos == std::string::npos)
                    throw std::invalid_argument("�������� ������ ������� RAISE/BET.");
                int newBet = std::stoi(action.substr(spacePos + 1));
                if (newBet <= currentBet) {
                    sendMessageToPlayerCallback_(pid, "������ ������ ���� ������ ������� (" + std::to_string(currentBet) + ").\n");
                    continue; // ��������� ��� ������� ������.
                }
                int callAmt = currentBet - roundContributions[pid];
                roundContributions[pid] += callAmt + (newBet - currentBet);
                currentBet = newBet;
                bettingSystem_->raise(currentPlayer, newBet);
                broadcastCallback_(currentPlayer->GetName() + " ������ ���� �� " + std::to_string(newBet) + ".\n");
                // ����� ����� ���������� ���� ����������� ���� ������� ����� �����������.
                turnIndex = (turnIndex + 1) % activePlayers.size();
                continue;
            }
            catch (const std::exception& ex) {
                sendMessageToPlayerCallback_(pid, std::string("������ ��������� �����: ") + ex.what() + "\n");
                continue; // ��������� ������ ��������� ���.
            }
        }
        else {
            sendMessageToPlayerCallback_(pid, "�������� ��������. ���������� ��� ���.\n");
            continue;
        }
        // ��������� � ���������� ������.
        turnIndex = (turnIndex + 1) % activePlayers.size();

        // ���������, ��� �� �������� ������ �������� ������.
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
        broadcastCallback_("������ ���������. ������� ������: " + std::to_string(currentBet) +
            ". ���: " + std::to_string(bettingSystem_->getPot()) + "\n");


        };

            */
    


    /*
    
        void ServerGame::timerLoop() {
        std::unique_lock<std::mutex> lock(timerMutex_); // ������ ���� ����� ����� �������� ������ � ��� ����� ����

        while (!stopRequested_) {
            bool reset = timerCv_.wait_for(lock, std::chrono::seconds(20), [this]() { return timerResetRequested_; });
            if (stopRequested_) break;
            if (!reset) {
                if (table_->getPlayers().size() >= 2 && !gameStarted_) {
                    broadcastCallback_("������ �����. ���� ����������� �������������.\n");
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
                broadcastCallback_("����� ����� ��������. ������ �������.\n");
            }
        }
    }
    */



 
 
  

    /*
    * 
    
    
    // �������� ���� ���� (������ ����).
    void ServerGame::gameRound() {
        auto players = table_->getActivePlayers();
        if (players.empty()) {
            broadcastCallback_("������: ������� ���!\n");
            return;
        }
        broadcastCallback_("START: ����� ���� ����������.\n");

        // �������
        for (const auto& p : players) {
            try {
                table_->dealStartingCards(p);
            }
            catch (const std::exception& ex) {
                std::ostringstream oss;
                oss << "������ ������� ���� ���";
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
        broadcastCallback_("GAME_STATE: �������: ������ ����������.\n");
        std::this_thread::sleep_for(std::chrono::seconds(10));
        bettingRound();
        if (table_->getPlayers().size() < 2) return;

        // ���� (3 �����)
        table_->dealCommunityCard();
        table_->dealCommunityCard();
        table_->dealCommunityCard();
        broadcastCallback_("GAME_STATE: ���� ������.\n");
        broadcastState();
        bettingRound();
        if (table_->getPlayers().size() < 2) return;

        // Ҹ�� (1 �����)
        table_->dealCommunityCard();
        broadcastCallback_("GAME_STATE: Ҹ�� ������.\n");
        broadcastState();
        bettingRound();
        if (table_->getPlayers().size() < 2) return;

        // ����� (1 �����)
        table_->dealCommunityCard();
        broadcastCallback_("GAME_STATE: ����� ������.\n");
        broadcastState();
        bettingRound();

        std::vector<HandEvaluation> evaluations;
        auto playerss = table_->getPlayers();
        const auto& communityCards = table_->getCommunityCards();

        std::unordered_map<int, HandEvaluation> playerEvaluations; //  ����������� ID ������ � ��� ������ ����

        for (const auto& player : playerss) {
            HandEvaluation eval = rules_->EvaluateHand(player->GetFullHand(communityCards)); //  �������� 7 ����
            evaluations.push_back(eval);
            playerEvaluations[player->GetID()] = eval; //  ��������� ��� ����������� �������������
        }


        // 2. ���������� ����������� �� �� �����������
        std::vector<int> winnerIndices = rules_->determineWinners(evaluations);

        // 3. ����������� ������� ����������� � `shared_ptr<IPlayer>`
        std::vector<std::shared_ptr<IPlayer>> winners;
        for (int index : winnerIndices) {
            winners.push_back(players[index]); //  ������ ������� � `distribute()` `shared_ptr<IPlayer>`
        }
        // 3. �������� ����-����
        auto sidePots = bettingSystem_->getSidePots();

        // 2. ������������ ����
        auto winnings = PotDistributor::distribute(sidePots, winners);

        // 3. ���������� ������� ���������� ��� �������
        for (const auto& winner : winners) {
            int playerID = winner->GetID(); //  �������� ID ������
            int winAmount = winnings[winner]; //  ������ ������� `shared_ptr<IPlayer>`
            sendMessageToPlayerCallback_(playerID, "SHOWDOWN: �� �������� " + std::to_string(winAmount) + "!\n");
        }

        broadcastCallback_("GAME_STATE: ������� ��������. ���� ��������.\n");
    }

    

    * 
    void playHand(); 
    void startGame(); // ��������� ���������� �������, ��������� playHand() � ������������� gameStarted_ = true
    void advanceNextRound();
    void processTurn();

    * 
    * 
       // �������� ����� ��� ���� (���� ����).
    // ��������� ������ ������������������: ������� �������� ����, ����������� ������ ������, ��������� ���������?����, �������.
    void ServerGame::playHand() {
        // ��������� ���������: ���������� ���� � ����.
        table_->resetTable();
        bettingSystem_->resetPot();

        // ������� ��������� ���� ������� ������.
        auto players = table_->getPlayers();
        for (const auto& player : players) {
            try {
                table_->dealStartingCards(player);
                player->SetActive(true);
            }
            catch (const std::exception& ex) {
                broadcastCallback_("������ ������� ���� ��� " + player->GetName() + ": " + ex.what() + "\n");
            }
        }
        // ���������� ������� ��� �����.
        sendPlayerCards();
        broadcastState();
        if (table_->getPlayers().size() < 2) return;
        // �������: ���� ������.
        broadcastCallback_("�������: ������ ����������.\n");
        bettingRound();
        if (table_->getPlayers().size() < 2) return;


        // ����: ���������� 3 ��������� �����.
        table_->dealCommunityCard();
        table_->dealCommunityCard();
        table_->dealCommunityCard();
        broadcastCallback_("���� ������.\n");
        broadcastState();
        bettingRound();
        if (table_->getPlayers().size() < 2) return;

        // Ҹ��: ���� �����.
        table_->dealCommunityCard();
        broadcastCallback_("Ҹ�� ������.\n");
        broadcastState();
        bettingRound();
        if (table_->getPlayers().size() < 2) return;

        // �����: ��������� ��������� �����.
        table_->dealCommunityCard();
        broadcastCallback_("����� ������.\n");
        broadcastState();
        bettingRound();

        std::vector<HandEvaluation> evaluations;
        auto playerss = table_->getPlayers();
        const auto& communityCards = table_->getCommunityCards();

        std::unordered_map<int, HandEvaluation> playerEvaluations; //  ����������� ID ������ � ��� ������ ����

        for (const auto& player : playerss) {
            HandEvaluation eval = rules_->EvaluateHand(player->GetFullHand(communityCards)); //  �������� 7 ����
            evaluations.push_back(eval);
            playerEvaluations[player->GetID()] = eval; //  ��������� ��� ����������� �������������
        }


        // 2. ���������� ����������� �� �� �����������
        std::vector<int> winnerIndices = rules_->determineWinners(evaluations);

        // 3. ����������� ������� ����������� � `shared_ptr<IPlayer>`
        std::vector<std::shared_ptr<IPlayer>> winners;
        for (int index : winnerIndices) {
            winners.push_back(players[index]); //  ������ ������� � `distribute()` `shared_ptr<IPlayer>`
        }
        // 3. �������� ����-����
        auto sidePots = bettingSystem_->getSidePots();

        // 2. ������������ ����
        auto winnings = PotDistributor::distribute(sidePots, winners);

        // 3. ���������� ������� ���������� ��� �������
        for (const auto& winner : winners) {
            int playerID = winner->GetID(); //  �������� ID ������
            int winAmount = winnings[winner]; //  ������ ������� `shared_ptr<IPlayer>`
            sendMessageToPlayerCallback_(playerID, "SHOWDOWN: �� �������� " + std::to_string(winAmount) + "!\n");
        }

        broadcastCallback_("������� ��������. ���� ��������.\n");
    }

    void ServerGame::startGame() {
        std::lock_guard<std::mutex> lock(gameMutex_);

        // ���������, ���������� �� ������� ��� ������ ����
        if (table_->getPlayers().size() < 2) {
            broadcastCallback_("����� ������� 2 ������ ��� ������ ����.\n");
            return;
        }

        // ���� ���� ��� �� ��������, �������� ����� ����
        if (!gameStarted_) {
            broadcastCallback_("���� ����������!\n");

            gameStarted_ = true;
            playHand();  //  ��������� ����������� ����� ����
            gameStarted_ = false; //  ��������� ���� ����� ����
        }
    }

    void ServerGame::advanceNextRound() {
    }

    // ���������� ��������� ���� � ��� ������� ������ �������� ������� (� �������� ������ ���� ����� ������� ��������� ������ �������)
    void ServerGame::processTurn() {
        advanceNextRound();
        broadcastState();
    }


    */