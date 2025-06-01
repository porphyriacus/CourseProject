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

    // pазмещает ставку игрока на текущем раунде
    virtual void placeBet(std::shared_ptr<IPlayer> player, int amount) = 0;

    // игрок соглашается с текущей ставкой
    virtual void call(std::shared_ptr<IPlayer> player) = 0;

    // игрок повышает ставку на указанное значение
    virtual void raise(std::shared_ptr<IPlayer> player, int amount) = 0;

    // игрок сбрасывается
    virtual void fold(std::shared_ptr<IPlayer> player) = 0;

    // возвращает текущий банк ставки
    virtual int getPot() const = 0;

    // сбрасывает банк (обычно вызывается в начале нового раунда)
    virtual void resetPot() = 0;

    virtual std::vector<std::pair<int, std::vector<std::shared_ptr<IPlayer>>>> getSidePots() const = 0;

    virtual ~IBettingSystem() = default;
};


class BettingSystem : public IBettingSystem {
private:
    ITable* table; // Поле для доступа к игрокам
    Bank bank;     // Управление вкладами игроков
    int currentBet; // Текущая обязательная ставка
    int minBet;     // Минимальная ставка (может быть фиксированной или рассчитываемой)
    mutable std::mutex mtx; // Мьютекс для защиты данных


    // Возвращает минимальный баланс среди активных игроков
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
      
        bank.addContribution(player, amount);// добавляет вклад игрока 
        {
           // std::lock_guard<std::mutex> lock(mtx);
            if (amount > currentBet) {
                std::cout << "current bet now is  " << amount << std::endl;
                currentBet = amount; // Обновляем текущую ставку
            }
        }
    }

    // Игрок соглашается с текущей ставкой (Call)
    void call(std::shared_ptr<IPlayer> player) override {
        if (!player) {
            throw std::invalid_argument("call: null player pointer");
        }
        // Получаем уже внесённую сумму игрока
        int currentContribution = bank.getContribution(player);

        // Вычисляем, сколько необходимо добавить, чтобы довести вклад до currentBet.
        // Это и есть дополнительная сумма для действия CALL.
        int additionalAmount = 0;
        {
            std::lock_guard<std::mutex> lock(mtx);
            additionalAmount = currentBet - currentContribution;
        }
        additionalAmount = currentBet - currentContribution;
        // Если игрок уже поставил больше (например, в результате предыдущего рейза), дополнительная сумма равна 0.
        if (additionalAmount < 0) {
            additionalAmount = 0;
        }

        // Выполняем ставку игрока и обновляем банк
        player->Bet(additionalAmount);
        bank.addContribution(player, additionalAmount);
       
    }

    // Игрок увеличивает ставку (Raise)
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
        //    newBet = effectiveMinBet; // Ограничиваем новую ставку
        //}
        int currentContribution = bank.getContribution(player);

        player->Bet(amount);
        bank.addContribution(player, amount);

        {
           // std::lock_guard<std::mutex> lock(mtx);
            currentBet = amount + currentContribution; // Обновляем текущую ставку
        }
    }

    // Игрок сбрасывает карты (Fold)
    void fold(std::shared_ptr<IPlayer> player) override {
        if (!player) {
            throw std::invalid_argument("fold: null player pointer");
        }
        player->Fold();
    }

    // Возвращает общий банк (Pot)
    int getPot() const override {
        return bank.getTotal();
    }

    // Сбрасывает банк и текущую ставку
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
