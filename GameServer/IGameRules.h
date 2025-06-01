#pragma once
#include <vector>
#include "Cards.h"

// останется время попробовать битовые операции для вычисления комбинаций 

enum class HandRank {
    HighCard = 0, // старшая карта
    Pair, // пара
    TwoPair,// две пары
    ThreeOfAKind, // сет (тройка)
    Straight,// стрит (5 карт подряд по значению)
    Flush,// флэш (5 одной масти)
    FullHouse,// фул хаус (сет + пара)
    FourOfAKind,// каре (чатырка одного значения)
    StraightFlush,// стрит флэш (5 подряд по значению и одной масти)
    RoyalFlush // роял флэш (10, J, Q, K, A одной масти имбуля частный случай стрит флжшика)
};

struct HandEvaluation {
    HandRank rank;              // тип комбинации
    std::vector<int> kickers;   // дополнительные значения для разрешения ничьей (например, старшие карты)
};

class IGameRules {
public:
    // Определение победителя. Возвращает список индексов победителей.
    virtual std::vector<int> determineWinners(const std::vector<HandEvaluation>& evaluations) const = 0;
    // Метод для оценки комбинации. На входе набор карт (например, 5 или 7 карт).
    virtual HandEvaluation EvaluateHand(const std::vector<Cards>& cards) const = 0;
    virtual ~IGameRules() = default;
};


