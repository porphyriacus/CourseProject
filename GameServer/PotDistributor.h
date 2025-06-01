#pragma once
#include "IPlayer.h"
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <stdexcept>



class PotDistributor {
public:
    //  Функция распределения банка
    // sidePots: каждая пара состоит из суммы банка и списка игроков (shared_ptr), которые внесли вклады в него
    // winners: список победителей (shared_ptr)
    // возвращает: map от shared_ptr<IPlayer> к выигрышным суммам
    static std::unordered_map<std::shared_ptr<IPlayer>, int> distribute(
        const std::vector<std::pair<int, std::vector<std::shared_ptr<IPlayer>>>>& sidePots,
        const std::vector<std::shared_ptr<IPlayer>>& winners)
    {
        std::unordered_map<std::shared_ptr<IPlayer>, int> winnings;

        //  Для каждого банка определяем, какие игроки из списка eligible также оказались победителями
        for (const auto& potPair : sidePots) {
            int potAmount = potPair.first;
            const auto& eligiblePlayers = potPair.second;

            //  Определяем пересечение: игроки, участвовавшие в данном банке и имеющие право на выигрыш
            std::vector<std::shared_ptr<IPlayer>> potWinners;
            for (const auto& plyr : winners) {
                if (std::find(eligiblePlayers.begin(), eligiblePlayers.end(), plyr) != eligiblePlayers.end()) {
                    potWinners.push_back(plyr);
                }
            }

            //  Если никому не достался данный банк — пропускаем его
            if (potWinners.empty()) {
                continue;
            }

            // Делим банк поровну между победителями
            int share = potAmount / static_cast<int>(potWinners.size());
            for (const auto& plyr : potWinners) {
                winnings[plyr] += share;
            }
        }
        return winnings;
    }
};
