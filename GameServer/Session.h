#pragma once

#include <boost/asio.hpp>
#include <iostream>
#include <memory>
#include <set>
#include <string>

using boost::asio::ip::tcp;

// Класс, обрабатывающий одно соединение с клиентом
class Session : public std::enable_shared_from_this<Session> {
public:
    // Конструктор принимает сокет
    Session(tcp::socket socket) : socket_(std::move(socket)) {}

    // Запуск обработки соединения
    void start() {
        do_read();
    }

private:
    // Чтение данных от клиента
    void do_read() {
        auto self(shared_from_this());

        // Асинхронное чтение до символа новой строки
        boost::asio::async_read_until(socket_, buffer_, '\n',
            [this, self](boost::system::error_code ec, std::size_t bytesTransferred) {
                if (!ec) {
                    std::istream is(&buffer_);
                    std::string message;
                    std::getline(is, message);

                    // Если получено пустое сообщение, завершаем сессию
                    if (message.empty()) {
                        std::cerr << "Ошибка: получено пустое сообщение!" << std::endl;
                        return;
                    }

                    std::cout << "Получено сообщение: " << message << std::endl;

                    // Обработка команды (например, регистрация)
                    std::string response;
                    if (message.find("REGISTRATION ") == 0) {
                        std::string data = message.substr(13);
                        if (registeredUsers.find(data) == registeredUsers.end()) {
                            registeredUsers.insert(data);
                            response = "Регистрация успешна!\n";
                        }
                        else {
                            response = "Ошибка: пользователь уже существует!\n";
                        }
                    }
                    else {
                        response = "Неизвестная команда!\n";
                    }

                    // Отправка ответа клиенту
                    do_write(response);

                    // Продолжаем слушать клиента
                    do_read();
                }
                else {
                    std::cerr << "Ошибка чтения: " << ec.message() << std::endl;
                }
            });
    }

    // Отправка ответа клиенту
    void do_write(const std::string& message) {
        auto self(shared_from_this());

        boost::asio::async_write(socket_, boost::asio::buffer(message),
            [this, self](boost::system::error_code ec, std::size_t length) {
                if (ec) {
                    std::cerr << "Ошибка записи: " << ec.message() << std::endl;
                }
            });
    }

    tcp::socket socket_;
    boost::asio::streambuf buffer_;

    // Пример хранения зарегистрированных пользователей
    static inline std::set<std::string> registeredUsers;
};
