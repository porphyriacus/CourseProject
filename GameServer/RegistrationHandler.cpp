#include "RegistrationHandler.h"

RegistrationHandler::RegistrationHandler() {
    loadDatabaseFromFile();
}

// �������� ���� ������������� �� ����� ("username:password:balance")
void RegistrationHandler::loadDatabaseFromFile() {
    std::ifstream file("users.db");
    std::string line;
    while (std::getline(file, line)) {
        size_t delimiter1 = line.find(":");
        size_t delimiter2 = line.find(":", delimiter1 + 1);
        if (delimiter1 != std::string::npos && delimiter2 != std::string::npos) {
            std::string username = line.substr(0, delimiter1);
            std::string password = line.substr(delimiter1 + 1, delimiter2 - delimiter1 - 1);
            int balance = std::stoi(line.substr(delimiter2 + 1));

            userDatabase[username] = std::make_pair(password, balance);
        }
    }
    file.close();
}

//  ���������� ���� ������ � ����
void RegistrationHandler::saveDatabaseToFile() {
    std::ofstream file("users.db", std::ios::binary);
    for (const auto& [user, data] : userDatabase) {
        file << user << ":" << data.first << ":" << data.second << "\n";
    }
    file.close();
}

//  �������� ������������� ������������
bool RegistrationHandler::userExists(const std::string& username) {
    return userDatabase.find(username) != userDatabase.end();
}

// ����������� ������ ������������ (������ � �������� 10000)
bool RegistrationHandler::registerUser(const std::string& username, const std::string& password) {
    if (userExists(username)) return false; // ������������ ��� ����������

    userDatabase[username] = std::make_pair(password, 10000); //  ��������� ������ 10000
    saveDatabaseToFile();
    return true;
}

// ��������� ������� "REGISTRATION username:password"
std::string RegistrationHandler::processRegistration(const std::string& data) {
    size_t delimiter = data.find(":");
    if (delimiter == std::string::npos) {
        return "�������� ������ ������� �����������.\n";
    }
    std::string username = data.substr(0, delimiter);
    std::string password = data.substr(delimiter + 1);

    if (username.empty() || password.empty()) {
        return "������: ��� ������������ ��� ������ �� ����� ���� �������.\n";
    }

    if (userExists(username)) {
        return "������: ������������ ��� ����������!\n";
    }

    //  ������������ ������ ������������
    try {
        if (registerUser(username, password)) {
            return "����������� �������! ��� ��������� ������: 10000\n";
        }
        else {
            return "������ �����������.\n";
        }
    }
    catch (const std::exception& ex) {
        return std::string("������ �����������: ") + ex.what() + "\n";
    }
}

void RegistrationHandler::updateBalance(const std::string& username, int newBalance) {
    if (!userExists(username)) return; // ���� ������ ��� � ����, �� ���������

    userDatabase[username].second = newBalance; // ��������� ������
    saveDatabaseToFile();
}
