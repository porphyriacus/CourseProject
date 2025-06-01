#pragma once

#include "EntryHandler.h"

#include <cctype> // Для проверки числовых данных



//  Загрузка пользователей из файла "users.db"
void EntryHandler::loadDatabaseFromFile() {
    std::ifstream file("users.db");
    if (!file.is_open()) {
        std::cerr << "Ошибка: не удалось открыть файл базы данных (users.db)!" << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue; // Защита от пустых строк

        size_t delimiter1 = line.find(":");
        size_t delimiter2 = line.find(":", delimiter1 + 1);

        if (delimiter1 == std::string::npos || delimiter2 == std::string::npos) {
            std::cerr << "Ошибка: некорректная строка в файле users.db -> " << line << std::endl;
            continue;
        }

        std::string username = line.substr(0, delimiter1);
        std::string password = line.substr(delimiter1 + 1, delimiter2 - delimiter1 - 1);
        std::string balanceStr = line.substr(delimiter2 + 1);

        // Проверяем, содержит ли баланс только цифры
        if (!std::all_of(balanceStr.begin(), balanceStr.end(), ::isdigit)) {
            std::cerr << "Ошибка: некорректный баланс у пользователя " << username << std::endl;
            continue;
        }

        int balance = std::stoi(balanceStr);
        userDatabase[username] = std::make_pair(password, balance);
    }
    file.close();
}

// Сохранение базы данных обратно в файл
void EntryHandler::saveDatabaseToFile() {
    std::ofstream file("users.db", std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Ошибка: не удалось сохранить файл users.db!" << std::endl;
        return;
    }

    for (const auto& [username, data] : userDatabase) {
        file << username << ":" << data.first << ":" << data.second << "\n";
    }
    file.close();
}


std::string EntryHandler::processEntry(const std::string& data) {
    loadDatabaseFromFile();
    size_t delimiter = data.find(":");
    if (delimiter == std::string::npos) {
        return "Ошибка: неверный формат данных для входа\n";
    }

    std::string username = data.substr(0, delimiter);
    std::string password = data.substr(delimiter + 1);

    auto it = userDatabase.find(username);
    if (it == userDatabase.end()) {
        return "Ошибка: пользователь не найден!\n";
    }
    if (it->second.first != password) {
        return "Ошибка: неверный пароль!\n";
    }

    // Берём баланс игрока из базы и отправляем данные серверу
    int balance = it->second.second;

    return "Вход успешен! " + username + ":" + std::to_string(balance) + "\n";
}
