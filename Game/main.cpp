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

// Обработчик серверных сообщений
void HandleServerMessage(String^ msg) {
    ServerMessageProcessor::ProcessMessage(msg); //  Передаём сообщение обработчику
}


// Фоновый поток для обработки сообщений от сервера
void NetworkProcessingThread(msclr::gcroot<GameNetwork^> gameNetwork) {
    while (gameNetwork->isConnected()) {
        String^ response = gameNetwork->receiveResponse(); // Получаем ответ
        HandleServerMessage(response); // Передаём обработку серверного ответа
    }
}


int main() {
    setlocale(LC_ALL, "Russian");

    String^ serverIP = "127.0.0.1";
    int serverPort = 5555;

    // Получаем единственный экземпляр GameNetwork
    msclr::gcroot<GameNetwork^> gameNetwork = GameNetwork::getInstance(serverIP, serverPort);

    // Проверяем соединение
    if (!gameNetwork->isConnected()) {
        Console::WriteLine("Ошибка: Не удалось подключиться к серверу.");
        return -1;
    }

    // Запускаем асинхронный приём сообщений от сервера
    gameNetwork->startAsync(gcnew System::Action<String^>(HandleServerMessage));

    // Запускаем серверный обработчик в отдельном потоке
    std::thread serverThread(NetworkProcessingThread, gameNetwork);
    serverThread.detach(); // Фоновый поток

    // Настройка UI
    Application::EnableVisualStyles();
    Application::SetCompatibleTextRenderingDefault(false);

    

    while (!GlobalController::shouldExit) {
        FirstWindow^ mainForm = gcnew FirstWindow();
        DialogResult dr = mainForm->ShowDialog();

        if (dr == DialogResult::Cancel) {
            break; // Выход из цикла
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
