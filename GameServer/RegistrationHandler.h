#ifndef REGISTRATION_HANDLER_H
#define REGISTRATION_HANDLER_H

#include <unordered_map>
#include <string>
#include <fstream>
#include <iostream>
class RegistrationHandler {
private:
    std::unordered_map<std::string, std::pair<std::string, int>> userDatabase; //  Имя -> (Пароль, Баланс)
    void loadDatabaseFromFile();
    void saveDatabaseToFile();
    bool userExists(const std::string& username);
    bool registerUser(const std::string& username, const std::string& password);


public:
    RegistrationHandler();
    std::string processRegistration(const std::string& data);
    void updateBalance(const std::string& username, int newBalance);
};

#endif

