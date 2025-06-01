#pragma once
#include <string>
#include <vector>
#include "Cards.h"
#include "IPlayer.h"


class Player : public IPlayer {
private:
    mutable std::mutex mtx;

    int ID;
    std::string name;
    int balance;
    std::vector<Cards> hand;

    bool active;
    bool allIn = false;
    bool isFolded = false;
public:

    std::vector<Cards> GetFullHand(const std::vector<Cards*>& communityCards)const override;

    Player(const std::string& name, int startingBalance, int id)
        : name(name), balance(startingBalance), ID(id), active(true) {
    }
    Player() : name("Без имени"), balance(1000), ID(1), active(true) {}

    std::string GetHand() override;

    void SetID(int id) override;
    int GetID() const override;

    int GetBalance() const override;
    std::string GetName() const override;

    void AddCard(const Cards& card) override;
    void ClearHand() override;

    bool CanBet(int amount) const override;
    void Bet(int amount) override;
    void addToBalance(int amount) override;

    void Fold() override;
    bool IsActive() const override;
    void SetActive(bool a) override;

    const std::vector<Cards>& GetHand() const override; 

    bool IsAllIn() const override;
    void SetAllIn(bool allInStatus) override;

    

    bool CanContinue() const override;
    ~Player() override = default;
};

