#include "Player.h"
#include <iostream>
#include <sstream>

void Player::SetActive(bool a) {
    this -> active = a;
}

std::vector<Cards> Player::GetFullHand(const std::vector<Cards*>& communityCards) const {
    std::vector<Cards> fullHand = hand; //  добавляем карты игрока
    for (const auto& card : communityCards) {
        fullHand.push_back(*card); //  добавляем общие карты
    }
    return fullHand;
}

std::string Player::GetHand() {
    if (hand.empty()) return "Нет карт";

    std::ostringstream oss;
    for (const auto& card : hand) {
        oss << card.getCardID() << " ";
    }

    std::string result = oss.str();
    result.pop_back(); //  Убираем последний пробел
    return result;
}

int Player::GetID() const {
    std::lock_guard<std::mutex> lock(mtx);
    return this->ID;
}
void Player::SetID(int id){
    std::lock_guard<std::mutex> lock(mtx);
    this->ID = id;
}

int Player::GetBalance() const {
    std::lock_guard<std::mutex> lock(mtx);
    return this->balance;
}
void Player::addToBalance(int amount) {
    std::lock_guard<std::mutex> lock(mtx);
    balance += amount;
}


std::string Player::GetName() const {
    std::lock_guard<std::mutex> lock(mtx);
    return this->name;
}

void Player::AddCard(const Cards& card) {
    std::lock_guard<std::mutex> lock(mtx);
    this->hand.push_back(card);
}

void Player::ClearHand() {
    std::lock_guard<std::mutex> lock(mtx);
    this->hand.clear();
    this->active = true; 
    this->isFolded = false;
    
}


bool Player::CanBet(int amount) const {
    std::lock_guard<std::mutex> lock(mtx);
    int a = balance;
  
    return a > amount; // это текущая ставка минус вклад игрока
}
void Player::Bet(int amount) {
    //std::lock_guard<std::mutex> lock(mtx);
    if (!CanBet(amount)) {
        //throw std::runtime_error(name + " cannot bet " + std::to_string(amount) + ". Insufficient balance or inactive state.");
    }
    balance -= amount;
    if (balance == 0) {
        
        std::cout << "----------------------БАЛАНС НУЛЬ ДОЛБ--------------------" << std::endl;
        allIn = true; // игрок переходит в состояние "all-in"
    }
    else if (balance < 0) {
        balance = 0;
        allIn = true;
    }
}



void Player::Fold() {
    std::lock_guard<std::mutex> lock(mtx);
    isFolded = true;
    active = false; // игрок временно становится неактивным
}

bool Player::IsActive() const {
    std::lock_guard<std::mutex> lock(mtx);
  //return active && !isFolded; // Игрок активен только если он не сбросил карты
    return active && !isFolded;
}

const std::vector<Cards>& Player::GetHand() const {
    std::lock_guard<std::mutex> lock(mtx);
    return this->hand;
}

bool Player::IsAllIn() const  {
    std::lock_guard<std::mutex> lock(mtx);
    return allIn;
}

void Player::SetAllIn(bool allInStatus)  {
    std::lock_guard<std::mutex> lock(mtx);
    allIn = allInStatus;
}

bool Player::CanContinue() const {
    std::lock_guard<std::mutex> lock(mtx);
    return balance > 0 && active && !isFolded;
}