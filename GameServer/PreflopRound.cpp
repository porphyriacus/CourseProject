#include "PreflopRound.h"
#include "ITable.h"
#include "IPlayer.h"

//void PreflopRound::startRound(IBettingSystem& bettingSystem, IGameView& view, IGameInput& input) {
//    view.displayMessage("���� Preflop �������.");
//    // �������� �������� ������� ��� ������� ��������� ����
//    ITable* table = bettingSystem.getTable();  // ��������������, ��� BettingSystem ����� ����� getTable()
//    auto players = table->getActivePlayers();
//
//    // ������� ������� ������ �� 2 ��������� �����
//    for (IPlayer* player : players) {
//        table->dealStartingCards(*player);
//    }
//    view.displayMessage("��������� ����� �������.");
//
//    // ����� ������ � ��� ������� ��������� ������
//    view.displayMessage("����� ������ (Preflop) ����������.");
//    for (IPlayer* player : players) {
//        // ����� ����� ����� ����� ��������� ������ ������:
//        std::string action = input.promptAction(*player, /*currentBet=*/0);
//        if (action == "Fold") {
//            bettingSystem.fold(player);
//        }
//        else if (action == "Call") {
//            bettingSystem.call(player);
//        }
//        else if (action == "Raise") {
//            int bet = input.promptBet(*player, 10); // ��������, ����������� ������ 10
//            bettingSystem.raise(player, bet);
//        }
//    }
//}
//
//void PreflopRound::endRound(IBettingSystem& bettingSystem, IGameView& view) {
//    view.displayMessage("����� Preflop ��������.");
//}
//
//std::string PreflopRound::getRoundType() const {
//    return "Preflop";
//}
//
