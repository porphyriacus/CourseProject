#include "TexasHoldemRules.h"

//  ��������� ���� ������ ���
int TexasHoldemRules::compareHands(const HandEvaluation& hand1, const HandEvaluation& hand2) const {
    if (static_cast<int>(hand1.rank) > static_cast<int>(hand2.rank))
        return 1;
    else if (static_cast<int>(hand1.rank) < static_cast<int>(hand2.rank))
        return -1;

    // ���������� ������, ���� ����� �����
    size_t minSize = std::min(hand1.kickers.size(), hand2.kickers.size());

    for (size_t i = 0; i < minSize; ++i) {
        if (hand1.kickers[i] > hand2.kickers[i])
            return 1;
        else if (hand1.kickers[i] < hand2.kickers[i])
            return -1;
    }
    return 0; // ���� �����
}


// ������� ��� ����������� ���������� ����� ���������� ����������
// ������� ���������� ������ �������� ���������� ������� (���� ��������� - � ������ ������)
std::vector<int> TexasHoldemRules::determineWinners(const std::vector<HandEvaluation>& evaluations) const {
    std::vector<int> winners;
    if (evaluations.empty()) {
        return winners;
    }

    // �������������� �������� ������ �������
    HandEvaluation best = evaluations[0];
    winners.push_back(0);

    // ������ �������� �� ��������� ������� � ����������
    for (size_t i = 1; i < evaluations.size(); ++i) {
        int comp = compareHands(evaluations[i], best);
        if (comp > 0) {
            // ����� ���������� �������, ��������� best � ���������� ������ �����������
            best = evaluations[i];
            winners.clear();
            winners.push_back(i);
        }
        else if (comp == 0) {
            // ���� ���������� ����� best, ��������� ������ � ������ �����������
            winners.push_back(i);
        }
        // ���� comp < 0, ������ �� ������, ������� ������ ������ best
    }
    return winners;
}

