#include "ShowdownRound.h"
#include "ITable.h"
#include "IPlayer.h"
#include "TexasHoldemRules.h"
#include <vector>
#include <string>

//void ShowdownRound::startRound(IBettingSystem& bettingSystem, IGameView& view, IGameInput& input) {
//    view.displayMessage("���� Showdown �������.");
//
//    ITable* table = bettingSystem.getTable();
//    // ��� �������� ������ ���������� ���� �����
//    auto players = table->getActivePlayers();
//    for (IPlayer* player : players) {
//        view.displayPlayerHand(*player);
//    }
//
//    // ��� ������� ������ �������� ������ ����: 2 ��������� + 5 ����� ����
//    std::vector<HandEvaluation> evaluations;
//    for (IPlayer* player : players) {
//        std::vector<Cards> fullHand = player->GetHand(); // ��������� �����
//        const auto& communityCards = table->getCommunityCards();
//        for (const auto& cardPtr : communityCards) {
//            fullHand.push_back(*cardPtr);
//        }
//        // ��������� ���� (����� ��������������, ��� TexasHoldemRules ��������� ����������� ������)
//        TexasHoldemRules rules;
//        HandEvaluation eval = rules.EvaluateHand(fullHand);
//        evaluations.push_back(eval);
//    }
//
//    // ���������� ����������(��)
//    TexasHoldemRules rules;
//    std::vector<int> winnerIndexes = rules.determineWinners(evaluations);
//    if (!winnerIndexes.empty()) {
//        std::string resultMsg = "����������(�): ";
//        for (int idx : winnerIndexes) {
//            resultMsg += players[idx]->GetName() + " ";
//        }
//        view.displayMessage(resultMsg);
//    }
//    else {
//        view.displayMessage("���������� �� ���������.");
//    }
//}
//
//void ShowdownRound::endRound(IBettingSystem& bettingSystem, IGameView& view) {
//    view.displayMessage("���� Showdown ��������.");
//}
//
//std::string ShowdownRound::getRoundType() const {
//    return "Showdown";
//}
