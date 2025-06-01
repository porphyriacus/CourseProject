#pragma once
#include "GameNetwork.h"
namespace Game {
	using namespace System::Windows::Forms;
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Collections::Generic;
	using namespace System::Timers;
	/// <summary>
	/// Summary for Table
	/// </summary>
	public ref class Table : public System::Windows::Forms::Form
	{
	public:
		Table(void)
		{
			//InitializeComponent();
			this->InitializeComponent();
			InitializePlayerUI();
			card2->Visible = false;
			card1->Visible = false;
			CurrentBetLabel->Visible = false;
			PotLabel->Visible = false;

			HideActionsButtons();
			
		}
	public:
		void HideAll() {
			card2->Visible = false;
			card1->Visible = false;

			tableCard1->Visible = false;
			tableCard2->Visible = false;
			tableCard3->Visible = false;
			tableCard4->Visible = false;
			tableCard5->Visible = false;

			fold->Visible = false;
			call->Visible = false;
			check->Visible = false;
			raise->Visible = false;

			trackBarRaise->Visible = false;

			panel1->BorderStyle = BorderStyle::None;
			panel2->BorderStyle = BorderStyle::None;
			panel3->BorderStyle = BorderStyle::None;
			panel4->BorderStyle = BorderStyle::None;
			panel5->BorderStyle = BorderStyle::None;

			labelMove->Visible = false;
			pictureBoxMove->Visible = false;

			CurrentBetLabel->Visible = false;
			PotLabel->Visible = false;
		}

		void HideActionsButtons() {
			fold->Visible = false;
			call->Visible = false;
			check->Visible = false;
			raise->Visible = false;

			trackBarRaise->Visible = false;

			panel1->BorderStyle = BorderStyle::None;
			panel2->BorderStyle = BorderStyle::None;
			panel3->BorderStyle = BorderStyle::None;
			panel4->BorderStyle = BorderStyle::None;
			panel5->BorderStyle = BorderStyle::None;

			labelMove->Visible = false;
			pictureBoxMove->Visible = false;
		}

		void ShowPot(String^ message) {
			array<String^>^ parts = message->Split(' ');

			if (parts->Length == 2) {
				// Убираем "CURRENT_BET:" и "POT:", оставляя только значения
				String^ pot = "Банк: " + parts[0]->Replace("POT:", "")->Trim();
				String^ bet = "Текущая ставка: " + parts[1]->Replace("CURRENT_BET:", "")->Replace(".", "")->Replace("/", "")->Trim();

				CurrentBetLabel->Text = bet;
				PotLabel->Text = pot;

				CurrentBetLabel->Visible = true;
				PotLabel->Visible = true;
			}

			else {
				Console::WriteLine("Ошибка: сообщение не соответствует ожидаемому формату (pot/bet)");
			}
		}
	protected:
		~Table()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::PictureBox^ tableCard3;
	private: System::Windows::Forms::PictureBox^ tableCard2;
	private: System::Windows::Forms::PictureBox^ tableCard1;
	private: System::Windows::Forms::PictureBox^ card2;
	private: System::Windows::Forms::PictureBox^ card1;
	private: System::Windows::Forms::PictureBox^ tableCard5;
	private: System::Windows::Forms::Button^ fold;
	private: System::Windows::Forms::Button^ check;
	private: System::Windows::Forms::Button^ call;
	private: System::Windows::Forms::Button^ raise;
	private: System::Windows::Forms::TrackBar^ trackBarRaise;
	private: System::Windows::Forms::Label^ GameStatus;
	private: System::Windows::Forms::Label^ labelRaise;
	private: System::Windows::Forms::PictureBox^ pictureBoxMove;
	private: System::Windows::Forms::Label^ labelMove;
	private: System::Windows::Forms::Label^ CurrentBetLabel;
	private: System::Windows::Forms::Label^ PotLabel;


	private: System::Windows::Forms::PictureBox^ tableCard4;

	private:
		ref struct PlayerUI {
			System::Windows::Forms::Panel^ panel;
			System::Windows::Forms::Label^ labelName;
			System::Windows::Forms::Label^ labelBalance;
			System::Windows::Forms::Label^ labelStatus;
		};
	
	private: System::Void RemoveHighlight(System::Object^ sender, System::EventArgs^ e) {
		System::Windows::Forms::Timer^ timer = dynamic_cast<System::Windows::Forms::Timer^>(sender);
		if (timer != nullptr) {
			timer->Stop(); // oстанавливаем таймер
			delete timer;  // yдаляем объект через деструктор
		}

		panel1->BorderStyle = BorderStyle::None;
		panel2->BorderStyle = BorderStyle::None;
		panel3->BorderStyle = BorderStyle::None;
		panel4->BorderStyle = BorderStyle::None;
		panel5->BorderStyle = BorderStyle::None;
	}


	public: void HighlightPlayerPanel(int playerID) {
		Panel^ targetPanel = nullptr;

		if (playerID == 1) targetPanel = panel1;
		else if (playerID == 2) targetPanel = panel2;
		else if (playerID == 3) targetPanel = panel3;
		else if (playerID == 4) targetPanel = panel4;
		else if (playerID == 5) targetPanel = panel5;


		if (targetPanel != nullptr) {
			targetPanel->BorderStyle = BorderStyle::FixedSingle; // Обводка панели
			System::Windows::Forms::Timer^ timer = gcnew System::Windows::Forms::Timer();
			timer->Interval = 30000; // 30 секунд
			timer->Tick += gcnew EventHandler(this, &Table::RemoveHighlight);
			timer->Start();
		}
	}
	public: void MoveAction(String^ message) {
		
		labelMove->Text = message;
		
		pictureBoxMove->Visible = true;
		labelMove->Visible = true;

		panel1->BorderStyle = BorderStyle::None;
		panel2->BorderStyle = BorderStyle::None;
		panel3->BorderStyle = BorderStyle::None;
		panel4->BorderStyle = BorderStyle::None;
		panel5->BorderStyle = BorderStyle::None;
		
	}

		public: void UpdateTableCards(List<String^>^ cardImages) {
			
			tableCard1->Visible = false;
			tableCard2->Visible = false;
			tableCard3->Visible = false;
			tableCard4->Visible = false;
			tableCard5->Visible = false;

			// Проверяем размер массива и загружаем доступные карты
			int numCards = cardImages->Count;

			if (numCards > 0) {
				tableCard1->Image = Image::FromFile(cardImages[0]);
				tableCard1->Visible = true;
			}
			if (numCards > 1) {
				tableCard2->Image = Image::FromFile(cardImages[1]);
				tableCard2->Visible = true;
			}
			if (numCards > 2) {
				tableCard3->Image = Image::FromFile(cardImages[2]);
				tableCard3->Visible = true;
			}
			if (numCards > 3) {
				tableCard4->Image = Image::FromFile(cardImages[3]);
				tableCard4->Visible = true;
			}
			if (numCards > 4) {
				tableCard5->Image = Image::FromFile(cardImages[4]);
				tableCard5->Visible = true;
			}

			Console::WriteLine("Количество карт на столе: " + numCards);
			HideActionsButtons();
		}



			  void FlipCard(PictureBox^ card, String^ frontImage) {
				  try {
					  // Исходная ширина карты
					  int originalWidth = card->Width;

					  // Плавно уменьшаем ширину (имитация переворота)
					  for (int i = originalWidth; i > 0; i -= 5) {
						  card->Width = i;
						  System::Threading::Thread::Sleep(20);
						  card->Refresh(); // Принудительно обновляем UI
					  }

					  // Меняем изображение карты
					  card->Image = Image::FromFile(frontImage);
					  card->Refresh();

					  // Восстанавливаем ширину карты
					  for (int i = 0; i <= originalWidth; i += 5) {
						  card->Width = i;
						  System::Threading::Thread::Sleep(20);
						  card->Refresh();
					  }

					  Console::WriteLine("Переворот завершен: " + frontImage);
				  }
				  catch (Exception^ ex) {
					  Console::WriteLine("Ошибка переворота карты: " + ex->Message);
				  }
			  }

		public: void UpdateCards(List<String^>^ cardImages) {
			// Загружаем рубашку карты
			String^ cardBack = "C:\\Users\\dasha\\Desktop\\BSUIR\\Poker\\x64\\Debug\\cards\\back.png";

			card1->Image = Image::FromFile(cardBack);
			card2->Image = Image::FromFile(cardBack);
			card2->Visible = true;
			card1->Visible = true;

			Console::WriteLine("Карты загружены с рубашкой.");

			// Задержка перед переворотом
			System::Threading::Thread::Sleep(200);

			// Запускаем анимацию переворота
			FlipCard(card1, cardImages[0]);
			FlipCard(card2, cardImages[1]);

			HideActionsButtons();
		}


		/*	public: void UpdateCards(List<String^>^ cardImages) {
				card1->Image = Image::FromFile(cardImages[0]);
				card2->Image = Image::FromFile(cardImages[1]);
				card2->Visible = true;
				card1->Visible = true;
				Console::WriteLine("уебище ");
				HideActionsButtons();
			}*/

	
		private: System::Void trackBarRaise_ValueChanged(System::Object^ sender, System::EventArgs^ e) {
			labelRaise->Text = "Ставка: " + trackBarRaise->Value.ToString();
		}

		public: void Options(String^ message) {
			HideActionsButtons();
			if (message->Contains("CHECK")) check->Visible = true;
			if (message->Contains("CALL")) call->Visible = true;
			if (message->Contains("FOLD")) fold->Visible = true;

			if (message->Contains("RAISE")) {
				raise->Visible = true;

				//  <a, b> 
				int startPos = message->IndexOf("RAISE <") + 7; // cмещение на "RAISE <"
				int endPos = message->IndexOf(">", startPos);

				if (startPos != -1 && endPos != -1) {
					String^ range = message->Substring(startPos, endPos - startPos);
					array<String^>^ values = range->Split(',');
					if (values->Length == 2) {
						int minRaise = Convert::ToInt32(values[0]->Trim()); // a
						int maxRaise = Convert::ToInt32(values[1]->Trim()); // b

						if (minRaise < maxRaise) {
							trackBarRaise->Minimum = minRaise;
							trackBarRaise->Maximum = maxRaise;
						}
						else {
							int minRaise = 20;
							trackBarRaise->Minimum = minRaise;
							trackBarRaise->Maximum = 30;
						}
						
						trackBarRaise->Value = minRaise; 

						// шаг изменения
						trackBarRaise->LargeChange = (maxRaise - minRaise) / 4;
						trackBarRaise->SmallChange = 1;

						trackBarRaise->TickStyle = TickStyle::None;

						trackBarRaise->Visible = false;

						trackBarRaise->Scroll += gcnew EventHandler(this, &Table::trackBarRaise_ValueChanged);
					}
				}
			}
		}

		public: void EditStatus(String^ status) {
			HideActionsButtons();
			GameStatus->Text = status;
		}

		
		// массив для пяти игроков
		array<PlayerUI^>^ playerUIs;

		
		void InitializePlayerUI() {
			playerUIs = gcnew array<PlayerUI^>(5);

			// панель 1: для первого игрока — используем panel1, labelName1, labelBalanse1, labelStatus1
			playerUIs[0] = gcnew PlayerUI();
			playerUIs[0]->panel = this->panel1;
			playerUIs[0]->labelName = this->labelName1;
			playerUIs[0]->labelBalance = this->labelBalanse1;
			playerUIs[0]->labelStatus = this->labelStatus1;

			// панель 2: для второго игрока
			playerUIs[1] = gcnew PlayerUI();
			playerUIs[1]->panel = this->panel2;
			playerUIs[1]->labelName = this->labelName2;
			playerUIs[1]->labelBalance = this->labelBalanse2;
			playerUIs[1]->labelStatus = this->labelStatus2;

			// 3
			playerUIs[2] = gcnew PlayerUI();
			playerUIs[2]->panel = this->panel3;
			playerUIs[2]->labelName = this->labelName3;
			playerUIs[2]->labelBalance = this->labelBalanse3;
			playerUIs[2]->labelStatus = this->labelStatus4;

			// 4
			playerUIs[3] = gcnew PlayerUI();
			playerUIs[3]->panel = this->panel4;
			playerUIs[3]->labelName = this->labelName4;
			playerUIs[3]->labelBalance = this->labelBalanse4;
			playerUIs[3]->labelStatus = this->labelStatus3;

			// 5
			// labelBalanse5 используется для имени, label11 для баланса, а labelStatus5 для статуса лень менять 
			playerUIs[4] = gcnew PlayerUI();
			playerUIs[4]->panel = this->panel5;
			playerUIs[4]->labelName = this->labelBalanse5;   // имя
			playerUIs[4]->labelBalance = this->label11;        // баланс
			playerUIs[4]->labelStatus = this->labelStatus5;      // статус

			// скрываем все панели по умолчанию
			for (int i = 0; i < playerUIs->Length; i++) {
				playerUIs[i]->panel->Visible = false;
			}
		}

		
		// index – порядковый номер (0...4), playerName, balance, playerStatus ета данные игрока
		public: void UpdatePlayerUI(int index, String^ playerName, int balance, String^ playerStatus) {
			if (index < 0 || index >= playerUIs->Length)
				return;

			// заполняе соответствующие метки
			playerUIs[index]->labelName->Text = playerName;
			playerUIs[index]->labelBalance->Text = "$" + balance.ToString();
			playerUIs[index]->labelStatus->Text = playerStatus;
			// делае видимой панель игрока
			playerUIs[index]->panel->Visible = true;
		}

		void HideAllPlayerPanels() {
			for (int i = 0; i < playerUIs->Length; i++) {
				playerUIs[i]->panel->Visible = false;
			}
		}


	private: System::Windows::Forms::Panel^ panel;
	protected:

	private: System::Windows::Forms::Button^ cancel;
	private: System::Windows::Forms::PictureBox^ pictureBox;


	private: System::Drawing::Size originalSize;
	private: System::Windows::Forms::Panel^ panel1;

	private: System::Windows::Forms::PictureBox^ pictureBox1;
	private: System::Windows::Forms::Label^ labelStatus1;
	private: System::Windows::Forms::Label^ labelBalanse1;
	private: System::Windows::Forms::Label^ labelName1;
	private: System::Windows::Forms::Panel^ panel5;
	private: System::Windows::Forms::Label^ labelStatus5;
	private: System::Windows::Forms::Label^ label11;
	private: System::Windows::Forms::Label^ labelBalanse5;
	private: System::Windows::Forms::PictureBox^ pictureBox5;
	private: System::Windows::Forms::Panel^ panel4;
	private: System::Windows::Forms::Label^ labelStatus3;
	private: System::Windows::Forms::Label^ labelBalanse4;
	private: System::Windows::Forms::Label^ labelName4;
	private: System::Windows::Forms::PictureBox^ pictureBox4;
	private: System::Windows::Forms::Panel^ panel3;
	private: System::Windows::Forms::Label^ labelStatus4;
	private: System::Windows::Forms::Label^ labelBalanse3;
	private: System::Windows::Forms::Label^ labelName3;
	private: System::Windows::Forms::PictureBox^ pictureBox3;
	private: System::Windows::Forms::Panel^ panel2;
	private: System::Windows::Forms::Label^ labelStatus2;
	private: System::Windows::Forms::Label^ labelBalanse2;
	private: System::Windows::Forms::Label^ labelName2;
	private: System::Windows::Forms::PictureBox^ pictureBox2;

	public:
		// отображение данных игроков на столе
		void UpdateTableState(String^ serverMessage)
		{
			// Убираем префикс
			String^ prefix = "Состояние стола:";
			if (serverMessage->StartsWith(prefix))
			{
				serverMessage = serverMessage->Substring(prefix->Length);
			}

			// После префикса данные игроков будут разделены символом '|'
			array<String^>^ segments = serverMessage->Split(gcnew array<wchar_t>{'|'}, StringSplitOptions::RemoveEmptyEntries);

			// Сначала скрываем все панели (если меньше игроков, чем максимум)
			HideAllPlayerPanels();

			for each (String ^ segment in segments)
			{
				// пустые сегменты скип
				if (String::IsNullOrWhiteSpace(segment))
					continue;

				// "Игрок ID 1: 44, Баланс:10000, Статус: Активен"
				// по символу ',' 
				array<String^>^ parts = segment->Split(gcnew array<wchar_t>{','}, StringSplitOptions::RemoveEmptyEntries);
				if (parts->Length < 3)
					continue; // Если сегмент неполный, пропускаем его

				// 1 элемент: "Игрок ID 1: 44"
				array<String^>^ firstColonSplit = parts[0]->Split(gcnew array<wchar_t>{':'}, StringSplitOptions::RemoveEmptyEntries);
				if (firstColonSplit->Length < 2)
					continue;
				// firstColonSplit[0] = "Игрок ID 1", firstColonSplit[1] = " 44"
				// ID 
				array<String^>^ tokens = firstColonSplit[0]->Split(' ');
				int id = 0;
				if (!Int32::TryParse(tokens[tokens->Length - 1], id))
					continue;

				// имя из первой части после двоеточия:
				String^ playerName = firstColonSplit[1]->Trim();

				// " Баланс:10000"
				array<String^>^ balanceSplit = parts[1]->Split(gcnew array<wchar_t>{':'}, StringSplitOptions::RemoveEmptyEntries);
				int balance = 0;
				if (balanceSplit->Length >= 2)
				{
					Int32::TryParse(balanceSplit[1]->Trim(), balance);
				}

				//" Статус: Активен"
				array<String^>^ statusSplit = parts[2]->Split(gcnew array<wchar_t>{':'}, StringSplitOptions::RemoveEmptyEntries);
				String^ status = "";
				if (statusSplit->Length >= 2)
				{
					status = statusSplit[1]->Trim();
				}

				// номер панели как id - 1 (если id начинается с 1)
				int panelIndex = id - 1;
				// обновляе инфо на панели
				if (panelIndex >= 0 && panelIndex < playerUIs->Length)
				{
					UpdatePlayerUI(panelIndex, playerName, balance, status);
				}
			}

			// обновляет форму 
			this->Refresh();
		}
private: System::ComponentModel::IContainer^ components;

	private:


#pragma region Windows Form Designer generated code
		void InitializeComponent(void)
		{
			System::ComponentModel::ComponentResourceManager^ resources = (gcnew System::ComponentModel::ComponentResourceManager(Table::typeid));
			this->panel = (gcnew System::Windows::Forms::Panel());
			this->labelMove = (gcnew System::Windows::Forms::Label());
			this->tableCard5 = (gcnew System::Windows::Forms::PictureBox());
			this->tableCard4 = (gcnew System::Windows::Forms::PictureBox());
			this->tableCard3 = (gcnew System::Windows::Forms::PictureBox());
			this->tableCard2 = (gcnew System::Windows::Forms::PictureBox());
			this->tableCard1 = (gcnew System::Windows::Forms::PictureBox());
			this->panel5 = (gcnew System::Windows::Forms::Panel());
			this->labelStatus5 = (gcnew System::Windows::Forms::Label());
			this->label11 = (gcnew System::Windows::Forms::Label());
			this->labelBalanse5 = (gcnew System::Windows::Forms::Label());
			this->pictureBox5 = (gcnew System::Windows::Forms::PictureBox());
			this->panel4 = (gcnew System::Windows::Forms::Panel());
			this->labelStatus3 = (gcnew System::Windows::Forms::Label());
			this->labelBalanse4 = (gcnew System::Windows::Forms::Label());
			this->labelName4 = (gcnew System::Windows::Forms::Label());
			this->pictureBox4 = (gcnew System::Windows::Forms::PictureBox());
			this->panel2 = (gcnew System::Windows::Forms::Panel());
			this->labelStatus2 = (gcnew System::Windows::Forms::Label());
			this->labelBalanse2 = (gcnew System::Windows::Forms::Label());
			this->labelName2 = (gcnew System::Windows::Forms::Label());
			this->pictureBox2 = (gcnew System::Windows::Forms::PictureBox());
			this->panel1 = (gcnew System::Windows::Forms::Panel());
			this->labelStatus1 = (gcnew System::Windows::Forms::Label());
			this->labelBalanse1 = (gcnew System::Windows::Forms::Label());
			this->labelName1 = (gcnew System::Windows::Forms::Label());
			this->pictureBox1 = (gcnew System::Windows::Forms::PictureBox());
			this->panel3 = (gcnew System::Windows::Forms::Panel());
			this->labelStatus4 = (gcnew System::Windows::Forms::Label());
			this->labelBalanse3 = (gcnew System::Windows::Forms::Label());
			this->labelName3 = (gcnew System::Windows::Forms::Label());
			this->pictureBox3 = (gcnew System::Windows::Forms::PictureBox());
			this->pictureBox = (gcnew System::Windows::Forms::PictureBox());
			this->pictureBoxMove = (gcnew System::Windows::Forms::PictureBox());
			this->card2 = (gcnew System::Windows::Forms::PictureBox());
			this->card1 = (gcnew System::Windows::Forms::PictureBox());
			this->cancel = (gcnew System::Windows::Forms::Button());
			this->fold = (gcnew System::Windows::Forms::Button());
			this->check = (gcnew System::Windows::Forms::Button());
			this->call = (gcnew System::Windows::Forms::Button());
			this->raise = (gcnew System::Windows::Forms::Button());
			this->trackBarRaise = (gcnew System::Windows::Forms::TrackBar());
			this->GameStatus = (gcnew System::Windows::Forms::Label());
			this->labelRaise = (gcnew System::Windows::Forms::Label());
			this->PotLabel = (gcnew System::Windows::Forms::Label());
			this->CurrentBetLabel = (gcnew System::Windows::Forms::Label());
			this->panel->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->tableCard5))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->tableCard4))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->tableCard3))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->tableCard2))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->tableCard1))->BeginInit();
			this->panel5->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox5))->BeginInit();
			this->panel4->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox4))->BeginInit();
			this->panel2->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox2))->BeginInit();
			this->panel1->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->BeginInit();
			this->panel3->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox3))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBoxMove))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->card2))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->card1))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackBarRaise))->BeginInit();
			this->SuspendLayout();
			// 
			// panel
			// 
			this->panel->BackColor = System::Drawing::Color::Transparent;
			this->panel->BackgroundImage = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"panel.BackgroundImage")));
			this->panel->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Zoom;
			this->panel->Controls->Add(this->CurrentBetLabel);
			this->panel->Controls->Add(this->PotLabel);
			this->panel->Controls->Add(this->labelMove);
			this->panel->Controls->Add(this->tableCard5);
			this->panel->Controls->Add(this->tableCard4);
			this->panel->Controls->Add(this->tableCard3);
			this->panel->Controls->Add(this->tableCard2);
			this->panel->Controls->Add(this->tableCard1);
			this->panel->Controls->Add(this->panel5);
			this->panel->Controls->Add(this->panel4);
			this->panel->Controls->Add(this->panel2);
			this->panel->Controls->Add(this->panel1);
			this->panel->Controls->Add(this->panel3);
			this->panel->Controls->Add(this->pictureBox);
			this->panel->Controls->Add(this->pictureBoxMove);
			this->panel->Location = System::Drawing::Point(111, 1);
			this->panel->Margin = System::Windows::Forms::Padding(3, 2, 3, 2);
			this->panel->Name = L"panel";
			this->panel->Size = System::Drawing::Size(1038, 676);
			this->panel->TabIndex = 0;
			// 
			// labelMove
			// 
			this->labelMove->AutoSize = true;
			this->labelMove->BackColor = System::Drawing::Color::White;
			this->labelMove->Font = (gcnew System::Drawing::Font(L"Times New Roman", 10.8F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->labelMove->ForeColor = System::Drawing::SystemColors::ActiveCaptionText;
			this->labelMove->Location = System::Drawing::Point(612, 21);
			this->labelMove->Name = L"labelMove";
			this->labelMove->Size = System::Drawing::Size(0, 20);
			this->labelMove->TabIndex = 16;
			// 
			// tableCard5
			// 
			this->tableCard5->BackColor = System::Drawing::Color::Transparent;
			this->tableCard5->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Zoom;
			this->tableCard5->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"tableCard5.Image")));
			this->tableCard5->Location = System::Drawing::Point(676, 255);
			this->tableCard5->Name = L"tableCard5";
			this->tableCard5->Size = System::Drawing::Size(102, 147);
			this->tableCard5->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			this->tableCard5->TabIndex = 14;
			this->tableCard5->TabStop = false;
			// 
			// tableCard4
			// 
			this->tableCard4->BackColor = System::Drawing::Color::Transparent;
			this->tableCard4->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Zoom;
			this->tableCard4->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"tableCard4.Image")));
			this->tableCard4->Location = System::Drawing::Point(568, 255);
			this->tableCard4->Name = L"tableCard4";
			this->tableCard4->Size = System::Drawing::Size(102, 147);
			this->tableCard4->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			this->tableCard4->TabIndex = 13;
			this->tableCard4->TabStop = false;
			// 
			// tableCard3
			// 
			this->tableCard3->BackColor = System::Drawing::Color::Transparent;
			this->tableCard3->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Zoom;
			this->tableCard3->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"tableCard3.Image")));
			this->tableCard3->Location = System::Drawing::Point(460, 255);
			this->tableCard3->Name = L"tableCard3";
			this->tableCard3->Size = System::Drawing::Size(102, 147);
			this->tableCard3->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			this->tableCard3->TabIndex = 12;
			this->tableCard3->TabStop = false;
			// 
			// tableCard2
			// 
			this->tableCard2->BackColor = System::Drawing::Color::Transparent;
			this->tableCard2->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Zoom;
			this->tableCard2->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"tableCard2.Image")));
			this->tableCard2->Location = System::Drawing::Point(352, 255);
			this->tableCard2->Name = L"tableCard2";
			this->tableCard2->Size = System::Drawing::Size(102, 147);
			this->tableCard2->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			this->tableCard2->TabIndex = 11;
			this->tableCard2->TabStop = false;
			// 
			// tableCard1
			// 
			this->tableCard1->BackColor = System::Drawing::Color::Transparent;
			this->tableCard1->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Zoom;
			this->tableCard1->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"tableCard1.Image")));
			this->tableCard1->Location = System::Drawing::Point(244, 255);
			this->tableCard1->Name = L"tableCard1";
			this->tableCard1->Size = System::Drawing::Size(102, 147);
			this->tableCard1->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			this->tableCard1->TabIndex = 10;
			this->tableCard1->TabStop = false;
			// 
			// panel5
			// 
			this->panel5->Controls->Add(this->labelStatus5);
			this->panel5->Controls->Add(this->label11);
			this->panel5->Controls->Add(this->labelBalanse5);
			this->panel5->Controls->Add(this->pictureBox5);
			this->panel5->Location = System::Drawing::Point(86, 52);
			this->panel5->Name = L"panel5";
			this->panel5->Size = System::Drawing::Size(209, 198);
			this->panel5->TabIndex = 6;
			// 
			// labelStatus5
			// 
			this->labelStatus5->AutoSize = true;
			this->labelStatus5->ForeColor = System::Drawing::SystemColors::ButtonFace;
			this->labelStatus5->Location = System::Drawing::Point(4, 179);
			this->labelStatus5->Name = L"labelStatus5";
			this->labelStatus5->Size = System::Drawing::Size(11, 16);
			this->labelStatus5->TabIndex = 3;
			this->labelStatus5->Text = L"-";
			// 
			// label11
			// 
			this->label11->AutoSize = true;
			this->label11->ForeColor = System::Drawing::SystemColors::ButtonFace;
			this->label11->Location = System::Drawing::Point(4, 163);
			this->label11->Name = L"label11";
			this->label11->Size = System::Drawing::Size(21, 16);
			this->label11->TabIndex = 2;
			this->label11->Text = L"$0";
			// 
			// labelBalanse5
			// 
			this->labelBalanse5->AutoSize = true;
			this->labelBalanse5->Font = (gcnew System::Drawing::Font(L"Times New Roman", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->labelBalanse5->ForeColor = System::Drawing::SystemColors::ButtonHighlight;
			this->labelBalanse5->Location = System::Drawing::Point(3, 140);
			this->labelBalanse5->Name = L"labelBalanse5";
			this->labelBalanse5->Size = System::Drawing::Size(65, 23);
			this->labelBalanse5->TabIndex = 1;
			this->labelBalanse5->Text = L"Player";
			// 
			// pictureBox5
			// 
			this->pictureBox5->ErrorImage = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox5.ErrorImage")));
			this->pictureBox5->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox5.Image")));
			this->pictureBox5->Location = System::Drawing::Point(6, 3);
			this->pictureBox5->Name = L"pictureBox5";
			this->pictureBox5->Size = System::Drawing::Size(203, 134);
			this->pictureBox5->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			this->pictureBox5->TabIndex = 0;
			this->pictureBox5->TabStop = false;
			// 
			// panel4
			// 
			this->panel4->Controls->Add(this->labelStatus3);
			this->panel4->Controls->Add(this->labelBalanse4);
			this->panel4->Controls->Add(this->labelName4);
			this->panel4->Controls->Add(this->pictureBox4);
			this->panel4->Location = System::Drawing::Point(787, 337);
			this->panel4->Name = L"panel4";
			this->panel4->Size = System::Drawing::Size(209, 198);
			this->panel4->TabIndex = 5;
			// 
			// labelStatus3
			// 
			this->labelStatus3->AutoSize = true;
			this->labelStatus3->ForeColor = System::Drawing::SystemColors::ButtonFace;
			this->labelStatus3->Location = System::Drawing::Point(4, 179);
			this->labelStatus3->Name = L"labelStatus3";
			this->labelStatus3->Size = System::Drawing::Size(11, 16);
			this->labelStatus3->TabIndex = 3;
			this->labelStatus3->Text = L"-";
			// 
			// labelBalanse4
			// 
			this->labelBalanse4->AutoSize = true;
			this->labelBalanse4->ForeColor = System::Drawing::SystemColors::ButtonFace;
			this->labelBalanse4->Location = System::Drawing::Point(4, 163);
			this->labelBalanse4->Name = L"labelBalanse4";
			this->labelBalanse4->Size = System::Drawing::Size(21, 16);
			this->labelBalanse4->TabIndex = 2;
			this->labelBalanse4->Text = L"$0";
			// 
			// labelName4
			// 
			this->labelName4->AutoSize = true;
			this->labelName4->Font = (gcnew System::Drawing::Font(L"Times New Roman", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->labelName4->ForeColor = System::Drawing::SystemColors::ButtonHighlight;
			this->labelName4->Location = System::Drawing::Point(3, 140);
			this->labelName4->Name = L"labelName4";
			this->labelName4->Size = System::Drawing::Size(65, 23);
			this->labelName4->TabIndex = 1;
			this->labelName4->Text = L"Player";
			// 
			// pictureBox4
			// 
			this->pictureBox4->ErrorImage = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox4.ErrorImage")));
			this->pictureBox4->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox4.Image")));
			this->pictureBox4->Location = System::Drawing::Point(6, 3);
			this->pictureBox4->Name = L"pictureBox4";
			this->pictureBox4->Size = System::Drawing::Size(203, 134);
			this->pictureBox4->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			this->pictureBox4->TabIndex = 0;
			this->pictureBox4->TabStop = false;
			// 
			// panel2
			// 
			this->panel2->Controls->Add(this->labelStatus2);
			this->panel2->Controls->Add(this->labelBalanse2);
			this->panel2->Controls->Add(this->labelName2);
			this->panel2->Controls->Add(this->pictureBox2);
			this->panel2->Location = System::Drawing::Point(29, 334);
			this->panel2->Name = L"panel2";
			this->panel2->Size = System::Drawing::Size(209, 198);
			this->panel2->TabIndex = 3;
			// 
			// labelStatus2
			// 
			this->labelStatus2->AutoSize = true;
			this->labelStatus2->ForeColor = System::Drawing::SystemColors::ButtonFace;
			this->labelStatus2->Location = System::Drawing::Point(4, 179);
			this->labelStatus2->Name = L"labelStatus2";
			this->labelStatus2->Size = System::Drawing::Size(11, 16);
			this->labelStatus2->TabIndex = 3;
			this->labelStatus2->Text = L"-";
			// 
			// labelBalanse2
			// 
			this->labelBalanse2->AutoSize = true;
			this->labelBalanse2->ForeColor = System::Drawing::SystemColors::ButtonFace;
			this->labelBalanse2->Location = System::Drawing::Point(4, 163);
			this->labelBalanse2->Name = L"labelBalanse2";
			this->labelBalanse2->Size = System::Drawing::Size(21, 16);
			this->labelBalanse2->TabIndex = 2;
			this->labelBalanse2->Text = L"$0";
			// 
			// labelName2
			// 
			this->labelName2->AutoSize = true;
			this->labelName2->Font = (gcnew System::Drawing::Font(L"Times New Roman", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->labelName2->ForeColor = System::Drawing::SystemColors::ButtonHighlight;
			this->labelName2->Location = System::Drawing::Point(3, 140);
			this->labelName2->Name = L"labelName2";
			this->labelName2->Size = System::Drawing::Size(65, 23);
			this->labelName2->TabIndex = 1;
			this->labelName2->Text = L"Player";
			// 
			// pictureBox2
			// 
			this->pictureBox2->ErrorImage = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox2.ErrorImage")));
			this->pictureBox2->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox2.Image")));
			this->pictureBox2->Location = System::Drawing::Point(6, 3);
			this->pictureBox2->Name = L"pictureBox2";
			this->pictureBox2->Size = System::Drawing::Size(203, 134);
			this->pictureBox2->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			this->pictureBox2->TabIndex = 0;
			this->pictureBox2->TabStop = false;
			// 
			// panel1
			// 
			this->panel1->Controls->Add(this->labelStatus1);
			this->panel1->Controls->Add(this->labelBalanse1);
			this->panel1->Controls->Add(this->labelName1);
			this->panel1->Controls->Add(this->pictureBox1);
			this->panel1->Location = System::Drawing::Point(392, 429);
			this->panel1->Name = L"panel1";
			this->panel1->Size = System::Drawing::Size(209, 198);
			this->panel1->TabIndex = 2;
			// 
			// labelStatus1
			// 
			this->labelStatus1->AutoSize = true;
			this->labelStatus1->ForeColor = System::Drawing::SystemColors::ButtonFace;
			this->labelStatus1->Location = System::Drawing::Point(4, 179);
			this->labelStatus1->Name = L"labelStatus1";
			this->labelStatus1->Size = System::Drawing::Size(11, 16);
			this->labelStatus1->TabIndex = 3;
			this->labelStatus1->Text = L"-";
			// 
			// labelBalanse1
			// 
			this->labelBalanse1->AutoSize = true;
			this->labelBalanse1->ForeColor = System::Drawing::SystemColors::ButtonFace;
			this->labelBalanse1->Location = System::Drawing::Point(4, 163);
			this->labelBalanse1->Name = L"labelBalanse1";
			this->labelBalanse1->Size = System::Drawing::Size(21, 16);
			this->labelBalanse1->TabIndex = 2;
			this->labelBalanse1->Text = L"$0";
			// 
			// labelName1
			// 
			this->labelName1->AutoSize = true;
			this->labelName1->Font = (gcnew System::Drawing::Font(L"Times New Roman", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->labelName1->ForeColor = System::Drawing::SystemColors::ButtonHighlight;
			this->labelName1->Location = System::Drawing::Point(3, 140);
			this->labelName1->Name = L"labelName1";
			this->labelName1->Size = System::Drawing::Size(65, 23);
			this->labelName1->TabIndex = 1;
			this->labelName1->Text = L"Player";
			// 
			// pictureBox1
			// 
			this->pictureBox1->ErrorImage = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox1.ErrorImage")));
			this->pictureBox1->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox1.Image")));
			this->pictureBox1->Location = System::Drawing::Point(6, 3);
			this->pictureBox1->Name = L"pictureBox1";
			this->pictureBox1->Size = System::Drawing::Size(203, 134);
			this->pictureBox1->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			this->pictureBox1->TabIndex = 0;
			this->pictureBox1->TabStop = false;
			// 
			// panel3
			// 
			this->panel3->Controls->Add(this->labelStatus4);
			this->panel3->Controls->Add(this->labelBalanse3);
			this->panel3->Controls->Add(this->labelName3);
			this->panel3->Controls->Add(this->pictureBox3);
			this->panel3->Location = System::Drawing::Point(739, 49);
			this->panel3->Name = L"panel3";
			this->panel3->Size = System::Drawing::Size(209, 198);
			this->panel3->TabIndex = 4;
			// 
			// labelStatus4
			// 
			this->labelStatus4->AutoSize = true;
			this->labelStatus4->ForeColor = System::Drawing::SystemColors::ButtonFace;
			this->labelStatus4->Location = System::Drawing::Point(4, 179);
			this->labelStatus4->Name = L"labelStatus4";
			this->labelStatus4->Size = System::Drawing::Size(11, 16);
			this->labelStatus4->TabIndex = 3;
			this->labelStatus4->Text = L"-";
			// 
			// labelBalanse3
			// 
			this->labelBalanse3->AutoSize = true;
			this->labelBalanse3->ForeColor = System::Drawing::SystemColors::ButtonFace;
			this->labelBalanse3->Location = System::Drawing::Point(4, 163);
			this->labelBalanse3->Name = L"labelBalanse3";
			this->labelBalanse3->Size = System::Drawing::Size(21, 16);
			this->labelBalanse3->TabIndex = 2;
			this->labelBalanse3->Text = L"$0";
			// 
			// labelName3
			// 
			this->labelName3->AutoSize = true;
			this->labelName3->Font = (gcnew System::Drawing::Font(L"Times New Roman", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->labelName3->ForeColor = System::Drawing::SystemColors::ButtonHighlight;
			this->labelName3->Location = System::Drawing::Point(3, 140);
			this->labelName3->Name = L"labelName3";
			this->labelName3->Size = System::Drawing::Size(65, 23);
			this->labelName3->TabIndex = 1;
			this->labelName3->Text = L"Player";
			// 
			// pictureBox3
			// 
			this->pictureBox3->ErrorImage = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox3.ErrorImage")));
			this->pictureBox3->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox3.Image")));
			this->pictureBox3->Location = System::Drawing::Point(6, 3);
			this->pictureBox3->Name = L"pictureBox3";
			this->pictureBox3->Size = System::Drawing::Size(203, 134);
			this->pictureBox3->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			this->pictureBox3->TabIndex = 0;
			this->pictureBox3->TabStop = false;
			// 
			// pictureBox
			// 
			this->pictureBox->BackColor = System::Drawing::Color::Transparent;
			this->pictureBox->BackgroundImage = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox.BackgroundImage")));
			this->pictureBox->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox.Image")));
			this->pictureBox->InitialImage = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox.InitialImage")));
			this->pictureBox->Location = System::Drawing::Point(437, 49);
			this->pictureBox->Name = L"pictureBox";
			this->pictureBox->Size = System::Drawing::Size(141, 112);
			this->pictureBox->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			this->pictureBox->TabIndex = 1;
			this->pictureBox->TabStop = false;
			// 
			// pictureBoxMove
			// 
			this->pictureBoxMove->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBoxMove.Image")));
			this->pictureBoxMove->InitialImage = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBoxMove.InitialImage")));
			this->pictureBoxMove->Location = System::Drawing::Point(542, -44);
			this->pictureBoxMove->Name = L"pictureBoxMove";
			this->pictureBoxMove->Size = System::Drawing::Size(443, 148);
			this->pictureBoxMove->SizeMode = System::Windows::Forms::PictureBoxSizeMode::StretchImage;
			this->pictureBoxMove->TabIndex = 15;
			this->pictureBoxMove->TabStop = false;
			// 
			// card2
			// 
			this->card2->BackColor = System::Drawing::Color::Transparent;
			this->card2->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Zoom;
			this->card2->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"card2.Image")));
			this->card2->Location = System::Drawing::Point(368, 593);
			this->card2->Name = L"card2";
			this->card2->Size = System::Drawing::Size(129, 176);
			this->card2->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			this->card2->TabIndex = 8;
			this->card2->TabStop = false;
			// 
			// card1
			// 
			this->card1->BackColor = System::Drawing::Color::Transparent;
			this->card1->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Zoom;
			this->card1->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"card1.Image")));
			this->card1->Location = System::Drawing::Point(233, 593);
			this->card1->Name = L"card1";
			this->card1->Size = System::Drawing::Size(129, 176);
			this->card1->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			this->card1->TabIndex = 9;
			this->card1->TabStop = false;
			// 
			// cancel
			// 
			this->cancel->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->cancel->AutoEllipsis = true;
			this->cancel->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(47)), static_cast<System::Int32>(static_cast<System::Byte>(47)),
				static_cast<System::Int32>(static_cast<System::Byte>(47)));
			this->cancel->FlatAppearance->BorderColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)),
				static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)));
			this->cancel->FlatAppearance->BorderSize = 3;
			this->cancel->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->cancel->Font = (gcnew System::Drawing::Font(L"Times New Roman", 10));
			this->cancel->ForeColor = System::Drawing::Color::Gray;
			this->cancel->Location = System::Drawing::Point(2, 720);
			this->cancel->Margin = System::Windows::Forms::Padding(3, 2, 3, 2);
			this->cancel->Name = L"cancel";
			this->cancel->Size = System::Drawing::Size(144, 39);
			this->cancel->TabIndex = 5;
			this->cancel->Text = L"выход";
			this->cancel->UseVisualStyleBackColor = false;
			this->cancel->Click += gcnew System::EventHandler(this, &Table::cancel_Click);
			// 
			// fold
			// 
			this->fold->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->fold->AutoEllipsis = true;
			this->fold->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(62)), static_cast<System::Int32>(static_cast<System::Byte>(11)),
				static_cast<System::Int32>(static_cast<System::Byte>(12)));
			this->fold->FlatAppearance->BorderColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)),
				static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)));
			this->fold->FlatAppearance->BorderSize = 3;
			this->fold->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->fold->Font = (gcnew System::Drawing::Font(L"Times New Roman", 10));
			this->fold->ForeColor = System::Drawing::Color::DarkGray;
			this->fold->Location = System::Drawing::Point(510, 690);
			this->fold->Margin = System::Windows::Forms::Padding(3, 2, 3, 2);
			this->fold->Name = L"fold";
			this->fold->Size = System::Drawing::Size(179, 69);
			this->fold->TabIndex = 10;
			this->fold->Text = L"FOLD";
			this->fold->UseVisualStyleBackColor = false;
			this->fold->Click += gcnew System::EventHandler(this, &Table::fold_Click);
			// 
			// check
			// 
			this->check->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->check->AutoEllipsis = true;
			this->check->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(62)), static_cast<System::Int32>(static_cast<System::Byte>(11)),
				static_cast<System::Int32>(static_cast<System::Byte>(12)));
			this->check->FlatAppearance->BorderColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)),
				static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)));
			this->check->FlatAppearance->BorderSize = 3;
			this->check->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->check->Font = (gcnew System::Drawing::Font(L"Times New Roman", 10));
			this->check->ForeColor = System::Drawing::Color::DarkGray;
			this->check->Location = System::Drawing::Point(881, 690);
			this->check->Margin = System::Windows::Forms::Padding(3, 2, 3, 2);
			this->check->Name = L"check";
			this->check->Size = System::Drawing::Size(179, 69);
			this->check->TabIndex = 11;
			this->check->Text = L"CHECK";
			this->check->UseVisualStyleBackColor = false;
			this->check->Click += gcnew System::EventHandler(this, &Table::check_Click);
			// 
			// call
			// 
			this->call->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->call->AutoEllipsis = true;
			this->call->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(62)), static_cast<System::Int32>(static_cast<System::Byte>(11)),
				static_cast<System::Int32>(static_cast<System::Byte>(12)));
			this->call->FlatAppearance->BorderColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)),
				static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)));
			this->call->FlatAppearance->BorderSize = 3;
			this->call->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->call->Font = (gcnew System::Drawing::Font(L"Times New Roman", 10));
			this->call->ForeColor = System::Drawing::Color::DarkGray;
			this->call->Location = System::Drawing::Point(696, 690);
			this->call->Margin = System::Windows::Forms::Padding(3, 2, 3, 2);
			this->call->Name = L"call";
			this->call->Size = System::Drawing::Size(179, 69);
			this->call->TabIndex = 12;
			this->call->Text = L"CALL";
			this->call->UseVisualStyleBackColor = false;
			this->call->Click += gcnew System::EventHandler(this, &Table::call_Click);
			// 
			// raise
			// 
			this->raise->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->raise->AutoEllipsis = true;
			this->raise->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(62)), static_cast<System::Int32>(static_cast<System::Byte>(11)),
				static_cast<System::Int32>(static_cast<System::Byte>(12)));
			this->raise->FlatAppearance->BorderColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)),
				static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)));
			this->raise->FlatAppearance->BorderSize = 3;
			this->raise->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->raise->Font = (gcnew System::Drawing::Font(L"Times New Roman", 10));
			this->raise->ForeColor = System::Drawing::Color::DarkGray;
			this->raise->Location = System::Drawing::Point(1066, 690);
			this->raise->Margin = System::Windows::Forms::Padding(3, 2, 3, 2);
			this->raise->Name = L"raise";
			this->raise->Size = System::Drawing::Size(179, 69);
			this->raise->TabIndex = 13;
			this->raise->Text = L"RAISE";
			this->raise->UseVisualStyleBackColor = false;
			this->raise->Click += gcnew System::EventHandler(this, &Table::RAISE_Click);
			// 
			// trackBarRaise
			// 
			this->trackBarRaise->AutoSize = false;
			this->trackBarRaise->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(0)),
				static_cast<System::Int32>(static_cast<System::Byte>(0)));
			this->trackBarRaise->Location = System::Drawing::Point(1155, 56);
			this->trackBarRaise->Name = L"trackBarRaise";
			this->trackBarRaise->Orientation = System::Windows::Forms::Orientation::Vertical;
			this->trackBarRaise->Size = System::Drawing::Size(50, 621);
			this->trackBarRaise->TabIndex = 14;
			this->trackBarRaise->TickStyle = System::Windows::Forms::TickStyle::Both;
			// 
			// GameStatus
			// 
			this->GameStatus->AutoSize = true;
			this->GameStatus->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(0)),
				static_cast<System::Int32>(static_cast<System::Byte>(0)));
			this->GameStatus->Font = (gcnew System::Drawing::Font(L"Times New Roman", 13.8F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->GameStatus->ForeColor = System::Drawing::Color::Gray;
			this->GameStatus->Location = System::Drawing::Point(12, 20);
			this->GameStatus->Name = L"GameStatus";
			this->GameStatus->Size = System::Drawing::Size(219, 25);
			this->GameStatus->TabIndex = 15;
			this->GameStatus->Text = L"Ожидаем игроков...";
			// 
			// labelRaise
			// 
			this->labelRaise->AutoSize = true;
			this->labelRaise->BackColor = System::Drawing::Color::Transparent;
			this->labelRaise->Font = (gcnew System::Drawing::Font(L"Times New Roman", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->labelRaise->ForeColor = System::Drawing::SystemColors::ButtonHighlight;
			this->labelRaise->Location = System::Drawing::Point(1096, 22);
			this->labelRaise->Name = L"labelRaise";
			this->labelRaise->Size = System::Drawing::Size(0, 23);
			this->labelRaise->TabIndex = 16;
			// 
			// PotLabel
			// 
			this->PotLabel->AutoSize = true;
			this->PotLabel->Font = (gcnew System::Drawing::Font(L"Times New Roman", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->PotLabel->ForeColor = System::Drawing::SystemColors::ButtonHighlight;
			this->PotLabel->Location = System::Drawing::Point(433, 166);
			this->PotLabel->Name = L"PotLabel";
			this->PotLabel->Size = System::Drawing::Size(64, 23);
			this->PotLabel->TabIndex = 17;
			this->PotLabel->Text = L"Банк:";
			// 
			// CurrentBetLabel
			// 
			this->CurrentBetLabel->AutoSize = true;
			this->CurrentBetLabel->Font = (gcnew System::Drawing::Font(L"Times New Roman", 10.8F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->CurrentBetLabel->ForeColor = System::Drawing::SystemColors::ButtonHighlight;
			this->CurrentBetLabel->Location = System::Drawing::Point(433, 192);
			this->CurrentBetLabel->Name = L"CurrentBetLabel";
			this->CurrentBetLabel->Size = System::Drawing::Size(135, 20);
			this->CurrentBetLabel->TabIndex = 18;
			this->CurrentBetLabel->Text = L"Текущая ставка:";
			// 
			// Table
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackgroundImage = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"$this.BackgroundImage")));
			this->ClientSize = System::Drawing::Size(1261, 770);
			this->Controls->Add(this->labelRaise);
			this->Controls->Add(this->GameStatus);
			this->Controls->Add(this->trackBarRaise);
			this->Controls->Add(this->raise);
			this->Controls->Add(this->call);
			this->Controls->Add(this->check);
			this->Controls->Add(this->fold);
			this->Controls->Add(this->card2);
			this->Controls->Add(this->card1);
			this->Controls->Add(this->cancel);
			this->Controls->Add(this->panel);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::None;
			this->Icon = (cli::safe_cast<System::Drawing::Icon^>(resources->GetObject(L"$this.Icon")));
			this->Margin = System::Windows::Forms::Padding(3, 2, 3, 2);
			this->Name = L"Table";
			this->panel->ResumeLayout(false);
			this->panel->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->tableCard5))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->tableCard4))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->tableCard3))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->tableCard2))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->tableCard1))->EndInit();
			this->panel5->ResumeLayout(false);
			this->panel5->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox5))->EndInit();
			this->panel4->ResumeLayout(false);
			this->panel4->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox4))->EndInit();
			this->panel2->ResumeLayout(false);
			this->panel2->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox2))->EndInit();
			this->panel1->ResumeLayout(false);
			this->panel1->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->EndInit();
			this->panel3->ResumeLayout(false);
			this->panel3->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox3))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBoxMove))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->card2))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->card1))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackBarRaise))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion


