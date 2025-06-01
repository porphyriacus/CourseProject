#pragma once
#include <boost/asio.hpp>
#include <iostream>
#include <memory>
#include <set>
#include <map>

#include "PlayerManager.h"
#include "RegistrationHandler.h"
#include "EntryHandler.h"

class IGameView {
public:
    virtual void displayMessage(const std::string& message) = 0; // Вывод сообщения клиентам
    virtual ~IGameView() = default;
};
#include "ConnectionManager.h"
#include "ServerGame.h"             // Наш класс игры, реализующий IGame (переписанный без консоли)
//#include "ConnectionManager.h"

using boost::asio::ip::tcp;

class Server {
public:
    Server(int port);
    void start();


    void sendMessageToPlayer(int playerID, const std::string& message);
    void broadcastGameState();

private:
    void registerPlayerConnection(int playerID, std::shared_ptr<tcp::socket> socket);


    void acceptConnection();
    void startRead(std::shared_ptr<tcp::socket> socket);
    void handleMessage(std::shared_ptr<tcp::socket> socket, const std::string& message);
    void sendMessage(std::shared_ptr<tcp::socket> socket, const std::string& message);
    void broadcastMessage(const std::string& message);
    void handleDisconnect(std::shared_ptr<tcp::socket> socket);

private:
    boost::asio::io_context ioContext;
    tcp::acceptor acceptor;

    std::set<std::shared_ptr<tcp::socket>> activeConnections;

    std::map<std::shared_ptr<tcp::socket>, int> socketToPlayerID;

    std::unique_ptr<PlayerManager> playerManager; 
    std::unique_ptr<EntryHandler> entryHandler;
    std::unique_ptr<RegistrationHandler> registrationHandler;

    std::unique_ptr<ConnectionManager> connectionManager;
    std::unique_ptr<ServerGame> serverGame;
};
