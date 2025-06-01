#pragma once
#include <string>
#include <vector>
#include "Cards.h"
#include <mutex>
class IPlayer
{
public:
	virtual std::vector<Cards> GetFullHand(const std::vector<Cards*>& communityCards)const = 0;
	virtual std::string GetHand() = 0;
	virtual void SetActive(bool a) =0;

	virtual void SetID(int id) = 0;

	virtual bool CanContinue() const = 0;

	virtual bool IsAllIn() const = 0; // Возвращает true, если игрок в состоянии all‑in.
	virtual void SetAllIn(bool allIn) = 0;

	virtual int GetID() const = 0;
	virtual int GetBalance() const = 0;
	virtual std::string GetName() const = 0;
	//virtual std::string GetID() const = 0;

	virtual void AddCard(const Cards& card) = 0;
	virtual void ClearHand() = 0;

	virtual void Bet(int amount) = 0;
	virtual bool CanBet(int amount)const = 0;
	virtual void addToBalance(int amount) = 0;
	virtual void Fold() = 0;

	virtual bool IsActive() const = 0;
	virtual ~IPlayer() = 0;

	virtual const std::vector<Cards>& GetHand() const = 0;
};

inline IPlayer::~IPlayer() {}