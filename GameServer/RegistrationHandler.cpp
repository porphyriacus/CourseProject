#include "RegistrationHandler.h"

RegistrationHandler::RegistrationHandler() {
    loadDatabaseFromFile();
}

// Загрузка базы пользователей из файла ("username:password:balance")
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

//  Сохранение базы данных в файл
void RegistrationHandler::saveDatabaseToFile() {
    std::ofstream file("users.db", std::ios::binary);
    for (const auto& [user, data] : userDatabase) {
        file << user << ":" << data.first << ":" << data.second << "\n";
    }
    file.close();
}

//  Проверка существования пользователя
bool RegistrationHandler::userExists(const std::string& username) {
    return userDatabase.find(username) != userDatabase.end();
}

// Регистрация нового пользователя (теперь с балансом 10000)
bool RegistrationHandler::registerUser(const std::string& username, const std::string& password) {
    if (userExists(username)) return false; // Пользователь уже существует

    userDatabase[username] = std::make_pair(password, 10000); //  Добавляем баланс 10000
    saveDatabaseToFile();
    return true;
}

// Обработка команды "REGISTRATION username:password"
std::string RegistrationHandler::processRegistration(const std::string& data) {
    size_t delimiter = data.find(":");
    if (delimiter == std::string::npos) {
        return "Неверный формат команды регистрации.\n";
    }
    std::string username = data.substr(0, delimiter);
    std::string password = data.substr(delimiter + 1);

    if (username.empty() || password.empty()) {
        return "Ошибка: имя пользователя или пароль не могут быть пустыми.\n";
    }

    if (userExists(username)) {
        return "Ошибка: пользователь уже существует!\n";
    }

    //  Регистрируем нового пользователя
    try {
        if (registerUser(username, password)) {
            return "Регистрация успешна! Ваш стартовый баланс: 10000\n";
        }
        else {
            return "Ошибка регистрации.\n";
        }
    }
    catch (const std::exception& ex) {
        return std::string("Ошибка регистрации: ") + ex.what() + "\n";
    }
}

void RegistrationHandler::updateBalance(const std::string& username, int newBalance) {
    if (!userExists(username)) return; // Если игрока нет в базе, не обновляем

    userDatabase[username].second = newBalance; // Обновляем баланс
    saveDatabaseToFile();
}
