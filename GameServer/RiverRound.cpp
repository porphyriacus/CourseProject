#include "RiverRound.h"
#include "ITable.h"
#include "IPlayer.h"

//void RiverRound::startRound(IBettingSystem& bettingSystem, IGameView& view, IGameInput& input) {
//    view.displayMessage("���� River �������.");
//    ITable* table = bettingSystem.getTable();
//    // ����������� ��������� (5-�) ����� �����
//    table->dealCommunityCard();
//    view.displayMessage("�� ����� �������� ����� ��� River.");
//
//    auto players = table->getActivePlayers();
//    view.displayMessage("����� ������ (River) ����������.");
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
//    view.displayMessage("���� River ��������.");
//}
//
//std::string RiverRound::getRoundType() const {
//    return "River";
//}
