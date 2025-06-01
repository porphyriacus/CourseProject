#pragma once

#include <string>
#include "Table.h"
using namespace Game;
using namespace System;
using namespace System::Windows::Forms;
using namespace System::Collections::Generic;

public ref class GlobalController {
public:
    static bool shouldExit = false; // Объявляем внутри класса!

    static void StopApp() {
        shouldExit = true;
    }
};

/*

 broadcastCallback_("CURRENT_BET: " + std::to_string(currentBet) +
            ". POT: " + std::to_string(bettingSystem_->getPot()) + "\n");
*/

ref class ServerMessageProcessor {
private:
    static Table^ tableForm = nullptr;
    static System::Windows::Forms::Timer^ updateTimer;
    static bool isCreatingTable = false;

public:
    // Инициализация таймера обновлений (можно уменьшить интервал, если изменений много)
    static void Initialize() {
        try {
            updateTimer = gcnew System::Windows::Forms::Timer();
            updateTimer->Interval = 100; // 100 мс — минимальная задержка для обновления UI
            updateTimer->Tick += gcnew EventHandler(&ServerMessageProcessor::OnUpdateTimerTick);
            updateTimer->Start();
        }
        catch (Exception^ ex) {
            Console::WriteLine("Ошибка инициализации таймера: " + ex->Message);
        }
    }

    // Обработка входящего сообщения
    static void ProcessMessage(String^ serverMessage) {
        try {
            Console::WriteLine("Получено сообщение: [" + serverMessage + "]");

            if (serverMessage->Contains("Вход успешен! TABLE") ) {
                OpenOrUpdateTable(serverMessage);
                return;
            }
            else if ( serverMessage->Trim()->StartsWith("Состояние стола:")) {
                OOpenOrUpdateTable(serverMessage);
                return;
            }
            else if (serverMessage->Contains("Ошибка")) {
                ShowError(serverMessage);
                HideAll();
            }
            else if (serverMessage->Contains("Регистрация успешна!") ||
                serverMessage->Contains("START:")) {
                ShowSuccess(serverMessage);
                array<String^>^ parts = serverMessage->Split(':');
                if (parts->Length > 1) {
                    GAmeState(parts[1]->Trim());
                }

            }
            else if (serverMessage->Contains("GAME_STATE:")) {
                array<String^>^ parts = serverMessage->Split(':');
                if (parts->Length > 1) {
                    GAmeState(parts[1]->Trim());
                }
            }
            else if (serverMessage->Contains("ВАШ ХОД:")) {
                Options(serverMessage);
            }
            else if (serverMessage->Contains("ACTION_EXPECTED_FROM:")) {
                int startPos = serverMessage->IndexOf("ACTION_EXPECTED_FROM:") + 21;
                int endPos = serverMessage->IndexOf(".", startPos);
                if (startPos != -1 && endPos != -1) {
                    String^ idString = serverMessage->Substring(startPos, endPos - startPos);
                    int playerID = Convert::ToInt32(idString->Trim());
                    HighlightPlayerPanel(playerID);
                }
            }
            else if (serverMessage->Contains("MOVE:")) {
                int startPos = serverMessage->IndexOf("MOVE:") + 5;
                String^ moveData = serverMessage->Substring(startPos)->Trim();
                MoveAction(moveData);
            }
            else if (serverMessage->Contains("CURRENT_BET:")) {
    
                int betStart = serverMessage->IndexOf("CURRENT_BET:") + 13;
                int potStart = serverMessage->IndexOf("POT:");

                String^ betValue = serverMessage->Substring(betStart, potStart - betStart)->Trim();
                String^ potValue = serverMessage->Substring(potStart + 5)->Trim();

                // Банк/Ставка
                String^ combinedMessage = potValue + " " + betValue;

                ShowPot(combinedMessage);
            }
            else if (serverMessage->StartsWith("HANDS_CARDS: ")) {
                ProcessHandsCards(serverMessage);
            }
            else if (serverMessage->StartsWith("WINNER: ")) {
                int startPos = serverMessage->IndexOf("WINNER:") + 7;
                String^ moveData = serverMessage->Substring(startPos)->Trim();
                MoveAction(moveData);
                ShowSuccess(serverMessage);
                System::Threading::Thread::Sleep(5000); // Ожидание 2 секунды

                HideAll();
            }
            else if (serverMessage->StartsWith("TABLE_CARDS: ")) {
                ProcessTableCards(serverMessage);
            }
            else {
                ShowGameEvent(serverMessage);
            }
        }
        catch (Exception^ ex) {
            Console::WriteLine("Ошибка в ProcessMessage: " + ex->Message);
        }
    }

private:

    static void ShowPot(String^ pot) {
        try {
            if (tableForm != nullptr && tableForm->IsHandleCreated) {
                tableForm->BeginInvoke(gcnew Action<String^>(tableForm, &Table::ShowPot),  pot);

            }
        }
        catch (Exception^ ex) {
            Console::WriteLine("Ошибка в MoveAction: " + ex->Message);
        }
    }

    static void HideAll() {
        try {
            if (tableForm != nullptr && tableForm->IsHandleCreated) {
                tableForm->BeginInvoke(gcnew Action(tableForm, &Table::HideAll));
            }
        }
        catch (Exception^ ex) {
            Console::WriteLine("Ошибка в HideAll: " + ex->Message);
        }
    }

    static void OnUpdateTimerTick(Object^ sender, EventArgs^ e) {
        // хай будзе но пока пусто ибо все обновления выполняются моментально
    }

    static void OOpenOrUpdateTable(String^ message) {
        try {

            // если окно создано, обновляем UI немедленно через BeginInvoke для асинхронности
            if (!String::IsNullOrEmpty(message) && tableForm != nullptr && tableForm->IsHandleCreated) {
                tableForm->BeginInvoke(gcnew Action<String^>(tableForm, &Table::UpdateTableState), message);
                tableForm->BeginInvoke(gcnew Action(tableForm, &Table::BringToFront));
            }
        }
        catch (Exception^ ex) {
            Console::WriteLine("Ошибка в OpenOrUpdateTable: " + ex->Message);
        }
    }

    static void CloseAllWindows() {
        try {
            for each (Form ^ openForm in Application::OpenForms) {
                if (openForm != tableForm) {  // Закрываем все, кроме стола
                    openForm->Close();
                }
            }
        }
        catch (Exception^ ex) {
            Console::WriteLine("Ошибка при закрытии окон: " + ex->Message);
        }
    }


    // непосредственное обновление окна стола
    static void OpenOrUpdateTable(String^ message) {
        try {
            // Если окно стола не открыто или закрыто, запускаем его
            if (tableForm == nullptr || tableForm->IsDisposed) {
                GlobalController::StopApp();

                CloseAllWindows();
                if (!isCreatingTable) {
                    isCreatingTable = true;
                    System::Threading::Thread^ uiThread = gcnew System::Threading::Thread(gcnew System::Threading::ThreadStart(&ServerMessageProcessor::RunTableForm));
                    uiThread->SetApartmentState(System::Threading::ApartmentState::STA);
                    uiThread->Start();
                    // Ждем появления окна, но коротко
                    int waitCount = 0;
                    while ((tableForm == nullptr || !tableForm->IsHandleCreated) && waitCount < 100) {
                        System::Threading::Thread::Sleep(5);
                        waitCount++;
                    }
                }
            }
  
        }
        catch (Exception^ ex) {
            Console::WriteLine("Ошибка в OpenOrUpdateTable: " + ex->Message);
        }
    }

    // запуск окна Table в отдельном потоке
    static void RunTableForm() {
        try {
            tableForm = gcnew Table();
            isCreatingTable = false;
            Console::WriteLine("Форма стола создана и запущена!");
            Application::Run(tableForm);
        }
        catch (Exception^ ex) {
            Console::WriteLine("Ошибка при запуске окна Table: " + ex->Message);
        }
    }

    // прямой вызов методов обновления UI для соответствующих сообщений
    static void GAmeState(String^ message) {
        try {
            if (tableForm != nullptr && tableForm->IsHandleCreated) {
                tableForm->BeginInvoke(gcnew Action<String^>(tableForm, &Table::EditStatus), message);
            }
        }
        catch (Exception^ ex) {
            Console::WriteLine("Ошибка в GAmeState: " + ex->Message);
        }
    }


    static void Options(String^ message) {
        try {
            if (tableForm != nullptr && tableForm->IsHandleCreated) {
                tableForm->BeginInvoke(gcnew Action<String^>(tableForm, &Table::Options), message);
            }
        }
        catch (Exception^ ex) {
            Console::WriteLine("Ошибка в Options: " + ex->Message);
        }
    }

    static void MoveAction(String^ message) {
        try {
            if (tableForm != nullptr && tableForm->IsHandleCreated) {
                tableForm->BeginInvoke(gcnew Action<String^>(tableForm, &Table::MoveAction), message);
            }
        }
        catch (Exception^ ex) {
            Console::WriteLine("Ошибка в MoveAction: " + ex->Message);
        }
    }

    static void HighlightPlayerPanel(int playerID) {
        try {
            if (tableForm != nullptr && tableForm->IsHandleCreated) {
                tableForm->BeginInvoke(gcnew Action<int>(tableForm, &Table::HighlightPlayerPanel), playerID);
            }
        }
        catch (Exception^ ex) {
            Console::WriteLine("Ошибка в HighlightPlayerPanel: " + ex->Message);
        }
    }

    static void ProcessHandsCards(String^ message) {
        try {
            array<String^>^ parts = message->Split(':');
            if (parts->Length > 1) {
                array<String^>^ cardNames = parts[1]->Trim()->Split(' ');
                List<String^>^ cardImages = gcnew List<String^>();
                for (int i = 0; i < cardNames->Length; i++) {
                    cardImages->Add("C:\\Users\\dasha\\Desktop\\BSUIR\\Poker\\x64\\Debug\\cards\\" + cardNames[i] + ".png");
                }
                if (tableForm != nullptr && !tableForm->IsDisposed) {
                    tableForm->BeginInvoke(gcnew Action<List<String^>^>(tableForm, &Table::UpdateCards), cardImages);
                    Console::WriteLine("Обновление карточек вызвано.");
                }
            }
            ShowGameEvent(message);
        }
        catch (Exception^ ex) {
            Console::WriteLine("Ошибка в ProcessHandsCards: " + ex->Message);
        }
    }

    static void ProcessTableCards(String^ message) {
        try {
            array<String^>^ parts = message->Split(':');
            if (parts->Length > 1) {
                List<String^>^ cardImages = gcnew List<String^>();
                for (int i = 1; i < parts->Length; i++) {
                    String^ cardName = parts[i]->Trim();
                    if (!String::IsNullOrEmpty(cardName)) {
                        cardImages->Add("C:\\Users\\dasha\\Desktop\\BSUIR\\Poker\\x64\\Debug\\cards\\" + cardName + ".png");
                    }
                }
                if (tableForm != nullptr && !tableForm->IsDisposed) {
                    tableForm->BeginInvoke(gcnew Action<List<String^>^>(tableForm, &Table::UpdateTableCards), cardImages);
                    Console::WriteLine("Количество карточек: " + cardImages->Count);
                }
            }
            ShowGameEvent(message);
        }
        catch (Exception^ ex) {
            Console::WriteLine("Ошибка в ProcessTableCards: " + ex->Message);
        }
    }

    static void ShowError(String^ message) {
        try {
            MessageBox::Show(message, "Ошибка", MessageBoxButtons::OK, MessageBoxIcon::Error);
        }
        catch (Exception^ ex) {
            Console::WriteLine("Ошибка в ShowError: " + ex->Message);
        }
    }

    static void ShowSuccess(String^ message) {
        try {
            MessageBox::Show(message, ":P", MessageBoxButtons::OK, MessageBoxIcon::Information);
        }
        catch (Exception^ ex) {
            Console::WriteLine("Ошибка в ShowSuccess: " + ex->Message);
        }
    }

    static void ShowGameEvent(String^ message) {
        try {
            Console::WriteLine("Событие игры: " + message);
        }
        catch (Exception^ ex) {
            Console::WriteLine("Ошибка в ShowGameEvent: " + ex->Message);
        }
    }
};

