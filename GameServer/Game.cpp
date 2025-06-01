//#include "Game.h"
//Game::Game(std::unique_ptr<ITable> tableImpl,
//    std::unique_ptr<IGameRules> rulesImpl,
//    std::unique_ptr<IBettingSystem> bettingSystemImpl,
//    std::unique_ptr<IGameView> viewImpl,
//    std::unique_ptr<IGameInput> inputImpl,
//    std::vector<std::unique_ptr<IRound>> roundsImpl)
//    : table(std::move(tableImpl)),
//    rules(std::move(rulesImpl)),
//    bettingSystem(std::move(bettingSystemImpl)),
//    view(std::move(viewImpl)),
//    input(std::move(inputImpl)),
//    rounds(std::move(roundsImpl)),
//    stopRequested(false),
//    currentRoundIndex(0) {
//}
//
//void Game::startGame() {
//    {
//        std::lock_guard<std::mutex> lock(gameMutex);
//        stopRequested = false;
//    }
//    gameThread = std::thread(&Game::gameLoop, this);
//}
//
//void Game::gameLoop() {
//    view->displayMessage("???? ????????.");
//    while (true) {
//        {
//            std::lock_guard<std::mutex> lock(gameMutex);
//            if (stopRequested || currentRoundIndex >= rounds.size()) {
//                view->displayMessage("???? ?????????.");
//                break;
//            }
//        }
//
//        IRound* currentRound = rounds[currentRoundIndex].get();
//        view->displayMessage("????: " + currentRound->getRoundType());
//        currentRound->startRound(*bettingSystem, *view, *input);
//        currentRound->endRound(*bettingSystem, *view);
//
//        advanceToNextRound();
//
//        std::unique_lock<std::mutex> lock(gameMutex);
//        cv.wait_for(lock, std::chrono::milliseconds(100), [this] { return stopRequested; });
//    }
//}
//
//void Game::stopGame() {
//    {
//        std::lock_guard<std::mutex> lock(gameMutex);
//        stopRequested = true;
//    }
//    cv.notify_all();
//    if (gameThread.joinable())
//        gameThread.join();
//    view->displayMessage("???? ???????????.");
//}
//
//void Game::addPlayer(const IPlayer& player) {
//    table->addPlayer(player);
//}
//
//void Game::removePlayer(int playerIndex) {
//    table->removePlayer(playerIndex);
//}
//
//void Game::processTurn() {
//    advanceToNextRound();
//}
//
//bool Game::isGameContinuing() const {
//    return table->getActivePlayers().size() >= 2;
//}
//
//const IRound* Game::getCurrentRound() const {
//    if (currentRoundIndex < rounds.size()) {
//        return rounds[currentRoundIndex].get();
//    }
//    return nullptr;
//}
//
//void Game::advanceToNextRound() {
//    std::lock_guard<std::mutex> lock(gameMutex);
//    if (currentRoundIndex < rounds.size()) {
//        ++currentRoundIndex;
//    }
//}
//
//Game::~Game() {
//    stopGame();
//}