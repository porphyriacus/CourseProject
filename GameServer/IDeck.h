#pragma once

#include "Cards.h"

class IDeck {
public:
    virtual void Reset() = 0; // �������� ������ ������
    virtual void Shuffle() = 0; // �������������
    virtual Cards* Deal() = 0; // ������� �����
    virtual void ReturnCard(const Cards& card) = 0; // ������� �����
    virtual void ReuseUsedCards() = 0;

    virtual bool isEmpty() = 0;
    virtual ~IDeck() = default; // ����������� ����������
};
