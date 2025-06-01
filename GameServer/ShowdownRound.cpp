#include "ShowdownRound.h"
#include "ITable.h"
#include "IPlayer.h"
#include "TexasHoldemRules.h"
#include <vector>
#include <string>

//void ShowdownRound::startRound(IBettingSystem& bettingSystem, IGameView& view, IGameInput& input) {
//    view.displayMessage("Этап Showdown начался.");
//
//    ITable* table = bettingSystem.getTable();
//    // Все активные игроки показывают свои карты
//    auto players = table->getActivePlayers();
//    for (IPlayer* player : players) {
//        view.displayPlayerHand(*player);
//    }
//
//    // Для каждого игрока собираем полную руку: 2 карманные + 5 общих карт
//    std::vector<HandEvaluation> evaluations;
//    for (IPlayer* player : players) {
//        std::vector<Cards> fullHand = player->GetHand(); // Карманные карты
//        const auto& communityCards = table->getCommunityCards();
//        for (const auto& cardPtr : communityCards) {
//            fullHand.push_back(*cardPtr);
//        }
//        // Оцениваем руку (здесь предполагается, что TexasHoldemRules реализует полноценную оценку)
//        TexasHoldemRules rules;
//        HandEvaluation eval = rules.EvaluateHand(fullHand);
//        evaluations.push_back(eval);
//    }
//
//    // Определяем победителя(ов)
//    TexasHoldemRules rules;
//    std::vector<int> winnerIndexes = rules.determineWinners(evaluations);
//    if (!winnerIndexes.empty()) {
//        std::string resultMsg = "Победитель(и): ";
//        for (int idx : winnerIndexes) {
//            resultMsg += players[idx]->GetName() + " ";
//        }
//        view.displayMessage(resultMsg);
//    }
//    else {
//        view.displayMessage("Победитель не определен.");
//    }
//}
//
//void ShowdownRound::endRound(IBettingSystem& bettingSystem, IGameView& view) {
//    view.displayMessage("Этап Showdown завершен.");
//}
//
//std::string ShowdownRound::getRoundType() const {
//    return "Showdown";
//}
