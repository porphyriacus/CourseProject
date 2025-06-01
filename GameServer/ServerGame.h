#pragma once

#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <vector>
#include <memory>
#include <sstream>
#include <chrono>
#include <iostream>
#include <functional>
#include <deque>
#include <unordered_map>
#include <stdexcept>
#include <algorithm> 

#include "ITable.h"
#include "IBettingSystem.h"
#include "IGameRules.h"
#include "IRound.h"
#include "IPlayer.h"
#include "Deck.h"
#include "TexasHoldemRules.h"
#include "PreFlopRound.h"
#include "PotDistributor.h"  // для распределения банка

#include <exception>



class ServerGame {
public:
    ServerGame(std::unique_ptr<ITable> table,
        std::unique_ptr<BettingSystem> bettingSystem,
        std::unique_ptr<TexasHoldemRules> rules,
        std::function<void(const std::string&)> broadcastCallback,
        std::function<void(int, const std::string&)> sendMessageToPlayerCallback);

    ~ServerGame();
    void stop();

    ////////////////////////////////////
        // Метод, который устанавливает флаг остановки
    void requestStop();
  //  void terminateGameThread();

    void commonBettingRound(std::vector<std::shared_ptr<IPlayer>>& activePlayers,
        std::unordered_map<std::shared_ptr<IPlayer>, int>& roundContributions);
    void ccommonBettingRound(std::vector<std::shared_ptr<IPlayer>>& activePlayers,
        std::unordered_map<std::shared_ptr<IPlayer>, int>& roundContributions);
    void preflopBettingRound(std::vector<std::shared_ptr<IPlayer>>& activePlayers,
        std::unordered_map<std::shared_ptr<IPlayer>, int>& roundContributions);
    void fullPreflopRound(std::vector<std::shared_ptr<IPlayer>>& activePlayers,
        std::unordered_map<std::shared_ptr<IPlayer>, int>& roundContributions);
    ////////////////////////////////

    void processPlayerAction(int playerID, const std::string& action);

    void addPlayer(std::shared_ptr<IPlayer> player); // добавляет игрока за стол, если игра не началась, делает активным
    void removePlayer(int playerID); // удаляет игрока со стола следовательно из игры

    void RoundEnd();

    void sendPlayerCards(); // составляет сообщение из карт которые находятся на руках у игрока

    void sendActions(std::unordered_map<std::shared_ptr<IPlayer>, int>& roundContributions, int currentBet, std::shared_ptr<IPlayer> pl);
    void handlePlayerAction(int pid, const std::string& action);

    void sendTableCards();
    void timerLoop();
    void startGameRounds(); // запускает игровой процесс в новом потоке (gameThread_)

    void broadcastState(); // обновляет состояние стола, отправляя игрокам баланс, статус и карты
    void broadcastGameState(); // вызывает broadcastState() для отправки актуального состояния

    void gameRound();

    bool isRoundCancelled() const {
        return roundCancelled_;
    }

    void cancelRound();
    void checkActivePlayerCount();

private:
    std::atomic<bool> roundCancelled_{ false };
    std::atomic<bool> stopRequested_{ false };  // Флаг остановки игрового цикла

    std::unique_ptr<ITable> table_;
    std::unique_ptr<BettingSystem> bettingSystem_;
    std::unique_ptr<TexasHoldemRules> rules_;
    std::vector<std::unique_ptr<IRound>> rounds_;

    std::function<void(const std::string&)> broadcastCallback_;
    std::function<void(int, const std::string&)> sendMessageToPlayerCallback_;

    std::mutex gameMutex_;
    std::atomic<bool> gameStarted_;
  //  std::atomic<bool> stopRequested_; // говорит серверу, нужно ли выключить все потоки

    std::mutex timerMutex_;
    std::condition_variable timerCv_; // управляет ожиданием 
    bool timerResetRequested_;
    std::thread timerThread_;
    std::thread gameThread_;

    std::mutex actionsMutex_;
    std::condition_variable actionsCv_;
    std::deque<std::pair<int, std::string>> actionsQueue_;
};

