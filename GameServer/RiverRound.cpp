#include "RiverRound.h"
#include "ITable.h"
#include "IPlayer.h"

//void RiverRound::startRound(IBettingSystem& bettingSystem, IGameView& view, IGameInput& input) {
//    view.displayMessage("Этап River начался.");
//    ITable* table = bettingSystem.getTable();
//    // Выкладываем последнюю (5-ю) общую карту
//    table->dealCommunityCard();
//    view.displayMessage("На столе выложена карта для River.");
//
//    auto players = table->getActivePlayers();
//    view.displayMessage("Раунд ставок (River) начинается.");
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
//void RiverRound::endRound(IBettingSystem& bettingSystem, IGameView& view) {
//    view.displayMessage("Этап River завершен.");
//}
//
//std::string RiverRound::getRoundType() const {
//    return "River";
//}
