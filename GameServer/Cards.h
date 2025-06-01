#pragma once
#include <iostream>
#include <memory>
#include <unordered_map>
enum Suits {
	Hearts, //  Червы
	Diamonds, //  Бубны
	Clubs, // Трефы
	Spades // Пики
};

enum Ranks {
	two = 2,
	three,
	four,
	five,
	six,
	seven,
	eight,
	nine,
	ten,
	Jack,
	Queen,
	King,
	Ace
};


class Cards
{
private:
	Suits suit;
	Ranks rank;

public:
	Suits GetSuit() const {
		return this->suit;
	}
	Ranks GetRank() const {
		return this->rank;
	}
	Cards(Ranks rank, Suits suit) {
		this->rank = rank;
		this->suit = suit;
	}
	// Метод для получения текстового идентификатора карты
	std::string getCardID() const {
		static const std::unordered_map<Ranks, std::string> rankMap = {
			{two, "2"}, {three, "3"}, {four, "4"}, {five, "5"}, {six, "6"},
			{seven, "7"}, {eight, "8"}, {nine, "9"}, {ten, "10"},
			{Jack, "J"}, {Queen, "Q"}, {King, "K"}, {Ace, "A"}
		};

		static const std::unordered_map<Suits, std::string> suitMap = {
			{Hearts, "hearts"}, {Diamonds, "diamonds"},
			{Clubs, "clubs"}, {Spades, "spades"}
		};

		return rankMap.at(rank) + "_" + suitMap.at(suit); // Например: "A_spades"
	}

	friend std::ostream& operator<<(std::ostream& os, const Cards& card) {
		os << card.getCardID(); //  Теперь выводится ID карты
		return os;
	}
};