/*
// Класс ServerGame реализует всю логику игры на стороне сервера.
// Он управляет столом, системой ставок, правилами и раундами.
// Сообщения рассылаются через два callback-функции:
//   • broadcastCallback_ – для рассылки сообщений всем игрокам;
//   • sendMessageToPlayerCallback_ – для отправки персональных сообщений (например, запрос действия или карта на руках).
class ServerGame {
public:



    ServerGame(std::unique_ptr<ITable> table,
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
        timerThread_ = std::thread(&ServerGame::timerLoop, this);
    }
     void stop() {
        stopRequested_ = true;
        timerCv_.notify_all();
        if (timerThread_.joinable())
            timerThread_.join();
        if (gameThread_.joinable())
            gameThread_.join();
    }
    ~ServerGame() {
        stop();
    }

    // Метод, который сервер вызывает при получении действия от игрока (например, через сетевой компонент).
    // Действие добавляется в очередь для обработки в bettingRound.
    void processPlayerAction(int playerID, const std::string& action) {
        {
            std::lock_guard<std::mutex> lock(actionsMutex_);
            actionsQueue_.push_back({ playerID, action });
        }
        actionsCv_.notify_all();
    }

    // Добавление нового игрока.
    void addPlayer(std::shared_ptr<IPlayer> player) {
        std::lock_guard<std::mutex> lock(gameMutex_);
        auto players = table_->getPlayers();
        if (std::any_of(players.begin(), players.end(),
            [&](const std::shared_ptr<IPlayer>& p) { return p->GetID() == player->GetID(); })) {
            std::cerr << "Ошибка: Игрок с ID " << player->GetID() << " уже существует!\n";
            return;
        }
        if (players.size() < 5) {
            if (!gameStarted_) {
                player->SetActive(true);
                std::cout << "сын шлюхи" << std::endl;
            }
            std::cout<<player->IsActive()<<std::endl;
            table_->addPlayer(player);
            broadcastState();
        }
        else {
            broadcastCallback_("Игра заполнена. Игрок " + player->GetName() + " не может быть добавлен.\n");
        }
    }

    // Удаление игрока по ID.
    void removePlayer(int playerID) {
        std::lock_guard<std::mutex> lock(gameMutex_);
        table_->removePlayer(playerID);
        auto players = table_->getPlayers();
        if (!players.empty()) {
            broadcastState();

            broadcastCallback_("Игрок с ID " + std::to_string(playerID) + " покинул игру.\n");
        }
        else {
            broadcastCallback_("Ошибка: после удаления игроков стол пуст!\n");
        }
    }

    // Отправка карт (карманных) каждому игроку с их текстовыми идентификаторами.
    void sendPlayerCards() {
        auto players = table_->getPlayers();
        for (const auto& player : players) {
            std::ostringstream oss;
            oss << "HANDS_CARDS: ";
            oss << player->GetHand();
            oss << '\n';
            std::string message = oss.str();
            sendMessageToPlayerCallback_(player->GetID(), message);
        }
    }

    // Основной метод для игры (одна рука).
    // Реализует полную последовательность: раздача массовых карт, циклические раунды ставок, раскрытие комьюнити‑карт, шоудаун.
    void playHand() {
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

    // Реализация цикла ставок с поддержкой цикличности.
    // Каждый игрок по очереди получает возможность действовать, пока все активные игроки не сравняют свои ставки.
    void bettingRound() {
        auto activePlayers = table_->getActivePlayers();
        if (activePlayers.size() < 2) {
            std::ostringstream oss;
            oss << "Warrning! Недостаточно игроков для ставки.";
            oss << '\n';
            std::string message = oss.str();
            broadcastCallback_(message);
            return;
        }

        // Для каждого игрока храним внесённый ими вклад в этом раунде.
        std::unordered_map<int, int> roundContributions;
        for (const auto& player : activePlayers) {
            roundContributions[player->GetID()] = 0;
        }
        int currentBet = 0;
        size_t turnIndex = 0;
        bool bettingInProgress = true;

        // Цикл ставок продолжается до тех пор, пока все активные игроки не сравняют вклад.
        while (bettingInProgress && activePlayers.size() >= 2) {
            auto &currentPlayer = activePlayers[turnIndex];
            int pid = currentPlayer->GetID();
           //// std::cout << "========================================="<<pid << std::endl;
           // std::ostringstream oss;
           // std::string mess = "ВАШ ХОД: Текущая ставка = " + std::to_string(currentBet) +
           //                    ", Ваш вклад = " + std::to_string(roundContributions[pid]) + 
           //                    ". Действия: CHECK/CALL, RAISE <amt>, FOLD.\n";
           // oss << "ВАШ ХОД: Текущая ставка = ";
           // oss << std::to_string(currentBet);
           // oss << ", Ваш вклад = ";
           // oss << std::to_string(roundContributions[pid]);
           // oss << ". Действия: CHECK/CALL, RAISE <amt>, FOLD.";
           // oss << '\n';
           // std::string message = oss.str();

            sendMessageToPlayerCallback_(pid, "ВАШ ХОД: Текущая ставка = " + std::to_string(currentBet) +
                ", Ваш вклад = " + std::to_string(roundContributions[pid]) +
                ". Действия: CHECK/CALL, RAISE <amt>, FOLD.\n");

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
            /*
            bool actionReceived = false;
            std::string action;
            {
                std::unique_lock<std::mutex> lock(actionsMutex_);
                actionReceived = actionsCv_.wait_for(lock, std::chrono::seconds(60), [this, pid]() -> bool {
                    for (const auto& act : actionsQueue_) {
                        if (act.first == pid)
                            return true;
                    }
                    return false;
                    });
                if (actionReceived) {
                    for (auto it = actionsQueue_.begin(); it != actionsQueue_.end(); ++it) {
                        if (it->first == pid) {
                            action = it->second;
                            actionsQueue_.erase(it);
                            break;
                        }
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
                removePlayer(pid);
                activePlayers = table_->getActivePlayers();
                if (activePlayers.size() < 2)
                    break;
                if (turnIndex >= activePlayers.size())
                    turnIndex = 0;
                continue;
            }
            else if (action.find("CALL") == 0 || action.find("CHECK") == 0) {
                int toCall = currentBet - roundContributions[pid];
                roundContributions[pid] += toCall;
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
    }

    // Таймер, который запускается при отсутствии игроков.
    void timerLoop() {
        std::unique_lock<std::mutex> lock(timerMutex_);
        while (!stopRequested_) {
            bool reset = timerCv_.wait_for(lock, std::chrono::seconds(30), [this]() { return timerResetRequested_; });
            if (stopRequested_) break;
            if (!reset) {
                if (table_->getPlayers().size() >= 2 && !gameStarted_) {
                    broadcastCallback_("Таймер истек. Игра запускается автоматически.\n");
                    for (auto &player : table_->getPlayers()) {
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

    // Запуск игрового цикла в отдельном потоке.
    void startGameRounds() {
        gameThread_ = std::thread(&ServerGame::gameRound, this);
    }

    void startGame() {
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

    void advanceNextRound() {
    }

    // Простейшая обработка хода – для примера просто проходит переход (в реальной логике сюда можно вложить обработку команд игроков)
    void processTurn() {
        advanceNextRound();
        broadcastState();
    }
    // Вывод состояния стола: формируется строка с именами игроков, балансами и статусами.
    void broadcastState() {
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

    // Основной цикл игры (начало руки).
    void gameRound() {
        auto players = table_->getPlayers();
        if (players.empty()) {
            broadcastCallback_("Ошибка: игроков нет!\n");
            return;
        }
        broadcastCallback_("Новая рука начинается.\n");

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
        broadcastState();
        broadcastCallback_("Префлоп: ставки начинаются.\n");
        bettingRound();
        if (table_->getPlayers().size() < 2) return;

        // Флоп (3 карты)
        table_->dealCommunityCard();
        table_->dealCommunityCard();
        table_->dealCommunityCard();
        broadcastCallback_("Флоп открыт.\n");
        broadcastState();
        bettingRound();
        if (table_->getPlayers().size() < 2) return;

        // Тёрн (1 карта)
        table_->dealCommunityCard();
        broadcastCallback_("Тёрн открыт.\n");
        broadcastState();
        bettingRound();
        if (table_->getPlayers().size() < 2) return;

        // Ривер (1 карта)
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
    // Внешний метод для отправки актуального состояния (вызывается сервером)
    void broadcastGameState() {
        broadcastState();
    }

private:
    std::unique_ptr<ITable> table_;
    std::unique_ptr<BettingSystem> bettingSystem_;
    std::unique_ptr<TexasHoldemRules> rules_;
    std::vector<std::unique_ptr<IRound>> rounds_;

    std::function<void(const std::string&)> broadcastCallback_;
    std::function<void(int, const std::string&)> sendMessageToPlayerCallback_;

    std::mutex gameMutex_;
    std::atomic<bool> gameStarted_;
    std::atomic<bool> stopRequested_;

    std::mutex timerMutex_;
    std::condition_variable timerCv_;
    bool timerResetRequested_;
    std::thread timerThread_;

    std::thread gameThread_;

    std::mutex actionsMutex_;
    std::condition_variable actionsCv_;
    std::deque<std::pair<int, std::string>> actionsQueue_;
};
*/