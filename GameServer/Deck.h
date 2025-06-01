#pragma once
#include "Cards.h"
#include <vector>
#include <algorithm>
#include <random>
#include <iostream>
#include "IDeck.h"

#include <mutex>
//генерация, перемешивание, раздача карт.
// управление калодой
class Deck : public IDeck
{
private:
    mutable std::mutex mtx;

	std::vector <Cards> cards;
    std::vector<Cards> usedCards;   // Использованные карты

public:
    Deck() {
        Reset();
    }

    bool isEmpty() override;
    void Reset() override; // создание колоды заново
 	void Shuffle() override; // перемешивание
	Cards* Deal() override; // раздача верхней карты
    void ReturnCard(const Cards& card) override; // возвращает карту обратно в список использованных
    void ReuseUsedCards() override; // после раунда использованные карты возвращаются в колоду
    bool IsEmpty() const {
        return cards.empty();
    }
};
