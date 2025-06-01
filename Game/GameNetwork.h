#pragma once
#include <string>
#include "NativeNetworkClient.h"
#include <msclr/gcroot.h>

using namespace System;

public ref class GameNetwork
{
private:
    // Указатель на нативного клиента.
    NativeNetworkClient* nativeClient;
    // Делегат для передачи управляемого ответа.
    System::Action<String^>^ messageCallback;

public:
    void NativeToManagedBridge(const std::string& response);


    // Обработка полученного сообщения. Здесь можно добавить дополнительный парсинг.
    void HandleMessage(String^ msg);

    // Синглтон.
    static GameNetwork^ instance = nullptr;
    static Object^ lockObj = gcnew Object();

public:
    GameNetwork(String^ serverIP, int port);
    ~GameNetwork();
    !GameNetwork();

    static GameNetwork^ getInstance(String^ serverIP, int port);
    void startAsync(System::Action<String^>^ onMessage);
    void sendCommand(String^ command);
    String^ receiveResponse();
    bool isConnected();
};

// Глобальная переменная для вызова из нативного кода.
// Используем msclr::gcroot с префиксом msclr::.
extern msclr::gcroot<GameNetwork^> g_networkInstance;

// Свободная функция, вызываемая из native-кода, которая передаёт вызов в GameNetwork.
void FreeNetworkMessageHandler(const std::string& response);