/*
ref class ServerMessageProcessor {
private:
    static Table^ tableForm = nullptr;
    static System::Windows::Forms::Timer^ updateTimer;
    static String^ pendingUpdateMessage = nullptr;
    static bool isCreatingTable = false;
public:
    static void Initialize() {
        updateTimer = gcnew System::Windows::Forms::Timer();
        updateTimer->Interval = 500; // Проверяем обновления каждые 500 мс
        updateTimer->Tick += gcnew System::EventHandler(&ServerMessageProcessor::OnUpdateTimerTick);
        updateTimer->Start();
    }

    static void ProcessMessage(String^ serverMessage) {
       Console::WriteLine("Получено сообщение: [" + serverMessage + "]");

        if (serverMessage->Contains("Ошибка")) {
            ShowError(serverMessage);
        }
        else if (serverMessage->Contains("Регистрация успешна!")) {
            ShowSuccess(serverMessage);
        }
        else if (serverMessage->Contains("START:")) {
            ShowSuccess(serverMessage);
        }
        else if (serverMessage->Contains("GAME_STATE:")) {
            array<String^>^ parts = serverMessage->Split(':');
            if (parts->Length > 1) {
                String^ gameStateText = parts[1]->Trim();  // Удаляем пробелы и получаем чистый текст
                GAmeState(gameStateText);  //  Передаём в обработчик
            }
        }
        else if (serverMessage->Contains("ВАШ ХОД:")) {///////////////
            Options(serverMessage);
        }
        else if (serverMessage->Contains("ACTION_EXPECTED_FROM:")) {
            int startPos = serverMessage->IndexOf("ACTION_EXPECTED_FROM:") + 21;
            int endPos = serverMessage->IndexOf(".", startPos);

            if (startPos != -1 && endPos != -1) {
                String^ idString = serverMessage->Substring(startPos, endPos - startPos);
                int playerID = Convert::ToInt32(idString->Trim());

                // Вызываем функцию выделения панели
                HighlightPlayerPanel(playerID);
            }
        }
        else if (serverMessage->Contains("MOVE:")) {///////////////
            int startPos = serverMessage->IndexOf("MOVE:") + 5;
            String^ moveData = serverMessage->Substring(startPos)->Trim();
            MoveAction(moveData);
        }
       
        else if (serverMessage->Contains("Вход успешен! TABLE")) {
            OpenOrUpdateTable(""); //  Открываем форму без обновления
        }
        else if (serverMessage->Trim()->StartsWith("Состояние стола:")) {
            OpenOrUpdateTable(serverMessage); //  Передаём сообщение для обновления
        }
        else if (serverMessage->StartsWith("HANDS_CARDS: ")) {
            array<String^>^ parts = serverMessage->Split(':');
            if (parts->Length > 1) {
                array<String^>^ cardNames = parts[1]->Trim()->Split(' ');
                List<String^>^ cardImages = gcnew List<String^>();

                for (int i = 0; i < cardNames->Length; i++) {
                    cardImages->Add("C:\\Users\\dasha\\Desktop\\BSUIR\\Poker\\x64\\Debug\\cards\\" + cardNames[i] + ".png"); // Формируем список карт
                }
                
                if (tableForm != nullptr && !tableForm->IsDisposed) {
                    tableForm->Invoke(gcnew Action<List<String^>^>(tableForm, &Table::UpdateCards), cardImages);
                    Console::WriteLine("пиоры");
                }
            }
            ShowGameEvent(serverMessage);
        }
        else if (serverMessage->StartsWith("TABLE_CARDS: ")) {
            array<String^>^ parts = serverMessage->Split(':');
            if (parts->Length > 1) {
                List<String^>^ cardImages = gcnew List<String^>();

                for (int i = 1; i < parts->Length; i++) {
                    String^ cardName = parts[i]->Trim();
                    if (!String::IsNullOrEmpty(cardName)) {
                        cardImages->Add("C:\\Users\\dasha\\Desktop\\BSUIR\\Poker\\x64\\Debug\\cards\\" + cardName + ".png");
                    }
                }

                if (tableForm != nullptr && !tableForm->IsDisposed) {
                    tableForm->Invoke(gcnew Action<List<String^>^>(tableForm, &Table::UpdateTableCards), cardImages);
                    Console::WriteLine("Количество карт: " + cardImages->Count);
                }
            }
            ShowGameEvent(serverMessage);
        }
        else {
            Console::WriteLine("Сообщение не опозано как команда");
            ShowGameEvent(serverMessage);
        }
    }


private:

    static void MoveActionHIDE() {
        if (tableForm != nullptr && tableForm->IsHandleCreated) {
            tableForm->Invoke(gcnew Action(tableForm, &Table::HideActionsButtons));
        }
    }

    static void MoveAction(String^ message) {
        if (tableForm != nullptr && tableForm->IsHandleCreated) {
            tableForm->Invoke(gcnew Action<String^>(tableForm, &Table::MoveAction), message);
        }
    }

    static void HighlightPlayerPanel(int playerID) {
        if (tableForm != nullptr && tableForm->IsHandleCreated) {
            tableForm->Invoke(gcnew Action<int>(tableForm, &Table::HighlightPlayerPanel), playerID);
        }
    }
    static void Options(String^ message) {
        if (tableForm != nullptr && tableForm->IsHandleCreated) {
            tableForm->Invoke(gcnew Action<String^>(tableForm, &Table::Options), message);
        }
    }

    static void GAmeState(String^ message) {
        if (tableForm != nullptr && tableForm->IsHandleCreated) {
            tableForm->Invoke(gcnew Action<String^>(tableForm, &Table::EditStatus), message);
        }
    }


    static void OpenOrUpdateTable(String^ message) {
        if (tableForm == nullptr || tableForm->IsDisposed) {
            if (!isCreatingTable) {
                isCreatingTable = true;
                System::Threading::Thread^ uiThread = gcnew System::Threading::Thread(
                    gcnew System::Threading::ThreadStart(&RunTableForm)
                );
                uiThread->SetApartmentState(System::Threading::ApartmentState::STA);
                uiThread->Start();
            }
            // Ожидаем, пока окно не будет создано и его хэндл не появится
            while (tableForm == nullptr || !tableForm->IsHandleCreated) {
                System::Threading::Thread::Sleep(10);
            }
        }

        // обновляем UI через Invoke в UI-потоке.
        if (!String::IsNullOrEmpty(message)) {
            tableForm->Invoke(gcnew Action<String^>(tableForm, &Table::UpdateTableState), message);
        }
    }

    // Метод, запускающий форму в отдельном потоке
    static void RunTableForm() {
        tableForm = gcnew Table();
        // Форма создана, можно сбросить флаг создания.
        isCreatingTable = false;
        Application::Run(tableForm);
    }

    // Вызывает немедленное обновление, если форма уже запущена
    static void TriggerImmediateUpdate() {
        if (tableForm != nullptr && tableForm->IsHandleCreated) {
            tableForm->Invoke(gcnew Action<String^>(tableForm, &Table::UpdateTableState), pendingUpdateMessage);
        }
    }


    static void OnUpdateTimerTick(System::Object^ sender, System::EventArgs^ e) {
        if (tableForm != nullptr && !tableForm->IsDisposed && pendingUpdateMessage != nullptr) {
            tableForm->UpdateTableState(pendingUpdateMessage);
            pendingUpdateMessage = nullptr; // Очистка после обновления
        }
    }


    static void ShowError(String^ message) {
        MessageBox::Show(message, "Ошибка", MessageBoxButtons::OK, MessageBoxIcon::Error);
    }

    static void ShowSuccess(String^ message) {
        MessageBox::Show(message, "Успех", MessageBoxButtons::OK, MessageBoxIcon::Information);
    }

    static void ShowStart(String^ message) {
        MessageBox::Show(message, "Игра началась!", MessageBoxButtons::OK, MessageBoxIcon::Information);
    }

    static void ShowGameEvent(String^ message) {
        Console::WriteLine("Событие игры: " + message);
    }
};
*/
//ref class ServerMessageProcessor {
//private:
//    static Table^ tableForm = nullptr; //  Сохраняем ссылку на форму
//
//public:
//    static void ProcessMessage(String^ serverMessage) {
//        if (serverMessage->Contains("Ошибка")) {
//            ShowError(serverMessage);
//        }
//        else if (serverMessage->Contains("Регистрация успешна!")) {
//            ShowSuccess(serverMessage);
//        }
//        else if (serverMessage->Contains("Вход успешен! TABLE")) {
//            OpenOrUpdateTable(serverMessage);
//        }
//        else if (serverMessage->StartsWith("Событие игры: Состояние стола")) {
//            OpenOrUpdateTable(serverMessage); // Если форма открыта, обновляем
//            ShowGameEvent(serverMessage);
//        }
//        else {
//            ShowGameEvent(serverMessage);
//        }
//    }
//
//private:
//    static void OpenOrUpdateTable(String^ message) {
//        if (tableForm == nullptr || tableForm->IsDisposed) {
//            tableForm = gcnew Table();
//
//            // Открываем форму в главном UI-потоке
//            tableForm->Invoke(gcnew MethodInvoker(tableForm, &Table::Show));
//
//            // Обновляем данные после открытия формы
//            tableForm->BeginInvoke(gcnew Action<String^>(tableForm, &Table::UpdateTableState), message);
//        }
//        else {
//            tableForm->BeginInvoke(gcnew Action<String^>(tableForm, &Table::UpdateTableState), message); // Обновляем существующую форму
//        }
//    }
//
//
//
//
//    static void ShowError(String^ message) {
//        MessageBox::Show(message, "Ошибка", MessageBoxButtons::OK, MessageBoxIcon::Error);
//    }
//
//    static void ShowSuccess(String^ message) {
//        MessageBox::Show(message, "Успех", MessageBoxButtons::OK, MessageBoxIcon::Information);
//    }
//
//    static void ShowGameEvent(String^ message) {
//        Console::WriteLine("Событие игры: " + message);
//    }
//};
