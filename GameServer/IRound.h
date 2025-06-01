#pragma once

#include "IBettingSystem.h"
#include <iostream>

class IRound {
public:
    virtual std::string getRoundType() const = 0;
    virtual void startRound(IBettingSystem& bettingSystem) = 0;
    virtual void endRound(IBettingSystem& bettingSystem) = 0;

    //// (Preflop, Flop, Turn, River, Showdown)
    virtual ~IRound() = default;
};

class PreFlopRound : public IRound {
public:
    std::string getRoundType() const override {
        return "Pre-Flop Round";
    }

    void startRound(IBettingSystem& bettingSystem) override {
        std::cout << "[PreFlopRound] Начало раунда Pre-Flop." << std::endl;
    }

    void endRound(IBettingSystem& bettingSystem) override {
        std::cout << "[PreFlopRound] Завершение раунда Pre-Flop." << std::endl;
    }
};