private: System::Void cancel_Click(System::Object^ sender, System::EventArgs^ e) {
	try {


		String^ command = "EXIT\n";
		GameNetwork^ network = GameNetwork::getInstance("127.0.0.1", 5555);

		if (!network->isConnected()) {
			Console::WriteLine("Ошибка: соединение с сервером отсутствует!");
			MessageBox::Show("Не удалось подключиться к серверу!", "Ошибка", MessageBoxButtons::OK, MessageBoxIcon::Error);
			return;
		}

		network->sendCommand(command);

		Application::Exit();
		Application::Exit();
	}
	catch (Exception^ ex) {
		Console::WriteLine("Error in cancel_Click: " + ex->Message);
	}
}

private: System::Void fold_Click(System::Object^ sender, System::EventArgs^ e) {
	try {
		String^ command = "ACTIONS: FOLD\n";
		GameNetwork^ network = GameNetwork::getInstance("127.0.0.1", 5555);

		if (!network->isConnected()) {
			Console::WriteLine("Ошибка: соединение с сервером отсутствует!");
			MessageBox::Show("Не удалось подключиться к серверу!", "Ошибка", MessageBoxButtons::OK, MessageBoxIcon::Error);
			return;
		}

		network->sendCommand(command);
		Console::WriteLine("Запрос отправлен: " + command);
		HideActionsButtons();
	}
	catch (Exception^ ex) {
		Console::WriteLine("Ошибка в fold_Click: " + ex->Message);
	}
}