HandEvaluation TexasHoldemRules::EvaluateHand(const std::vector<Cards>& cards) const {
    if (cards.size() != 7) {
        throw std::runtime_error("����� �������� ������ 7 ����!");
    }

    // ���������� ���� 7 ���� �� �������� ����� (��� �������� ������ �������)
    std::vector<Cards> sortedCards = cards;
    std::sort(sortedCards.begin(), sortedCards.end(),
        [](const Cards& a, const Cards& b) {
            return a.GetRank() > b.GetRank();
        });

    // ������� ��� ������ (������� 2..14) � ������ (0..3)
    std::array<int, 15> rankFrequency{};
    std::array<int, 4> suitFrequency{};

    for (const auto& card : cards) {
        ++rankFrequency[card.GetRank()];
        ++suitFrequency[card.GetSuit()];
    }

    HandEvaluation eval;

    //////////////////////////////////////////////////////////
    // 1. �������� Flush � Straight Flush (������� ����)
    /////////////////////////////////////////////////////////
    int flushSuit = -1;
    for (int s = 0; s < 4; ++s) {
        if (suitFrequency[s] >= 5) {
            flushSuit = s;
            break;
        }
    }
    if (flushSuit != -1) {
        // c������� ��� ����� �������� ����� � ��������� ���������
        std::vector<int> flushCards;
        for (const auto& card : cards) {
            if (card.GetSuit() == flushSuit) {
                flushCards.push_back(card.GetRank());
            }
        }
        // c�������� �� �������� � ������� ��������� (����� ��������� ������ ������������������)
        std::sort(flushCards.begin(), flushCards.end(), std::greater<int>());
        flushCards.erase(std::unique(flushCards.begin(), flushCards.end()), flushCards.end());

        // ����� Straight Flush: ��������� ������������������ ������ flushCards
        int straightFlushHigh = 0;
        for (size_t i = 0; i + 4 < flushCards.size(); ++i) {
            bool isSeq = true;
            for (int j = 0; j < 4; ++j) {
                if (flushCards[i + j] - flushCards[i + j + 1] != 1) {
                    isSeq = false;
                    break;
                }
            }
            if (isSeq) {
                straightFlushHigh = flushCards[i];
                break;
            }
        }
        // �������� �� "������": A-2-3-4-5 ��� ������ ������ Straight Flush
        if (!straightFlushHigh) {
            std::vector<int> wheel = { 14, 5, 4, 3, 2 };
            bool isWheel = true;
            for (int r : wheel) {
                if (std::find(flushCards.begin(), flushCards.end(), r) == flushCards.end()) {
                    isWheel = false;
                    break;
                }
            }
            if (isWheel) {
                straightFlushHigh = 5; // ������� ����� ����� 5
            }
        }
        if (straightFlushHigh) {
            // ���� ���������� ���������� � ���� � ������������� �������� � ���� ���� �� ������� �����
            if (straightFlushHigh == 14 && std::find(flushCards.begin(), flushCards.end(), 10) != flushCards.end()) {
                eval.rank = HandRank::RoyalFlush;
            }
            else {
                eval.rank = HandRank::StraightFlush;
            }
            eval.kickers.push_back(straightFlushHigh);
            return eval;
        }
        // Straight Flush �� ������, ������� ����:
        eval.rank = HandRank::Flush;
        // ����� 5 ������ ���� �����
        for (size_t i = 0; i < flushCards.size() && i < 5; ++i) {
            eval.kickers.push_back(flushCards[i]);
        }
        return eval;
    }

    //////////////////////////////////////////
    // 2. �������� Straight (��� ����� �����)
    ///////////////////////////////////////////
    int straightHigh = 0;
    for (int r = 14; r >= 5; --r) {
        bool seq = true;
        for (int j = 0; j < 5; ++j) {
            if (rankFrequency[r - j] == 0) {
                seq = false;
                break;
            }
        }
        if (seq) {
            straightHigh = r;
            break;
        }
    }
    // ������ "������": A-2-3-4-5
    if (!straightHigh) {
        if (rankFrequency[14] && rankFrequency[2] && rankFrequency[3] && rankFrequency[4] && rankFrequency[5])
            straightHigh = 5;
    }
    if (straightHigh) {
        eval.rank = HandRank::Straight;
        eval.kickers.push_back(straightHigh);
        return eval;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // 3 ���������� �����: ����, ����-����, ������, ��� ����, ����
    ///////////////////////////////////////////////////////////////////////////
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

    // ����
    if (!quads.empty()) {
        eval.rank = HandRank::FourOfAKind;
        eval.kickers.push_back(quads[0]); // ���� ������������ �� ������ �������� �����
        // ��������� ������ ����� �� ���������� ���� (��������������� - ������� ����� �� singles)
        if (!singles.empty())
            eval.kickers.push_back(singles[0]);
        else if (!pairs.empty())
            eval.kickers.push_back(pairs[0]);
        else if (trips.size() > 1)
            eval.kickers.push_back(trips[1]);
        return eval;
    }

    // ����-����: ����� ������� 1 ������ � ����, ���� ��� ������ (������ �������� ��� ����)
    if (!trips.empty() && (!pairs.empty() || trips.size() >= 2)) {
        eval.rank = HandRank::FullHouse;
        eval.kickers.push_back(trips[0]); // ������
        // ���� ���� ����, ���������� �, ����� ������ ������ ��������� ���� ����
        if (!pairs.empty())
            eval.kickers.push_back(pairs[0]);
        else
            eval.kickers.push_back(trips[1]);
        return eval;
    }

    // ������ (���� Full House �� �����������)
    if (!trips.empty()) {
        eval.rank = HandRank::ThreeOfAKind;
        eval.kickers.push_back(trips[0]);
        // ��������� ��� ������ ������ �� ��������� ���� (singles)
        int added = 0;
        for (int r : singles) {
            if (added < 2) {
                eval.kickers.push_back(r);
                added++;
            }
            else {
                break;
            }
        }
        return eval;
    }

    // ��� ����
    if (pairs.size() >= 2) {
        eval.rank = HandRank::TwoPair;
        // ����� ��� ������� ����
        eval.kickers.push_back(pairs[0]);
        eval.kickers.push_back(pairs[1]);
        // ��������� �����: ������� �����, �� �������� � ����
        if (!singles.empty()) {
            eval.kickers.push_back(singles[0]);
        }
        return eval;
    }

    // ���� ����
    if (!pairs.empty()) {
        eval.rank = HandRank::Pair;
        eval.kickers.push_back(pairs[0]);
        // ��������� ��� ������ ������ �� ���������� ����
        int added = 0;
        for (int r : singles) {
            if (added < 3) {
                eval.kickers.push_back(r);
                added++;
            }
            else {
                break;
            }
        }
        return eval;
    }

    /////////////////////////////////////////////////////////////////////////////////
    // 4. High Card: ���� �� ���� ���������� �� ������������ ����� 5 ������� ����
    ///////////////////////////////////////////////////////////////////////////////
    eval.rank = HandRank::HighCard;
    int count = 0;
    for (int r : singles) {
        if (count < 5) {
            eval.kickers.push_back(r);
            count++;
        }
        else {
            break;
        }
    }
    return eval;
}