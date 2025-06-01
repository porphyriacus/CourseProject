#pragma once

#include "include.h"
#include <set>
#include <map>
#include <memory>
class PlayerManager {
private:
    std::map<int, std::shared_ptr<IPlayer>> players; // ������ ������� �� ID
    std::set<int> freeIDs; // ��������� ��������� (�������������) ID
    int maxPlayers = 5;   // ����������� ����� �������

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
        // ���� players ������ � ������� 1, ����� ���������� ���������� ������������ ID+1
        return players.empty() ? 1 : (players.rbegin()->first + 1);
    }

    // ���������� ������
    bool addPlayer(std::shared_ptr<IPlayer> player) {
        if (players.size() >= maxPlayers) {
            std::cerr << "������: ����� �� ������ ���������!" << std::endl;
            return false;
        }
        int playerID = player->GetID(); // ������������, ��� ID ��� ���������
        players[playerID] = player;
        std::cout << "����� � ID " << playerID << " �������� � ����." << std::endl;
        return true;
    }

    // �������� ������ � ������������� ��� ID
    void removePlayer(int playerID) {
        auto it = players.find(playerID);
        if (it != players.end()) {
            players.erase(it);
            freeIDs.insert(playerID); // ����������� ID ��� ���������� �������������
            std::cout << "����� � ID " << playerID << " �����. ID ������ ��������." << std::endl;
        }
        else {
            std::cerr << "������: ������� ������� ��������������� ������ " << playerID << "\n";
        }
    }

    // ��������� ������ �� ID
    std::shared_ptr<IPlayer> getPlayer(int playerID) {
        auto it = players.find(playerID);
        return it != players.end() ? it->second : nullptr;
    }

    bool hasFreeSlots() const {
        return players.size() < maxPlayers || !freeIDs.empty();
    }
};
