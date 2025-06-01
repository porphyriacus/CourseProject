#pragma once
#include "IPlayer.h"
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <utility>
#include <mutex>
#include <stdexcept>

class Bank {
private:
    std::unordered_map<std::shared_ptr<IPlayer>, int> contributions;
    mutable std::mutex mtx; // ��� ������ ������� � contributions

public:
    Bank() = default;
    int getContribution(const std::shared_ptr<IPlayer>& player) const {
        std::lock_guard<std::mutex> lock(mtx);
        auto it = contributions.find(player);
        return (it != contributions.end()) ? it->second : 0;
    }
    // ��������� ����� ������ 
    void addContribution(std::shared_ptr<IPlayer> player, int amount) {
        if (player == nullptr) {
            throw std::invalid_argument("addContribution: null player pointer");
        }
        std::lock_guard<std::mutex> lock(mtx);
        contributions[player] += amount;
    }

    // ���������� ����� ���� (����� ���� �������).
    int getTotal() const {
        std::lock_guard<std::mutex> lock(mtx);
        int total = 0;
        for (const auto& pair : contributions) {
            total += pair.second;
        }
        return total;
    }


    // ��������� �������� ���� � ����-����
    // ���������� ������ ���, ��� ������ ���� {potSum, eligiblePlayers}
    // potSum � ����� ����������� �����,
    // eligiblePlayers � ������ �������, ������� ����� ������������ �� ���� ����
    std::vector<std::pair<int, std::vector<std::shared_ptr<IPlayer>>>> computeSidePots() const {
        std::lock_guard<std::mutex> lock(mtx);
        // ��������� ������ � ������ ���
        std::vector<std::pair<std::shared_ptr<IPlayer>, int>> contribList(contributions.begin(), contributions.end());

        // ����������� ������� � ������� �������
        contribList.erase(
            std::remove_if(contribList.begin(), contribList.end(),
                [](const std::pair<std::shared_ptr<IPlayer>, int>& p) { return p.second <= 0; }),
            contribList.end()
        );

        //  ��������� �� ����������� ����� ������.
        std::sort(contribList.begin(), contribList.end(),
            [](const std::pair<std::shared_ptr<IPlayer>, int>& a, const std::pair<std::shared_ptr<IPlayer>, int>& b) {
                return a.second < b.second;
            }
        );

        std::vector<std::pair<int, std::vector<std::shared_ptr<IPlayer>>>> pots;

        // g��� ���� ���������, �������� �������� �����.
        while (!contribList.empty()) {
            int minContribution = contribList.front().second;
            int count = static_cast<int>(contribList.size());
            int potSegment = minContribution * count;

            std::vector<std::shared_ptr<IPlayer>> eligible;
            for (const auto& p : contribList) {
                eligible.push_back(p.first);
            }
            pots.push_back({ potSegment, eligible });

            //  �������� ����������� ����� � �������.
            for (auto& p : contribList) {
                p.second -= minContribution;
            }
            // ������� �������, ��� ����� ���������.
            while (!contribList.empty() && contribList.front().second == 0) {
                contribList.erase(contribList.begin());
            }
        }
        return pots;
    }

    // ���������� ��� ������ 
    void reset() {
        std::lock_guard<std::mutex> lock(mtx);
        contributions.clear();
    }
};
