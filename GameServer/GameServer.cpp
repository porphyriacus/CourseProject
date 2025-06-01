#pragma once
#include "include.h"

/*
using boost::asio::ip::tcp;

class GameServer {
public:
    GameServer(boost::asio::io_context& io_context, int port)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)) {
        startAccept();
    }

private:
    tcp::acceptor acceptor_;

    void startAccept() {
        setlocale(LC_ALL, "Russian");
        std::cout << "Сервер запущен. Ожидание подключения..." << std::endl;

        acceptor_.async_accept([this](boost::system::error_code ec, tcp::socket socket) {
            if (!ec) {
                setlocale(LC_ALL, "Russian");
                std::cout << "Игрок подключился!" << std::endl;
                sendMessage(std::move(socket));
            }
            startAccept();
            });
    }

    void sendMessage(tcp::socket socket) {
        setlocale(LC_ALL, "Russian");
        std::wstring text = L"Добро пожаловать в покер!";

        // Преобразуем в UTF-8 без Boost.Locale
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        std::string utf8_message = converter.to_bytes(text);

        boost::asio::write(socket, boost::asio::buffer(utf8_message));
    }
};

int main() {
    setlocale(LC_ALL, "Russian");
    std::cout << "Добро пожаловать в покер!" << std::endl;

    boost::asio::io_context io_context;
    GameServer server(io_context, 5555);
    io_context.run();
    return 0;
}


*/
#include "Server.h"
#include <iostream>

int main() {
    const int SERVER_PORT = 5555;
    setlocale(LC_ALL, "Russian");
    try {
        Server server(SERVER_PORT);
        //std::cout << "Сервер запущен на порту " << SERVER_PORT << std::endl;
        server.start(); // Запуск основного цикла обработки подключений
    }
    catch (const std::exception& e) {
        std::cerr << "Ошибка сервера: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
