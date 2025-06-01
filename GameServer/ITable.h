#pragma once
#include "IPlayer.h"
#include "IDeck.h"
#include <vector>
#include <mutex>
#include <stdexcept>
#include <iostream>
#include <algorithm>

// ����������� �� ������ 
class ITable {

public:
    virtual void rotatePlayers() = 0;
    virtual void dealCommunityCard() = 0; // ������� ����� ����� �� ����
    virtual void resetTable() = 0; // ���������� ��������� �����

    virtual void dealStartingCards(std::shared_ptr<IPlayer> player) = 0; 

    virtual IDeck* getDeck() const = 0;

    virtual std::vector<std::shared_ptr<IPlayer>> getPlayers() const = 0; 
    virtual void addPlayer(std::shared_ptr<IPlayer> player) = 0; 
    virtual void removePlayer(int playerID) = 0;

    virtual const std::vector<Cards*>& getCommunityCards() const = 0;

    virtual void notifyPlayerLeft(std::shared_ptr<IPlayer> player) = 0; 
    virtual void notifyPlayerFolded(std::shared_ptr<IPlayer> player) = 0; 

    virtual std::vector<std::shared_ptr<IPlayer>> getActivePlayers() const = 0; 

    virtual ~ITable() = default;
};


class Table : public ITable {
private:
    mutable std::mutex mtx;

    std::unique_ptr<IDeck> deck;                // ��������� �� ������
    std::vector<Cards*> communityCards;         // ����� ����� �� �����
    std::vector<std::shared_ptr<IPlayer>> players; // ��������� �� �������

public:
    void rotatePlayers() override {
        std::lock_guard<std::mutex> lock(mtx);
        if (players.size() <= 1)
            return;
        // std::rotate �������� �������� ���, ��� ������� �� ������� players.begin() + 1 ���������� ������
        std::rotate(players.begin(), players.begin() + 1, players.end());
    }


    Table(std::unique_ptr<IDeck> deckImpl)
        : deck(std::move(deckImpl))
    {
        deck->Shuffle(); 
    }

    IDeck* getDeck() const override {
        std::lock_guard<std::mutex> lock(mtx);
        return deck.get();
    }

    // ���������� ����� ����� � ������ � ������� ������
    void resetTable() override {
        std::lock_guard<std::mutex> lock(mtx);
        for (Cards* card : communityCards) {
            deck->ReturnCard(*card);
            delete card; 
        }
        communityCards.clear();
        deck->ReuseUsedCards();
    }

    // ������ ��������� (���������) ����� ������ � ��� �����
    void dealStartingCards(std::shared_ptr<IPlayer> player) override {
        std::lock_guard<std::mutex> lock(mtx);

        if (deck->isEmpty()) {
            deck->ReuseUsedCards();
        }
        Cards* card1 = deck->Deal();
        if (!card1) throw std::runtime_error("Failed to deal first starting card.");
        player->AddCard(*card1);

        if (deck->isEmpty()) {
            deck->ReuseUsedCards();
        }
        Cards* card2 = deck->Deal();
        if (!card2) throw std::runtime_error("Failed to deal second starting card.");
        player->AddCard(*card2);
    }

    // ������ ���� ����� ����� �� ����
    void dealCommunityCard() override {
        std::lock_guard<std::mutex> lock(mtx);
        if (deck->isEmpty()) {
            std::cout << "Deck is empty. Reusing and shuffling cards..." << std::endl;
            deck->ReuseUsedCards();
        }
        Cards* card = deck->Deal();
        if (!card) {
            throw std::runtime_error("Failed to deal community card.");
        }
        communityCards.push_back(card);
    }

    std::vector<std::shared_ptr<IPlayer>> getPlayers() const override {
        std::lock_guard<std::mutex> lock(mtx);
        return players;
    }

    void addPlayer(std::shared_ptr<IPlayer> player) override {
        std::lock_guard<std::mutex> lock(mtx);
        players.push_back(player); //  ���������� shared_ptr
    }

    void removePlayer(int playerID) override {
        std::lock_guard<std::mutex> lock(mtx);

        auto it = std::remove_if(players.begin(), players.end(),
            [playerID](const std::shared_ptr<IPlayer>& p) { return p->GetID() == playerID; });

        if (it != players.end()) {
            players.erase(it, players.end()); //  ���������� ��������
            std::cout << "����� � ID " << playerID << " ����� � �������� �����.\n";
        }
        else {
            std::cerr << "������: ����� � ID " << playerID << " �� ������.\n";
        }
    }

    const std::vector<Cards*>& getCommunityCards() const override {
        std::lock_guard<std::mutex> lock(mtx);
        return communityCards;
    }

    std::vector<std::shared_ptr<IPlayer>> getActivePlayers() const override {
        std::lock_guard<std::mutex> lock(mtx);
        std::vector<std::shared_ptr<IPlayer>> activePlayers;
        for (const auto& player : players) {
            if (player->IsActive()) {
                activePlayers.push_back(player);
            }
        }
        return activePlayers;
    }

    void notifyPlayerLeft(std::shared_ptr<IPlayer> player) override {
        std::lock_guard<std::mutex> lock(mtx);
        auto it = std::find_if(players.begin(), players.end(),
            [&](const std::shared_ptr<IPlayer>& p) { return p == player; });

        if (it != players.end()) {
            (*it)->Fold();
            players.erase(it);
        }
        else {
            std::cerr << "������: ����� �� ������ ��� ������.\n";
        }
    }

    void notifyPlayerFolded(std::shared_ptr<IPlayer> player) override {
        std::lock_guard<std::mutex> lock(mtx);
        player->Fold();
    }
};
