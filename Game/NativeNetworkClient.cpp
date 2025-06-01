#include "NativeNetworkClient.h"
#include <boost/asio/read_until.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/streambuf.hpp>
#include <sstream>
#include <functional>

// NativeNetworkClient.cpp

#include <iostream>
#include <thread>

NativeNetworkClient::NativeNetworkClient(const std::string& serverIP, int port)
    : socket(ioContext)
{
    try {
        // �������� ���� ��������� �������� ����� ��� �����������.
        tcp::resolver resolver(ioContext);
        auto endpoints = resolver.resolve(serverIP, std::to_string(port));
        boost::asio::connect(socket, endpoints);
        std::cout << "Connected to server " << serverIP << ":" << port << std::endl;
    }
    catch (const std::exception& ex) {
        std::cerr << "Connection error: " << ex.what() << std::endl;
    }

    // ��������� ioContext � ��������� ������.
    try {
        std::thread([this]() { ioContext.run(); }).detach();
    }
    catch (const std::exception& ex) {
        std::cerr << "Error running ioContext: " << ex.what() << std::endl;
    }
}

NativeNetworkClient::~NativeNetworkClient()
{
    try {
        if (socket.is_open())
            socket.close();
        ioContext.stop();
    }
    catch (const std::exception& ex) {
        std::cerr << "Error closing socket: " << ex.what() << std::endl;
    }
}

bool NativeNetworkClient::isConnected() const {
    return socket.is_open();
}

void NativeNetworkClient::sendCommand(const std::string& command) {
    try {
        // ������� ����������� �������� ����� ������.
        std::string cmd = command + "\n";
        boost::asio::write(socket, boost::asio::buffer(cmd));
    }
    catch (const std::exception& ex) {
        std::cerr << "Error sending command: " << ex.what() << std::endl;
    }
}

std::string NativeNetworkClient::receiveResponse() {
    try {
        boost::asio::streambuf buf;
        boost::asio::read_until(socket, buf, "\n");
        std::istream is(&buf);
        std::string response;
        std::getline(is, response); // ������ ����� ������

        return response;
    }
    catch (const std::exception& ex) {
        std::cerr << "Error receiving response: " << ex.what() << std::endl;
        return "";
    }
}

void NativeNetworkClient::asyncRead(std::function<void(const std::string&)> callback) {
    try {
        // ������ ����� � ����������� ���������.
        std::shared_ptr<boost::asio::streambuf> buf = std::make_shared<boost::asio::streambuf>();
        boost::asio::async_read_until(
            socket,
            *buf,
            "\n",
            std::bind(&NativeNetworkClient::asyncReadHandler, this, buf, callback,
                std::placeholders::_1, std::placeholders::_2)
        );
    }
    catch (const std::exception& ex) {
        std::cerr << "Error in asyncRead: " << ex.what() << std::endl;
    }
}

void NativeNetworkClient::asyncReadHandler(std::shared_ptr<boost::asio::streambuf> buf,
    std::function<void(std::string)> callback,
    const boost::system::error_code& ec,
    std::size_t length) {
    try {
        if (!ec) {
            std::istream is(buf.get());
            std::string line;
            std::getline(is, line);
            std::string response(line); // ������ ����� ������

            callback(response); // ������� �����

            if (socket.is_open())
                asyncRead(callback);
        }
        else {
            std::cerr << "Error: " << ec.message() << std::endl;
            callback("");
        }
    }
    catch (const std::exception& ex) {
        std::cerr << "Error in asyncReadHandler: " << ex.what() << std::endl;
    }
}

/*
NativeNetworkClient::NativeNetworkClient(const std::string& serverIP, int port)
    : socket(ioContext)
{
    try {
        tcp::resolver resolver(ioContext);
        auto endpoints = resolver.resolve(serverIP, std::to_string(port)); // ���� ��� ��������� ������� �����
                                                                           // (������ ����� ����� ���� �� ����� ��������� ����)
        boost::asio::connect(socket, endpoints);
        std::cout << "Connected to server " << serverIP << ":" << port << std::endl;
    }
    catch (std::exception& ex) {
        std::cerr << "Connection error: " << ex.what() << std::endl;
    }

    // ��������� ioContext � ��������� ������.
    std::thread([this]() { ioContext.run(); }).detach(); // detach(): ����� ���������� �������� ����������,
                                                         // ���� ���� �������� ��� ����������
}

NativeNetworkClient::~NativeNetworkClient()
{
    if (socket.is_open())
        socket.close();
    ioContext.stop();
}

bool NativeNetworkClient::isConnected() const {
    return socket.is_open();
}

void NativeNetworkClient::sendCommand(const std::string& command) {
    // ������� ����������� �������� ����� ������.
    std::string cmd = command + "\n";
    boost::asio::write(socket, boost::asio::buffer(cmd)); // boost::asio::async_write() ?
}

std::string NativeNetworkClient::receiveResponse() {
    boost::asio::streambuf buf;
    boost::asio::read_until(socket, buf, "\n");
    std::istream is(&buf);
    std::string response;
    std::getline(is, response); // ������ ����� ������

    return response; // response ���������� �� �������� ?
}


void NativeNetworkClient::asyncRead(std::function<void(const std::string&)> callback) {

    // ������ ����� � ����������� ���������, ����� �� ��������� �� ���������� ������������ ������
    std::shared_ptr<boost::asio::streambuf> buf = std::make_shared<boost::asio::streambuf>();
    // ���������� std::bind ��� �������� ����������� ������ ������-�������
    boost::asio::async_read_until(
        socket,
        *buf,
        "\n",
        std::bind(&NativeNetworkClient::asyncReadHandler,
            this,
            buf,
            callback,
            std::placeholders::_1,
            std::placeholders::_2)
    );
}

void NativeNetworkClient::asyncReadHandler(std::shared_ptr<boost::asio::streambuf> buf,
    std::function<void(std::string)> callback,
    const boost::system::error_code& ec,
    std::size_t length) {
    if (!ec) {
        std::istream is(buf.get());
        std::string line;
        std::getline(is, line);
        std::string response(line); // ������ ����� ������


        callback(response); // ������� ���������� �����

        if (socket.is_open()) {
            asyncRead(callback);
        }
    }
    else {
        std::cerr << "������: " << ec.message() << std::endl;
        callback("");
    }
}

*/
