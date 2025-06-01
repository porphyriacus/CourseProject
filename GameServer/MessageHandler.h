#pragma once
#include "include.h"

class MessageHandler {
public:
    std::string parseMessage(std::string rawMessage);  // ����������� ����� � JSON
    void sendMessageToPlayer(int playerID, std::string message);
    void broadcastMessage(std::string message);  // �������� ����
};
