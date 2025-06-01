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
    mutable std::mutex mtx; // для защиты доступа к contributions

public:
    Bank() = default;
    int getContribution(const std::shared_ptr<IPlayer>& player) const {
        std::lock_guard<std::mutex> lock(mtx);
        auto it = contributions.find(player);
        return (it != contributions.end()) ? it->second : 0;
    }
    // добавляет вклад игрока 
    void addContribution(std::shared_ptr<IPlayer> player, int amount) {
        if (player == nullptr) {
            throw std::invalid_argument("addContribution: null player pointer");
        }
        std::lock_guard<std::mutex> lock(mtx);
        contributions[player] += amount;
    }

    // возвращает общий банк (сумму всех вкладов).
    int getTotal() const {
        std::lock_guard<std::mutex> lock(mtx);
        int total = 0;
        for (const auto& pair : contributions) {
            total += pair.second;
        }
        return total;
    }


    // Вычисляет основной банк и сайд-поты
    // Возвращает вектор пар, где каждая пара {potSum, eligiblePlayers}
    // potSum – сумма конкретного банка,
    // eligiblePlayers – список игроков, имеющих право претендовать на этот банк
    std::vector<std::pair<int, std::vector<std::shared_ptr<IPlayer>>>> computeSidePots() const {
        std::lock_guard<std::mutex> lock(mtx);
        // скопируем вклады в вектор пар
        std::vector<std::pair<std::shared_ptr<IPlayer>, int>> contribList(contributions.begin(), contributions.end());

        // отфильтруем игроков с нулевым вкладом
        contribList.erase(
            std::remove_if(contribList.begin(), contribList.end(),
                [](const std::pair<std::shared_ptr<IPlayer>, int>& p) { return p.second <= 0; }),
            contribList.end()
        );

        //  Сортируем по возрастанию суммы вклада.
        std::sort(contribList.begin(), contribList.end(),
            [](const std::pair<std::shared_ptr<IPlayer>, int>& a, const std::pair<std::shared_ptr<IPlayer>, int>& b) {
                return a.second < b.second;
            }
        );

        std::vector<std::pair<int, std::vector<std::shared_ptr<IPlayer>>>> pots;

        // gока есть вкладчики, выделяем сегменты банка.
        while (!contribList.empty()) {
            int minContribution = contribList.front().second;
            int count = static_cast<int>(contribList.size());
            int potSegment = minContribution * count;

            std::vector<std::shared_ptr<IPlayer>> eligible;
            for (const auto& p : contribList) {
                eligible.push_back(p.first);
            }
            pots.push_back({ potSegment, eligible });

            //  Вычитаем минимальный вклад у каждого.
            for (auto& p : contribList) {
                p.second -= minContribution;
            }
            // Удаляем игроков, чей вклад обнулился.
            while (!contribList.empty() && contribList.front().second == 0) {
                contribList.erase(contribList.begin());
            }
        }
        return pots;
    }

    // сбрасывает все вклады 
    void reset() {
        std::lock_guard<std::mutex> lock(mtx);
        contributions.clear();
    }
};
