#pragma once
#include "GameNetwork.h"
#using <System.Threading.dll>
using namespace System::Threading;

#using <System.Windows.Forms.dll>
using namespace System::Windows::Forms;
namespace Game {
	using namespace Game;

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for Entry
	/// </summary>
	public ref class Entry : public System::Windows::Forms::Form
	{
	public:
		Entry(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~Entry()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Button^ reg;
	private: System::Windows::Forms::Button^ cancel;
	private: System::Windows::Forms::Panel^ panel1;
	private: System::Windows::Forms::Label^ name;
	private: System::Windows::Forms::TextBox^ nameBox;
	private: System::Windows::Forms::Label^ password1;
	private: System::Windows::Forms::TextBox^ textBox1;
	private: System::Windows::Forms::CheckBox^ checkBox1;
	private: System::Windows::Forms::Label^ label1;


	public:
		void ShowWindow() {
			this->ShowDialog();
		}

	protected:




	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			System::ComponentModel::ComponentResourceManager^ resources = (gcnew System::ComponentModel::ComponentResourceManager(Entry::typeid));
			this->reg = (gcnew System::Windows::Forms::Button());
			this->cancel = (gcnew System::Windows::Forms::Button());
			this->panel1 = (gcnew System::Windows::Forms::Panel());
			this->textBox1 = (gcnew System::Windows::Forms::TextBox());
			this->checkBox1 = (gcnew System::Windows::Forms::CheckBox());
			this->password1 = (gcnew System::Windows::Forms::Label());
			this->nameBox = (gcnew System::Windows::Forms::TextBox());
			this->name = (gcnew System::Windows::Forms::Label());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->panel1->SuspendLayout();
			this->SuspendLayout();
			// 
			// reg
			// 
			this->reg->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->reg->AutoEllipsis = true;
			this->reg->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(62)), static_cast<System::Int32>(static_cast<System::Byte>(11)),
				static_cast<System::Int32>(static_cast<System::Byte>(12)));
			this->reg->FlatAppearance->BorderColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)),
				static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)));
			this->reg->FlatAppearance->BorderSize = 3;
			this->reg->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->reg->Font = (gcnew System::Drawing::Font(L"Times New Roman", 10));
			this->reg->ForeColor = System::Drawing::Color::DarkGray;
			this->reg->Location = System::Drawing::Point(129, 475);
			this->reg->Margin = System::Windows::Forms::Padding(3, 2, 3, 2);
			this->reg->Name = L"reg";
			this->reg->Size = System::Drawing::Size(315, 60);
			this->reg->TabIndex = 3;
			this->reg->Text = L"вход";
			this->reg->UseVisualStyleBackColor = false;
			this->reg->Click += gcnew System::EventHandler(this, &Entry::reg_Click);
			// 
			// cancel
			// 
			this->cancel->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->cancel->AutoEllipsis = true;
			this->cancel->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				static_cast<System::Int32>(static_cast<System::Byte>(64)));
			this->cancel->FlatAppearance->BorderColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)),
				static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)));
			this->cancel->FlatAppearance->BorderSize = 3;
			this->cancel->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->cancel->Font = (gcnew System::Drawing::Font(L"Times New Roman", 10));
			this->cancel->ForeColor = System::Drawing::Color::DarkGray;
			this->cancel->Location = System::Drawing::Point(129, 551);
			this->cancel->Margin = System::Windows::Forms::Padding(3, 2, 3, 2);
			this->cancel->Name = L"cancel";
			this->cancel->Size = System::Drawing::Size(315, 60);
			this->cancel->TabIndex = 4;
			this->cancel->Text = L"отмена";
			this->cancel->UseVisualStyleBackColor = false;
			this->cancel->Click += gcnew System::EventHandler(this, &Entry::cancel_Click);
			// 
			// panel1
			// 
			this->panel1->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(62)), static_cast<System::Int32>(static_cast<System::Byte>(11)),
				static_cast<System::Int32>(static_cast<System::Byte>(12)));
			this->panel1->BackgroundImageLayout = System::Windows::Forms::ImageLayout::None;
			this->panel1->Controls->Add(this->textBox1);
			this->panel1->Controls->Add(this->checkBox1);
			this->panel1->Controls->Add(this->password1);
			this->panel1->Controls->Add(this->nameBox);
			this->panel1->Controls->Add(this->name);
			this->panel1->Location = System::Drawing::Point(60, 199);
			this->panel1->Margin = System::Windows::Forms::Padding(3, 2, 3, 2);
			this->panel1->Name = L"panel1";
			this->panel1->Size = System::Drawing::Size(465, 249);
			this->panel1->TabIndex = 5;
			// 
			// textBox1
			// 
			this->textBox1->BackColor = System::Drawing::Color::LightGray;
			this->textBox1->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->textBox1->Font = (gcnew System::Drawing::Font(L"Times New Roman", 10, System::Drawing::FontStyle::Italic));
			this->textBox1->ForeColor = System::Drawing::Color::Black;
			this->textBox1->Location = System::Drawing::Point(133, 143);
			this->textBox1->Margin = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->textBox1->Name = L"textBox1";
			this->textBox1->Size = System::Drawing::Size(299, 27);
			this->textBox1->TabIndex = 9;
			this->textBox1->UseSystemPasswordChar = true;
			// 
			// checkBox1
			// 
			this->checkBox1->AutoSize = true;
			this->checkBox1->ForeColor = System::Drawing::SystemColors::ActiveBorder;
			this->checkBox1->Location = System::Drawing::Point(133, 190);
			this->checkBox1->Margin = System::Windows::Forms::Padding(3, 2, 3, 2);
			this->checkBox1->Name = L"checkBox1";
			this->checkBox1->Size = System::Drawing::Size(140, 20);
			this->checkBox1->TabIndex = 8;
			this->checkBox1->Text = L"показать пароль";
			this->checkBox1->UseVisualStyleBackColor = true;
			this->checkBox1->CheckedChanged += gcnew System::EventHandler(this, &Entry::checkBox1_CheckedChanged);
			// 
			// password1
			// 
			this->password1->AutoSize = true;
			this->password1->Font = (gcnew System::Drawing::Font(L"Times New Roman", 10.8F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->password1->ForeColor = System::Drawing::SystemColors::AppWorkspace;
			this->password1->Location = System::Drawing::Point(21, 143);
			this->password1->MinimumSize = System::Drawing::Size(100, 20);
			this->password1->Name = L"password1";
			this->password1->Size = System::Drawing::Size(100, 20);
			this->password1->TabIndex = 4;
			this->password1->Text = L"Пароль :";
			this->password1->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// nameBox
			// 
			this->nameBox->BackColor = System::Drawing::Color::LightGray;
			this->nameBox->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->nameBox->Font = (gcnew System::Drawing::Font(L"Times New Roman", 12));
			this->nameBox->ForeColor = System::Drawing::Color::Black;
			this->nameBox->Location = System::Drawing::Point(133, 50);
			this->nameBox->Margin = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->nameBox->Name = L"nameBox";
			this->nameBox->Size = System::Drawing::Size(299, 30);
			this->nameBox->TabIndex = 2;
			// 
			// name
			// 
			this->name->AutoSize = true;
			this->name->Font = (gcnew System::Drawing::Font(L"Times New Roman", 10.8F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->name->ForeColor = System::Drawing::SystemColors::AppWorkspace;
			this->name->Location = System::Drawing::Point(21, 50);
			this->name->MinimumSize = System::Drawing::Size(100, 20);
			this->name->Name = L"name";
			this->name->Size = System::Drawing::Size(100, 20);
			this->name->TabIndex = 1;
			this->name->Text = L"Имя :";
			this->name->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->BackColor = System::Drawing::Color::Transparent;
			this->label1->Font = (gcnew System::Drawing::Font(L"Times New Roman", 36, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label1->ForeColor = System::Drawing::Color::LightGray;
			this->label1->Location = System::Drawing::Point(199, 105);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(167, 68);
			this->label1->TabIndex = 6;
			this->label1->Text = L" Вход";
			// 
			// Entry
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackgroundImage = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"$this.BackgroundImage")));
			this->ClientSize = System::Drawing::Size(588, 672);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->panel1);
			this->Controls->Add(this->cancel);
			this->Controls->Add(this->reg);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::None;
			this->Margin = System::Windows::Forms::Padding(3, 2, 3, 2);
			this->MinimizeBox = false;
			this->Name = L"Entry";
			this->Text = L"Entry";
			this->panel1->ResumeLayout(false);
			this->panel1->PerformLayout();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
		private: System::Void entry_Click(System::Object^ sender, System::EventArgs^ e) {

		}
		private: System::Void checkBox1_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
			textBox1->UseSystemPasswordChar = !checkBox1->Checked; // Только смена видимости пароля
		}


private: System::Void cancel_Click(System::Object^ sender, System::EventArgs^ e) {
	this->DialogResult = System::Windows::Forms::DialogResult::Abort;
	this->Close(); 
}

private: System::Void reg_Click(System::Object^ sender, System::EventArgs^ e) {
	// Получаем имя пользователя и пароль
	String^ username = nameBox->Text;
	String^ password = textBox1->Text;

	// Проверяем, что поля не пустые
	if (String::IsNullOrEmpty(username) || String::IsNullOrEmpty(password)) {
		MessageBox::Show("Введите корректные данные!", "Ошибка", MessageBoxButtons::OK, MessageBoxIcon::Warning);
		return;
	}

	// Формируем команду для сервера (ENTRY вместо REGISTRATION)
	String^ command = "ENTRY " + username + ":" + password;
	GameNetwork^ network = GameNetwork::getInstance("127.0.0.1", 5555);

	if (!network->isConnected()) {
		Console::WriteLine("Ошибка: соединение с сервером отсутствует!");
		MessageBox::Show("Не удалось подключиться к серверу!", "Ошибка", MessageBoxButtons::OK, MessageBoxIcon::Error);
		return;
	}

	// Отправляем запрос на вход
	network->sendCommand(command);
	Console::WriteLine("Запрос отправлен: " + command);
}

};
}
