#include "FlopRound.h"
#include "ITable.h"
#include "IPlayer.h"

//void FlopRound::startRound(IBettingSystem& bettingSystem, IGameView& view, IGameInput& input) {
//    view.displayMessage("���� Flop �������.");
//    ITable* table = bettingSystem.getTable();
//    // ����������� 3 ����� �����
//    for (int i = 0; i < 3; ++i) {
//        table->dealCommunityCard();
//    }
//    view.displayMessage("�� ����� �������� 3 ����� �����.");
//
//    // ����� ������ ��� Flop
//    auto players = table->getActivePlayers();
//    view.displayMessage("����� ������ (Flop) ����������.");
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
//void FlopRound::endRound(IBettingSystem& bettingSystem, IGameView& view) {
//    view.displayMessage("���� Flop ��������.");
//}
//
//std::string FlopRound::getRoundType() const {
//    return "Flop";
//}
