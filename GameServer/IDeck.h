#pragma once

#include "Cards.h"

class IDeck {
public:
    virtual void Reset() = 0; // Создание полной колоды
    virtual void Shuffle() = 0; // Перемешивание
    virtual Cards* Deal() = 0; // Раздача карты
    virtual void ReturnCard(const Cards& card) = 0; // Возврат карты
    virtual void ReuseUsedCards() = 0;

    virtual bool isEmpty() = 0;
    virtual ~IDeck() = default; // Виртуальный деструктор
};
