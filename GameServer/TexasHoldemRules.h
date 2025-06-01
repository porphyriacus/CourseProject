#pragma once
#include "IGameRules.h"

#include <algorithm>
#include <array>
#include <map>
#include <iostream>
#include <vector>
#include <stdexcept>


// преобразование типа руки в строку 
inline std::string handRankToString(HandRank hr) {
    switch (hr) {
    case HandRank::HighCard:       return "High Card";
    case HandRank::Pair:           return "Pair";
    case HandRank::TwoPair:        return "Two Pair";
    case HandRank::ThreeOfAKind:   return "Three of a Kind";
    case HandRank::Straight:       return "Straight";
    case HandRank::Flush:          return "Flush";
    case HandRank::FullHouse:      return "Full House";
    case HandRank::FourOfAKind:    return "Four of a Kind";
    case HandRank::StraightFlush:  return "Straight Flush";
    case HandRank::RoyalFlush:     return "Royal Flush";
    default:                       return "Unknown";
    }
}

class TexasHoldemRules : public IGameRules
{
    //  сравнениу двух оценок рук
    int compareHands(const HandEvaluation& hand1, const HandEvaluation& hand2) const;
public:
    TexasHoldemRules() = default;
    // функция для определения победителя среди нескольких комбинаций
    // Функция возвращает вектор индексов победивших игроков (если несколько - в случае ничьей)
    std::vector<int> determineWinners(const std::vector<HandEvaluation>& evaluations) const override;

    HandEvaluation EvaluateHand(const std::vector<Cards>& cards) const override;
};

/* перебор 21 кмбинации тяжко
HandEvaluation evaluate5CardHand(const std::vector<Cards>& hand) {
    if (hand.size() != 5) {
        throw std::runtime_error("5 карт на проверку пупу");
    }

    // сортируем по убыванию ранга
    std::vector<Cards> sortedCards = hand;
    std::sort(sortedCards.begin(), sortedCards.end(),
        [](const Cards& a, const Cards& b) {
            return a.GetRank() > b.GetRank();
        });

    //для рангов (индексы 2..14) и мастей (индексы 0..3)
    std::array<int, 15> rankFrequency{};
    std::array<int, 4> suitFrequency{};  // для мастей

    for (const auto& card : sortedCards) {
        ++rankFrequency[card.GetRank()];
        ++suitFrequency[card.GetSuit()];
    }

    //флеш: все 5 карт должны быть одной масти
    bool isFlush = false;
    for (const int count : suitFrequency) {
        if (count == 5) {
            isFlush = true;
            break;
        }
    }

    // Поиск стрита
    bool isStraight = false;
    int straightHigh = 0;
    for (int i = 14; i >= 5; --i) {
        bool consecutive = true;
        for (int j = 0; j < 5; ++j) {
            if (rankFrequency[i - j] == 0) {
                consecutive = false;
                break;
            }
        }
        if (consecutive) {
            isStraight = true;
            straightHigh = i;
            break;
        }
    }
    // Особый случай: A-2-3-4-5 (в отсортированном виде: 14, 5, 4, 3, 2)
    if (!isStraight &&
        rankFrequency[14] && rankFrequency[2] && rankFrequency[3] &&
        rankFrequency[4] && rankFrequency[5]) {
        isStraight = true;
        straightHigh = 5;
    }

    // Подсчет количества карт одинакового ранга
    std::vector<int> quads, trips, pairs, singles;
    for (int r = 14; r >= 2; --r) {
        if (rankFrequency[r] == 4)
            quads.push_back(r);
        else if (rankFrequency[r] == 3)
            trips.push_back(r);
        else if (rankFrequency[r] == 2)
            pairs.push_back(r);
        else if (rankFrequency[r] == 1)
            singles.push_back(r);
    }

    HandEvaluation eval;

    // проверяем стрейт-флеш и роял флэш
    if (isFlush && isStraight) {
        // если самая высокая карта – Ace и самая низкая равна 10, считаем это роял флэш.
        if (sortedCards[0].GetRank() == 14 && sortedCards[4].GetRank() == 10) {
            eval.rank = HandRank::RoyalFlush;
            eval.kickers.push_back(straightHigh); // Кикер для справки
            return eval;
        }
        eval.rank = HandRank::StraightFlush;
        eval.kickers.push_back(straightHigh);
        return eval;
    }

    if (!quads.empty()) {
        eval.rank = HandRank::FourOfAKind;
        eval.kickers.push_back(quads[0]);
        if (!singles.empty()) {
            eval.kickers.push_back(singles[0]);
        }
        return eval;
    }

    if (!trips.empty() && (!pairs.empty() || trips.size() >= 2)) {
        eval.rank = HandRank::FullHouse;
        eval.kickers.push_back(trips[0]);
        int pairRank = pairs.empty() ? trips[1] : pairs[0];
        eval.kickers.push_back(pairRank);
        return eval;
    }

    if (isFlush) {
        eval.rank = HandRank::Flush;
        for (const auto& card : sortedCards) {
            eval.kickers.push_back(card.GetRank());
        }
        return eval;
    }

    if (isStraight) {
        eval.rank = HandRank::Straight;
        eval.kickers.push_back(straightHigh);
        return eval;
    }

    if (!trips.empty()) {
        eval.rank = HandRank::ThreeOfAKind;
        eval.kickers.push_back(trips[0]);
        for (const int r : singles) {
            eval.kickers.push_back(r);
            if (eval.kickers.size() >= 3)
                break;
        }
        return eval;
    }

    if (pairs.size() >= 2) {
        eval.rank = HandRank::TwoPair;
        eval.kickers.push_back(pairs[0]);
        eval.kickers.push_back(pairs[1]);
        if (!singles.empty()) {
            eval.kickers.push_back(singles[0]);
        }
        return eval;
    }

    if (!pairs.empty()) {
        eval.rank = HandRank::Pair;
        eval.kickers.push_back(pairs[0]);
        for (const int r : singles) {
            eval.kickers.push_back(r);
            if (eval.kickers.size() >= 4)
                break;
        }
        return eval;
    }

    eval.rank = HandRank::HighCard;
    for (const auto& card : sortedCards) {
        eval.kickers.push_back(card.GetRank());
    }
    return eval;
}


// принимает 7 карт (2 личные + 5 общих), перебирает все 5-карточные комбинации ( 21) возвращает лучшую оценку

HandEvaluation evaluateTexasHoldemHand(const std::vector<Cards>& cards7) {
    if (cards7.size() != 7) {
        throw std::runtime_error("Texas Hold'em hand must contain exactly 7 cards.");
    }

    HandEvaluation bestEval;
    bestEval.rank = HandRank::HighCard; // Начинаем с самой слабой комбинации

    // Перебираем все 5-карточные комбинации из 7
    for (int i = 0; i < 7 - 4; ++i) {
        for (int j = i + 1; j < 7 - 3; ++j) {
            for (int k = j + 1; k < 7 - 2; ++k) {
                for (int l = k + 1; l < 7 - 1; ++l) {
                    for (int m = l + 1; m < 7; ++m) {
                        std::vector<Cards> fiveCardHand = {
                            cards7[i], cards7[j], cards7[k], cards7[l], cards7[m]
                        };
                        HandEvaluation currentEval = evaluate5CardHand(fiveCardHand);
                        if (static_cast<int>(currentEval.rank) > static_cast<int>(bestEval.rank)) {
                            bestEval = currentEval;
                        }
                        // При равенстве типов можно дополнительно сравнить кикеры
                    }
                }
            }
        }
    }
    return bestEval;
}*/
