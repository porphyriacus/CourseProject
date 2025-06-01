#pragma once

#include "include.h"
#include <map>
#include <queue>
#include <memory>
#include <boost/asio.hpp>  //  Подключаем Boost.Asio

#include <boost/asio.hpp>
#include <deque>
#include <iostream>
#include <map>
#include <memory>
#include <string>
//
//// Для коротких имен
//using boost::asio::ip::tcp;
//
//// Чтобы использовать shared_from_this() в Connection
//class Connection : public std::enable_shared_from_this<Connection> {
//public:
//    Connection(boost::asio::io_context& ioContext,
//        std::shared_ptr<tcp::socket> sock)
//        : socket(sock),
//        strand(boost::asio::make_strand(ioContext))
//    {
//    }
//
//    // Сериализованный вызов отправки сообщения.
//    void sendData(const std::string& message) {
//        // Создаем буфер с сообщением.
//        auto safeResponse = std::make_shared<std::string>(message);
//
//        // Вызываем через strand, чтобы гарантировать последовательность вызовов для этого сокета.
//        boost::asio::post(strand,
//            [this, safeResponse]() {
//                bool writeInProgress = !writeQueue.empty();
//                writeQueue.push_back(safeResponse);
//                if (!writeInProgress) {
//                    doWrite();
//                }
//            }
//        );
//    }
//
//private:
//    void doWrite() {
//        // Захватываем shared_ptr для корректного удержания жизни объекта Connection 
//        auto self = shared_from_this();
//        boost::asio::async_write(*socket,
//            boost::asio::buffer(*writeQueue.front()),
//            boost::asio::bind_executor(strand,
//                [this, self](boost::system::error_code ec, std::size_t /*bytes_transferred*/) {
//                    if (!ec) {
//                        // Успешная отправка – удаляем сообщение из очереди.
//                        writeQueue.pop_front();
//                        if (!writeQueue.empty()) {
//                            // Если в очереди есть ещё сообщения, запускаем отправку следующего.
//                            doWrite();
//                        }
//                    }
//                    else {
//                        std::cerr << "Ошибка отправки данных: " << ec.message() << std::endl;
//                        // Здесь можно обработать ошибку, например, закрыть соединение.
//                    }
//                }
//            )
//        );
//    }
//
//    // Члены класса.
//    std::shared_ptr<tcp::socket> socket;
//    boost::asio::strand<boost::asio::io_context::executor_type> strand;
//    std::deque<std::shared_ptr<std::string>> writeQueue;
//};
//
//
//// Менеджер соединений, который теперь хранит объекты Connection
//class ConnectionManager {
//private:
//    std::map<int, std::shared_ptr<Connection>> activeConnections; // Привязка ID -> объект Connection
//
//public:
//    // Регистрируем игрока и создаем для него Connection
//    void registerConnection(int playerID, std::shared_ptr<tcp::socket> socket) {
//        activeConnections[playerID] = std::make_shared<Connection>( socket);
//    }
//
//    void handleDisconnect(int playerID) {
//        auto it = activeConnections.find(playerID);
//        if (it != activeConnections.end()) {
//            activeConnections.erase(it);
//            std::cout << "Игрок с ID " << playerID << " успешно отключён.\n";
//        }
//        else {
//            std::cerr << "Ошибка: Попытка отключить несуществующего игрока (ID " << playerID << ").\n";
//        }
//    }
//
//    // Отправляем сообщение конкретному игроку
//    void sendData(int playerID, const std::string& message) {
//        auto it = activeConnections.find(playerID);
//        if (it == activeConnections.end()) {
//            std::cerr << "ConnectionManager:\tОшибка: Игрок с ID " << playerID << " не подключён.\n";
//            return;
//        }
//        // Передаем сообщение через Connection
//        it->second->sendData(message);
//    }
//};
//
//



using boost::asio::ip::tcp;
class ConnectionManager {
private:
    std::map<int, std::shared_ptr<tcp::socket>> activeConnections; // ID -> сокет

public:
    int getPlayerIDBySocket(std::shared_ptr<tcp::socket> socket) {
        for (const auto& pair : activeConnections) {
            if (pair.second == socket) {
                return pair.first;
            }
        }
        return -1;
    }

    // Регистрируем подключение
    void registerConnection(int playerID, std::shared_ptr<tcp::socket> socket) {
        activeConnections[playerID] = socket;
    }

    // Удаляем подключение по ID
    void handleDisconnect(int playerID) {
        auto it = activeConnections.find(playerID);
        if (it != activeConnections.end()) {
            activeConnections.erase(it);
            std::cout << "Игрок с ID " << playerID << " успешно отключён." << std::endl;
        }
        else {
            std::cerr << "Ошибка: Попытка отключить несуществующего игрока (ID " << playerID << ").\n";
        }
    }

    // Отправляем сообщение конкретному игроку
    void sendData(int playerID, const std::string& message) {
        auto it = activeConnections.find(playerID);
        if (it == activeConnections.end()) {
            std::cerr << "ConnectionManager:\tОшибка: Игрок с ID " << playerID << " не подключён.\n";
            return;
        }
        auto socket = it->second;
        auto safeResponse = std::make_shared<std::string>(message);
        boost::asio::async_write(*socket, boost::asio::buffer(*safeResponse),
            [this, socket, safeResponse](boost::system::error_code ec, std::size_t) {
                if (ec) {
                    std::cerr << "ConnectionManager:\tОшибка отправки данных игроку: " << ec.message() << std::endl;
                }
                else {
                    std::cout << "ConnectionManager:\tСообщение отправлено игроку: " << *safeResponse << std::endl;
                }
            });
    }
};


