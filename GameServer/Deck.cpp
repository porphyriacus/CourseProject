#include "Deck.h"


void Deck::Shuffle() {
    std::random_device rd;           // ���������� ���� �����������������
    std::mt19937 random(rd());       // ��������� ��������� ����� ����������� �������� ��������-��������

    std::shuffle(cards.begin(), cards.end(), random); // ������������� �������������� � �������������� ��������� ������-�����
}


void Deck::Reset() {
    std::lock_guard<std::mutex> lock(mtx);
    cards.clear();
    usedCards.clear();

    for (int s = Hearts; s <= Spades; ++s) {
        for (int r = two; r <= Ace; ++r) {
            cards.push_back(Cards(static_cast<Ranks>(r), static_cast<Suits>(s)));
        }
    }
}

Cards* Deck::Deal() {
    std::lock_guard<std::mutex> lock(mtx);
    if (!cards.empty()) {
        // ������� ������ �� ����, ������� ��������� ����� �� �������
        Cards* topCard = new Cards(cards.back());
        cards.pop_back();
        usedCards.push_back(*topCard);  // ����� ������� ����� � usedCards, ���� ��� ��������� �������
        return topCard;  // ���������� ����� ��������� �� ����������� ���������� �����
    }
    else {
        throw std::runtime_error("Deck is empty. Cannot deal more cards.");
    }
}


void Deck::ReturnCard(const Cards& card) {
    std::lock_guard<std::mutex> lock(mtx);
    usedCards.push_back(card);
}

void Deck::ReuseUsedCards() {
    std::lock_guard<std::mutex> lock(mtx);
    for (const auto& card : usedCards) {
        cards.push_back(card);
    }
    usedCards.clear();
    Shuffle();
}

bool Deck::isEmpty() {
    std::lock_guard<std::mutex> lock(mtx);
    return cards.empty();
}