#pragma once
#include <string>
#include "NativeNetworkClient.h"
#include <msclr/gcroot.h>

using namespace System;

public ref class GameNetwork
{
private:
    // ��������� �� ��������� �������.
    NativeNetworkClient* nativeClient;
    // ������� ��� �������� ������������ ������.
    System::Action<String^>^ messageCallback;

public:
    void NativeToManagedBridge(const std::string& response);


    // ��������� ����������� ���������. ����� ����� �������� �������������� �������.
    void HandleMessage(String^ msg);

    // ��������.
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

// ���������� ���������� ��� ������ �� ��������� ����.
// ���������� msclr::gcroot � ��������� msclr::.
extern msclr::gcroot<GameNetwork^> g_networkInstance;

// ��������� �������, ���������� �� native-����, ������� ������� ����� � GameNetwork.
void FreeNetworkMessageHandler(const std::string& response);