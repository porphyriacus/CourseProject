#pragma once

#include "include.h"
#include <map>
#include <queue>
#include <memory>
#include <boost/asio.hpp>  //  ���������� Boost.Asio

#include <boost/asio.hpp>
#include <deque>
#include <iostream>
#include <map>
#include <memory>
#include <string>
//
//// ��� �������� ����
//using boost::asio::ip::tcp;
//
//// ����� ������������ shared_from_this() � Connection
//class Connection : public std::enable_shared_from_this<Connection> {
//public:
//    Connection(boost::asio::io_context& ioContext,
//        std::shared_ptr<tcp::socket> sock)
//        : socket(sock),
//        strand(boost::asio::make_strand(ioContext))
//    {
//    }
//
//    // ��������������� ����� �������� ���������.
//    void sendData(const std::string& message) {
//        // ������� ����� � ����������.
//        auto safeResponse = std::make_shared<std::string>(message);
//
//        // �������� ����� strand, ����� ������������� ������������������ ������� ��� ����� ������.
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
//        // ����������� shared_ptr ��� ����������� ��������� ����� ������� Connection 
//        auto self = shared_from_this();
//        boost::asio::async_write(*socket,
//            boost::asio::buffer(*writeQueue.front()),
//            boost::asio::bind_executor(strand,
//                [this, self](boost::system::error_code ec, std::size_t /*bytes_transferred*/) {
//                    if (!ec) {
//                        // �������� �������� � ������� ��������� �� �������.
//                        writeQueue.pop_front();
//                        if (!writeQueue.empty()) {
//                            // ���� � ������� ���� ��� ���������, ��������� �������� ����������.
//                            doWrite();
//                        }
//                    }
//                    else {
//                        std::cerr << "������ �������� ������: " << ec.message() << std::endl;
//                        // ����� ����� ���������� ������, ��������, ������� ����������.
//                    }
//                }
//            )
//        );
//    }
//
//    // ����� ������.
//    std::shared_ptr<tcp::socket> socket;
//    boost::asio::strand<boost::asio::io_context::executor_type> strand;
//    std::deque<std::shared_ptr<std::string>> writeQueue;
//};
//
//
//// �������� ����������, ������� ������ ������ ������� Connection
//class ConnectionManager {
//private:
//    std::map<int, std::shared_ptr<Connection>> activeConnections; // �������� ID -> ������ Connection
//
//public:
//    // ������������ ������ � ������� ��� ���� Connection
//    void registerConnection(int playerID, std::shared_ptr<tcp::socket> socket) {
//        activeConnections[playerID] = std::make_shared<Connection>( socket);
//    }
//
//    void handleDisconnect(int playerID) {
//        auto it = activeConnections.find(playerID);
//        if (it != activeConnections.end()) {
//            activeConnections.erase(it);
//            std::cout << "����� � ID " << playerID << " ������� ��������.\n";
//        }
//        else {
//            std::cerr << "������: ������� ��������� ��������������� ������ (ID " << playerID << ").\n";
//        }
//    }
//
//    // ���������� ��������� ����������� ������
//    void sendData(int playerID, const std::string& message) {
//        auto it = activeConnections.find(playerID);
//        if (it == activeConnections.end()) {
//            std::cerr << "ConnectionManager:\t������: ����� � ID " << playerID << " �� ���������.\n";
//            return;
//        }
//        // �������� ��������� ����� Connection
//        it->second->sendData(message);
//    }
//};
//
//



using boost::asio::ip::tcp;
class ConnectionManager {
private:
    std::map<int, std::shared_ptr<tcp::socket>> activeConnections; // ID -> �����

public:
    int getPlayerIDBySocket(std::shared_ptr<tcp::socket> socket) {
        for (const auto& pair : activeConnections) {
            if (pair.second == socket) {
                return pair.first;
            }
        }
        return -1;
    }

    // ������������ �����������
    void registerConnection(int playerID, std::shared_ptr<tcp::socket> socket) {
        activeConnections[playerID] = socket;
    }

    // ������� ����������� �� ID
    void handleDisconnect(int playerID) {
        auto it = activeConnections.find(playerID);
        if (it != activeConnections.end()) {
            activeConnections.erase(it);
            std::cout << "����� � ID " << playerID << " ������� ��������." << std::endl;
        }
        else {
            std::cerr << "������: ������� ��������� ��������������� ������ (ID " << playerID << ").\n";
        }
    }

    // ���������� ��������� ����������� ������
    void sendData(int playerID, const std::string& message) {
        auto it = activeConnections.find(playerID);
        if (it == activeConnections.end()) {
            std::cerr << "ConnectionManager:\t������: ����� � ID " << playerID << " �� ���������.\n";
            return;
        }
        auto socket = it->second;
        auto safeResponse = std::make_shared<std::string>(message);
        boost::asio::async_write(*socket, boost::asio::buffer(*safeResponse),
            [this, socket, safeResponse](boost::system::error_code ec, std::size_t) {
                if (ec) {
                    std::cerr << "ConnectionManager:\t������ �������� ������ ������: " << ec.message() << std::endl;
                }
                else {
                    std::cout << "ConnectionManager:\t��������� ���������� ������: " << *safeResponse << std::endl;
                }
            });
    }
};


