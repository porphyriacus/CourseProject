#include "PreflopRound.h"
#include "ITable.h"
#include "IPlayer.h"

//void PreflopRound::startRound(IBettingSystem& bettingSystem, IGameView& view, IGameInput& input) {
//    view.displayMessage("Ётап Preflop началс€.");
//    // ѕолучаем активных игроков дл€ раздачи стартовых карт
//    ITable* table = bettingSystem.getTable();  // ѕредполагаетс€, что BettingSystem имеет метод getTable()
//    auto players = table->getActivePlayers();
//
//    // –аздаем каждому игроку по 2 стартовые карты
//    for (IPlayer* player : players) {
//        table->dealStartingCards(*player);
//    }
//    view.displayMessage("—тартовые карты розданы.");
//
//    // –аунд ставок Ц дл€ каждого активного игрока
//    view.displayMessage("–аунд ставок (Preflop) начинаетс€.");
//    for (IPlayer* player : players) {
//        // «десь можно более тонко настроить логику ставок:
//        std::string action = input.promptAction(*player, /*currentBet=*/0);
//        if (action == "Fold") {
//            bettingSystem.fold(player);
//        }
//        else if (action == "Call") {
//            bettingSystem.call(player);
//        }
//        else if (action == "Raise") {
//            int bet = input.promptBet(*player, 10); // допустим, минимальна€ ставка 10
//            bettingSystem.raise(player, bet);
//        }
//    }
//}
//
//void PreflopRound::endRound(IBettingSystem& bettingSystem, IGameView& view) {
//    view.displayMessage("–аунд Preflop завершен.");
//}
//
//std::string PreflopRound::getRoundType() const {
//    return "Preflop";
//}
//
