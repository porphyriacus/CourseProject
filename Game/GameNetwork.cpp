#include "GameNetwork.h"
#include <msclr/marshal_cppstd.h>

using namespace msclr::interop;

// ���������� ���������� ������ ����� msclr::gcroot.
// ��� ��������� �������� ������ GameNetwork �� �������� callback?�������.
msclr::gcroot<GameNetwork^> g_networkInstance;

//
// ��������� �������, ���������� �� native-����.
// ��� �������� std::string � ������� ��������� � �����-���� GameNetwork.
void FreeNetworkMessageHandler(const std::string& response) {
    if (g_networkInstance.operator->()) {
        g_networkInstance->NativeToManagedBridge(response);
    }
}


GameNetwork::GameNetwork(String^ serverIP, int port)
{
    std::string ip = marshal_as<std::string>(serverIP);
    nativeClient = new NativeNetworkClient(ip, port);
    // ��������� ���� ��������� � ���������� ����������.
    g_networkInstance = this;
}

GameNetwork::~GameNetwork() {
    this->!GameNetwork();
}

GameNetwork::!GameNetwork() {
    if (nativeClient != nullptr) {
        delete nativeClient;
        nativeClient = nullptr;
    }
}

GameNetwork^ GameNetwork::getInstance(String^ serverIP, int port) {
    if (instance == nullptr) {
        System::Threading::Monitor::Enter(lockObj);
        if (instance == nullptr) {
            instance = gcnew GameNetwork(serverIP, port);
        }
        System::Threading::Monitor::Exit(lockObj);
    }
    return instance;
}

void GameNetwork::startAsync(System::Action<String^>^ onMessage) {
    messageCallback = onMessage;
    // ������� � �������� ������ ��������� ������� FreeNetworkMessageHandler.
    nativeClient->asyncRead(FreeNetworkMessageHandler);
}

void GameNetwork::NativeToManagedBridge(const std::string& response) {
    String^ managedResponse = gcnew String(response.c_str());
    HandleMessage(managedResponse);
}


void GameNetwork::HandleMessage(String^ msg) {
    if (messageCallback != nullptr && !String::IsNullOrEmpty(msg)) {
        // ����� ����� �������� �������������� ������� ��� ���������.
        messageCallback->Invoke(msg);
    }
}

void GameNetwork::sendCommand(String^ command) {
    std::string nativeCommand = marshal_as<std::string>(command);
    nativeClient->sendCommand(nativeCommand);
}

String^ GameNetwork::receiveResponse() {
    std::string response = nativeClient->receiveResponse();
    std::string safeResponse = std::string(response); // �������� ������

    return gcnew String(safeResponse.c_str()); // ������ .c_str() ���������� �� ���������� �����
}

bool GameNetwork::isConnected() {
    return nativeClient->isConnected();
}
