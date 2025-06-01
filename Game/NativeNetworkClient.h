#pragma once
#include <string>
#include <functional>
#include <memory>
#include <thread>
#include <iostream>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class NativeNetworkClient
{
public:
    NativeNetworkClient(const std::string& serverIP, int port);
    ~NativeNetworkClient();

    bool isConnected() const;
    void sendCommand(const std::string& command);
    std::string receiveResponse();

    // Асинхронное чтение с сервером. Callback принимает std::string.
    void asyncRead(std::function<void(const std::string&)> callback);


private:
    // Обработчик завершения асинхронного чтения.
    void asyncReadHandler(std::shared_ptr<boost::asio::streambuf> buf,
        std::function<void(std::string)> callback,
        const boost::system::error_code& ec,
        std::size_t length);

private:
    boost::asio::io_context ioContext;
    tcp::socket socket;
};
