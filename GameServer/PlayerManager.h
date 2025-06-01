#pragma once

#include "include.h"
#include <set>
#include <map>
#include <memory>
class PlayerManager {
private:
    std::map<int, std::shared_ptr<IPlayer>> players; // Храним игроков по ID
    std::set<int> freeIDs; // Множество свободных (освобожденных) ID
    int maxPlayers = 5;   // Ограничение числа игроков

public:

    bool IsPlayerExist(const std::string& username) {
        for (const auto& pair : players) {
            if (pair.second->GetName() == username) {
                return true;
            }
        }
        return false;
    }

    int getNextPlayerID() {
        if (!freeIDs.empty()) {
            int reusedID = *freeIDs.begin();
            freeIDs.erase(reusedID);
            return reusedID;
        }
        // Если players пустой — вернуть 1, иначе возвращаем наибольший существующий ID+1
        return players.empty() ? 1 : (players.rbegin()->first + 1);
    }

    // Добавление игрока
    bool addPlayer(std::shared_ptr<IPlayer> player) {
        if (players.size() >= maxPlayers) {
            std::cerr << "Ошибка: места за столом заполнены!" << std::endl;
            return false;
        }
        int playerID = player->GetID(); // Предполагаем, что ID уже выставлен
        players[playerID] = player;
        std::cout << "Игрок с ID " << playerID << " добавлен в игру." << std::endl;
        return true;
    }

    // Удаление игрока с освобождением его ID
    void removePlayer(int playerID) {
        auto it = players.find(playerID);
        if (it != players.end()) {
            players.erase(it);
            freeIDs.insert(playerID); // Освобождаем ID для повторного использования
            std::cout << "Игрок с ID " << playerID << " вышел. ID теперь доступен." << std::endl;
        }
        else {
            std::cerr << "Ошибка: попытка удалить несуществующего игрока " << playerID << "\n";
        }
    }

    // Получение игрока по ID
    std::shared_ptr<IPlayer> getPlayer(int playerID) {
        auto it = players.find(playerID);
        return it != players.end() ? it->second : nullptr;
    }

    bool hasFreeSlots() const {
        return players.size() < maxPlayers || !freeIDs.empty();
    }
};
