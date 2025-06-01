#pragma once
#include <string>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <iostream>
#include "PlayerManager.h"

class EntryHandler {
private:
    //  ���� �������������: username - (password, balance)
    std::unordered_map<std::string, std::pair<std::string, int>> userDatabase;
    // ��������� �� �������� �������
    PlayerManager* playerManager;

    void loadDatabaseFromFile();
    void saveDatabaseToFile();

public:
    EntryHandler(PlayerManager* pm) : playerManager(pm) { loadDatabaseFromFile(); }

    std::string processEntry(const std::string& data);
};

