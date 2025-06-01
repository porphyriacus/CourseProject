#pragma once

#include <memory>
#include <vector>
#include "ITable.h"
#include "IGameRules.h"
#include "IBettingSystem.h"
#include "IGameView.h"
#include "IGameInput.h"
#include "IRound.h"

class IGame {
public:
    virtual void startGame() = 0;                      // Запускает игровую сессию
    virtual void stopGame() = 0;                       // Останавливает игру

    virtual void addPlayer(const IPlayer& player) = 0;   // Добавляет нового игрока
    virtual void removePlayer(int playerIndex) = 0;      // Удаляет игрока по индексу

    virtual void processTurn() = 0;                      // Выполняет ход, переходя к следующему этапу
    virtual bool isGameContinuing() const = 0;           // Проверяет, можно ли продолжать игру

    virtual const IRound* getCurrentRound() const = 0;   // Возвращает текущий этап игры
    virtual void advanceToNextRound() = 0;              // Переход к следующему этапу

    virtual ~IGame() = default;
};
