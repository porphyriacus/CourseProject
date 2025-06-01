#include "TurnRound.h"
#include "ITable.h"
#include "IPlayer.h"

//void TurnRound::startRound(IBettingSystem& bettingSystem, IGameView& view, IGameInput& input) {
//    view.displayMessage("Этап Turn начался.");
//    ITable* table = bettingSystem.getTable();
//    // Выкладываем 1 общую карту для Turn
//    table->dealCommunityCard();
//    view.displayMessage("На столе выложена карта для Turn.");
//
//    auto players = table->getActivePlayers();
//    view.displayMessage("Раунд ставок (Turn) начинается.");
//    for (IPlayer* player : players) {
//        std::string action = input.promptAction(*player, /*currentBet=*/0);
//        if (action == "Fold") {
//            bettingSystem.fold(player);
//        }
//        else if (action == "Call") {
//            bettingSystem.call(player);
//        }
//        else if (action == "Raise") {
//            int bet = input.promptBet(*player, 10);
//            bettingSystem.raise(player, bet);
//        }
//    }
//}
//
//void TurnRound::endRound(IBettingSystem& bettingSystem, IGameView& view) {
//    view.displayMessage("Этап Turn завершен.");
//}
//
//std::string TurnRound::getRoundType() const {
//    return "Turn";
//}
