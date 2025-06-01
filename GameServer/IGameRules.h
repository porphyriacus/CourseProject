#pragma once
#include <vector>
#include "Cards.h"

// ��������� ����� ����������� ������� �������� ��� ���������� ���������� 

enum class HandRank {
    HighCard = 0, // ������� �����
    Pair, // ����
    TwoPair,// ��� ����
    ThreeOfAKind, // ��� (������)
    Straight,// ����� (5 ���� ������ �� ��������)
    Flush,// ���� (5 ����� �����)
    FullHouse,// ��� ���� (��� + ����)
    FourOfAKind,// ���� (������� ������ ��������)
    StraightFlush,// ����� ���� (5 ������ �� �������� � ����� �����)
    RoyalFlush // ���� ���� (10, J, Q, K, A ����� ����� ������ ������� ������ ����� �������)
};

struct HandEvaluation {
    HandRank rank;              // ��� ����������
    std::vector<int> kickers;   // �������������� �������� ��� ���������� ������ (��������, ������� �����)
};

class IGameRules {
public:
    // ����������� ����������. ���������� ������ �������� �����������.
    virtual std::vector<int> determineWinners(const std::vector<HandEvaluation>& evaluations) const = 0;
    // ����� ��� ������ ����������. �� ����� ����� ���� (��������, 5 ��� 7 ����).
    virtual HandEvaluation EvaluateHand(const std::vector<Cards>& cards) const = 0;
    virtual ~IGameRules() = default;
};


