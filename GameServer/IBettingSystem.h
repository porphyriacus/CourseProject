#pragma once
#include "IPlayer.h"  

#include "ITable.h"  
#include "Bank.h"
#include <stdexcept>
#include <mutex>
#include <algorithm>
#include <limits>
class IBettingSystem {
public:
    virtual ITable* getTable() = 0;

    // p�������� ������ ������ �� ������� ������
    virtual void placeBet(std::shared_ptr<IPlayer> player, int amount) = 0;

    // ����� ����������� � ������� �������
    virtual void call(std::shared_ptr<IPlayer> player) = 0;

    // ����� �������� ������ �� ��������� ��������
    virtual void raise(std::shared_ptr<IPlayer> player, int amount) = 0;

    // ����� ������������
    virtual void fold(std::shared_ptr<IPlayer> player) = 0;

    // ���������� ������� ���� ������
    virtual int getPot() const = 0;

    // ���������� ���� (������ ���������� � ������ ������ ������)
    virtual void resetPot() = 0;

    virtual std::vector<std::pair<int, std::vector<std::shared_ptr<IPlayer>>>> getSidePots() const = 0;

    virtual ~IBettingSystem() = default;
};


class BettingSystem : public IBettingSystem {
private:
    ITable* table; // ���� ��� ������� � �������
    Bank bank;     // ���������� �������� �������
    int currentBet; // ������� ������������ ������
    int minBet;     // ����������� ������ (����� ���� ������������� ��� ��������������)
    mutable std::mutex mtx; // ������� ��� ������ ������


    // ���������� ����������� ������ ����� �������� �������
    int getMinActivePlayerBalance() const {
        int minBalance = std::numeric_limits<int>::max();
        auto players = table->getActivePlayers();
        for (const auto& player : players) {
            minBalance = std::min(minBalance, player->GetBalance());
        }
        return (minBalance == std::numeric_limits<int>::max() ? 0 : minBalance);
    }

public:

    BettingSystem(ITable* tablePtr, int fixedMinBet)
        : table(tablePtr), currentBet(0), minBet(fixedMinBet) {
    }

     ITable* getTable()  override {
        std::lock_guard<std::mutex> lock(mtx);
        return table;
    }

    int getMinBet() const {
        std::lock_guard<std::mutex> lock(mtx);
        int a = getMinActivePlayerBalance();
        return a ;
        
    }

    int getSmallBlind() const {
        return minBet;
    }

    int getBigBlind() const {
        return getSmallBlind() * 2;
    }


    int getCurrentBet() const {
        std::lock_guard<std::mutex> lock(mtx);
        return currentBet;
    }

    void placeBet(std::shared_ptr<IPlayer> player, int amount) override {
        if (!player) {
            throw std::invalid_argument("placeBet: null player pointer");
        }

        int effectiveMinBet = getMinBet();
        if (amount > effectiveMinBet) {
            amount = effectiveMinBet;
        }
      
        if (player) {
            player->Bet(amount);
        }
      
        bank.addContribution(player, amount);// ��������� ����� ������ 
        {
           // std::lock_guard<std::mutex> lock(mtx);
            if (amount > currentBet) {
                std::cout << "current bet now is  " << amount << std::endl;
                currentBet = amount; // ��������� ������� ������
            }
        }
    }

    // ����� ����������� � ������� ������� (Call)
    void call(std::shared_ptr<IPlayer> player) override {
        if (!player) {
            throw std::invalid_argument("call: null player pointer");
        }
        // �������� ��� �������� ����� ������
        int currentContribution = bank.getContribution(player);

        // ���������, ������� ���������� ��������, ����� ������� ����� �� currentBet.
        // ��� � ���� �������������� ����� ��� �������� CALL.
        int additionalAmount = 0;
        {
            std::lock_guard<std::mutex> lock(mtx);
            additionalAmount = currentBet - currentContribution;
        }
        additionalAmount = currentBet - currentContribution;
        // ���� ����� ��� �������� ������ (��������, � ���������� ����������� �����), �������������� ����� ����� 0.
        if (additionalAmount < 0) {
            additionalAmount = 0;
        }

        // ��������� ������ ������ � ��������� ����
        player->Bet(additionalAmount);
        bank.addContribution(player, additionalAmount);
       
    }

    // ����� ����������� ������ (Raise)
    void raise(std::shared_ptr<IPlayer> player, int amount) override {
        if (!player) {
            throw std::invalid_argument("raise: null player pointer");
        }

        //int newBet;
        //{
        //    std::lock_guard<std::mutex> lock(mtx);
        //    newBet = currentBet + amount;
        //}

        //int effectiveMinBet = getMinBet();
        //if (newBet > effectiveMinBet) {
        //    newBet = effectiveMinBet; // ������������ ����� ������
        //}
        int currentContribution = bank.getContribution(player);

        player->Bet(amount);
        bank.addContribution(player, amount);

        {
           // std::lock_guard<std::mutex> lock(mtx);
            currentBet = amount + currentContribution; // ��������� ������� ������
        }
    }

    // ����� ���������� ����� (Fold)
    void fold(std::shared_ptr<IPlayer> player) override {
        if (!player) {
            throw std::invalid_argument("fold: null player pointer");
        }
        player->Fold();
    }

    // ���������� ����� ���� (Pot)
    int getPot() const override {
        return bank.getTotal();
    }

    // ���������� ���� � ������� ������
    void resetPot() override {
        bank.reset();
        std::lock_guard<std::mutex> lock(mtx);
        currentBet = 0;
    }

    std::vector<std::pair<int, std::vector<std::shared_ptr<IPlayer>>>> getSidePots() const override {
        return bank.computeSidePots();
    }

    virtual ~BettingSystem() override = default;
};
