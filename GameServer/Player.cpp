#include "Player.h"
#include <iostream>
#include <sstream>

void Player::SetActive(bool a) {
    this -> active = a;
}

std::vector<Cards> Player::GetFullHand(const std::vector<Cards*>& communityCards) const {
    std::vector<Cards> fullHand = hand; //  ��������� ����� ������
    for (const auto& card : communityCards) {
        fullHand.push_back(*card); //  ��������� ����� �����
    }
    return fullHand;
}

std::string Player::GetHand() {
    if (hand.empty()) return "��� ����";

    std::ostringstream oss;
    for (const auto& card : hand) {
        oss << card.getCardID() << " ";
    }

    std::string result = oss.str();
    result.pop_back(); //  ������� ��������� ������
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
  
    return a > amount; // ��� ������� ������ ����� ����� ������
}
void Player::Bet(int amount) {
    //std::lock_guard<std::mutex> lock(mtx);
    if (!CanBet(amount)) {
        //throw std::runtime_error(name + " cannot bet " + std::to_string(amount) + ". Insufficient balance or inactive state.");
    }
    balance -= amount;
    if (balance == 0) {
        
        std::cout << "----------------------������ ���� ����--------------------" << std::endl;
        allIn = true; // ����� ��������� � ��������� "all-in"
    }
    else if (balance < 0) {
        balance = 0;
        allIn = true;
    }
}



void Player::Fold() {
    std::lock_guard<std::mutex> lock(mtx);
    isFolded = true;
    active = false; // ����� �������� ���������� ����������
}

bool Player::IsActive() const {
    std::lock_guard<std::mutex> lock(mtx);
  //return active && !isFolded; // ����� ������� ������ ���� �� �� ������� �����
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