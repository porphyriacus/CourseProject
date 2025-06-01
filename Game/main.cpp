#include "GameNetwork.h"
#include "FirstWindow.h"
#include "ServerMessageProcessor.h"

#include <windows.h>
#include <vcclr.h>
//#include "FormManager.h"

using namespace System;
using namespace System::Windows::Forms;
using namespace Game;

//#pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")

#include <iostream>
#include <thread>
#include <msclr/gcroot.h>

// ���������� ��������� ���������
void HandleServerMessage(String^ msg) {
    ServerMessageProcessor::ProcessMessage(msg); //  ������� ��������� �����������
}


// ������� ����� ��� ��������� ��������� �� �������
void NetworkProcessingThread(msclr::gcroot<GameNetwork^> gameNetwork) {
    while (gameNetwork->isConnected()) {
        String^ response = gameNetwork->receiveResponse(); // �������� �����
        HandleServerMessage(response); // ������� ��������� ���������� ������
    }
}


int main() {
    setlocale(LC_ALL, "Russian");

    String^ serverIP = "127.0.0.1";
    int serverPort = 5555;

    // �������� ������������ ��������� GameNetwork
    msclr::gcroot<GameNetwork^> gameNetwork = GameNetwork::getInstance(serverIP, serverPort);

    // ��������� ����������
    if (!gameNetwork->isConnected()) {
        Console::WriteLine("������: �� ������� ������������ � �������.");
        return -1;
    }

    // ��������� ����������� ���� ��������� �� �������
    gameNetwork->startAsync(gcnew System::Action<String^>(HandleServerMessage));

    // ��������� ��������� ���������� � ��������� ������
    std::thread serverThread(NetworkProcessingThread, gameNetwork);
    serverThread.detach(); // ������� �����

    // ��������� UI
    Application::EnableVisualStyles();
    Application::SetCompatibleTextRenderingDefault(false);

    

    while (!GlobalController::shouldExit) {
        FirstWindow^ mainForm = gcnew FirstWindow();
        DialogResult dr = mainForm->ShowDialog();

        if (dr == DialogResult::Cancel) {
            break; // ����� �� �����
        }
        else if (dr == DialogResult::OK) {
            Entry^ windowA = gcnew Entry();
            windowA->ShowDialog();
        }
        else if (dr == DialogResult::Retry) {
            MyForm^ windowB = gcnew MyForm();
            windowB->ShowDialog();
        }
    }

    return 0;
}
