#pragma once
#include "Cards.h"
#include <vector>
#include <algorithm>
#include <random>
#include <iostream>
#include "IDeck.h"

#include <mutex>
//���������, �������������, ������� ����.
// ���������� �������
class Deck : public IDeck
{
private:
    mutable std::mutex mtx;

	std::vector <Cards> cards;
    std::vector<Cards> usedCards;   // �������������� �����

public:
    Deck() {
        Reset();
    }

    bool isEmpty() override;
    void Reset() override; // �������� ������ ������
 	void Shuffle() override; // �������������
	Cards* Deal() override; // ������� ������� �����
    void ReturnCard(const Cards& card) override; // ���������� ����� ������� � ������ ��������������
    void ReuseUsedCards() override; // ����� ������ �������������� ����� ������������ � ������
    bool IsEmpty() const {
        return cards.empty();
    }
};
