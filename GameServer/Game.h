#pragma once
#include "IGame.h"
#include "ITable.h"
#include "IGameRules.h"
#include "IBettingSystem.h"
#include "IGameView.h"
#include "IGameInput.h"
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>

class Game {
private:
    std::unique_ptr<ITable> table;
    std::unique_ptr<IGameRules> rules;
    std::unique_ptr<IBettingSystem> bettingSystem;
    std::vector<std::unique_ptr<IRound>> rounds;

    std::thread gameThread;
    mutable std::mutex gameMutex;
    std::condition_variable cv;
    bool stopRequested;
    size_t currentRoundIndex;

public:
    Game(std::unique_ptr<ITable> tableImpl,
        std::unique_ptr<IGameRules> rulesImpl,
        std::unique_ptr<IBettingSystem> bettingSystemImpl,
        std::vector<std::unique_ptr<IRound>> roundsImpl)
        : table(std::move(tableImpl)),
        rules(std::move(rulesImpl)),
        bettingSystem(std::move(bettingSystemImpl)),
        rounds(std::move(roundsImpl)),
        stopRequested(false),
        currentRoundIndex(0) {
    }

    void startGame();
    void gameLoop();
    void stopGame();
    void addPlayer(const IPlayer& player);
    void removePlayer(int playerIndex);
    void processTurn();
    bool isGameContinuing() const;
    const IRound* getCurrentRound() const;
    void advanceToNextRound();

    ~Game();
};


