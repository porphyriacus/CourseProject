#pragma once

#include <boost/asio.hpp>
#include <iostream>
#include <memory>
#include <set>
#include <string>

using boost::asio::ip::tcp;

// �����, �������������� ���� ���������� � ��������
class Session : public std::enable_shared_from_this<Session> {
public:
    // ����������� ��������� �����
    Session(tcp::socket socket) : socket_(std::move(socket)) {}

    // ������ ��������� ����������
    void start() {
        do_read();
    }

private:
    // ������ ������ �� �������
    void do_read() {
        auto self(shared_from_this());

        // ����������� ������ �� ������� ����� ������
        boost::asio::async_read_until(socket_, buffer_, '\n',
            [this, self](boost::system::error_code ec, std::size_t bytesTransferred) {
                if (!ec) {
                    std::istream is(&buffer_);
                    std::string message;
                    std::getline(is, message);

                    // ���� �������� ������ ���������, ��������� ������
                    if (message.empty()) {
                        std::cerr << "������: �������� ������ ���������!" << std::endl;
                        return;
                    }

                    std::cout << "�������� ���������: " << message << std::endl;

                    // ��������� ������� (��������, �����������)
                    std::string response;
                    if (message.find("REGISTRATION ") == 0) {
                        std::string data = message.substr(13);
                        if (registeredUsers.find(data) == registeredUsers.end()) {
                            registeredUsers.insert(data);
                            response = "����������� �������!\n";
                        }
                        else {
                            response = "������: ������������ ��� ����������!\n";
                        }
                    }
                    else {
                        response = "����������� �������!\n";
                    }

                    // �������� ������ �������
                    do_write(response);

                    // ���������� ������� �������
                    do_read();
                }
                else {
                    std::cerr << "������ ������: " << ec.message() << std::endl;
                }
            });
    }

    // �������� ������ �������
    void do_write(const std::string& message) {
        auto self(shared_from_this());

        boost::asio::async_write(socket_, boost::asio::buffer(message),
            [this, self](boost::system::error_code ec, std::size_t length) {
                if (ec) {
                    std::cerr << "������ ������: " << ec.message() << std::endl;
                }
            });
    }

    tcp::socket socket_;
    boost::asio::streambuf buffer_;

    // ������ �������� ������������������ �������������
    static inline std::set<std::string> registeredUsers;
};