private: System::Void call_Click(System::Object^ sender, System::EventArgs^ e) {
	try {
		String^ command = "ACTIONS: CALL\n";
		GameNetwork^ network = GameNetwork::getInstance("127.0.0.1", 5555);

		if (!network->isConnected()) {
			Console::WriteLine("Ошибка: соединение с сервером отсутствует!");
			MessageBox::Show("Не удалось подключиться к серверу!", "Ошибка", MessageBoxButtons::OK, MessageBoxIcon::Error);
			return;
		}

		network->sendCommand(command);
		Console::WriteLine("Запрос отправлен: " + command);
		HideActionsButtons();
	}
	catch (Exception^ ex) {
		Console::WriteLine("Ошибка в call_Click: " + ex->Message);
	}
}

private: System::Void check_Click(System::Object^ sender, System::EventArgs^ e) {
	try {
		String^ command = "ACTIONS: CHECK\n";
		GameNetwork^ network = GameNetwork::getInstance("127.0.0.1", 5555);

		if (!network->isConnected()) {
			Console::WriteLine("Ошибка: соединение с сервером отсутствует!");
			MessageBox::Show("Не удалось подключиться к серверу!", "Ошибка", MessageBoxButtons::OK, MessageBoxIcon::Error);
			return;
		}

		network->sendCommand(command);
		Console::WriteLine("Запрос отправлен: " + command);
		HideActionsButtons();
	}
	catch (Exception^ ex) {
		Console::WriteLine("Ошибка в check_Click: " + ex->Message);
	}
}

	   bool isTrackBarVisible = false; // для отслеживания состояния TrackBar

private: System::Void RAISE_Click(System::Object^ sender, System::EventArgs^ e) {
	try {
		if (!isTrackBarVisible) {
			// Отображаем TrackBar при первом клике
			trackBarRaise->Visible = true;
			isTrackBarVisible = true;
		}
		else {
			// При повторном клике отправляем значение ставки
			int rs = trackBarRaise->Value;
			String^ command = "ACTIONS: RAISE " + rs.ToString() + "\n";
			GameNetwork^ network = GameNetwork::getInstance("127.0.0.1", 5555);

			if (!network->isConnected()) {
				Console::WriteLine("Ошибка: соединение с сервером отсутствует!");
				MessageBox::Show("Не удалось подключиться к серверу!", "Ошибка", MessageBoxButtons::OK, MessageBoxIcon::Error);
				return;
			}

			network->sendCommand(command);
			Console::WriteLine("Запрос отправлен: " + command);
			labelRaise->Text = "" ;
			// Скрываем TrackBar после отправки
			trackBarRaise->Visible = false;
			isTrackBarVisible = false;
			HideActionsButtons();
		}
	}
	catch (Exception^ ex) {
		Console::WriteLine("Ошибка в RAISE_Click: " + ex->Message);
	}
}
		

};
}
