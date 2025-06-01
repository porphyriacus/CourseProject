#pragma once
#include "GameNetwork.h"
#using <System.Threading.dll>
using namespace System::Threading;

#using <System.Windows.Forms.dll>
using namespace System::Windows::Forms;

namespace Game {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for MyForm
	/// </summary>
	public ref class MyForm : public System::Windows::Forms::Form
	{
	public:
		MyForm(void)
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
		~MyForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Panel^ panel1;
	private: System::Windows::Forms::TextBox^ nameBox;


	private: System::Windows::Forms::Label^ name;
	private: System::Windows::Forms::ToolTip^ toolTip1;
	private: System::Windows::Forms::Label^ password1;


	private: System::Windows::Forms::Label^ password2;

	private: System::Windows::Forms::TextBox^ passwordBox2;

	private: System::Windows::Forms::TextBox^ passwordBox1;
	private: System::Windows::Forms::CheckBox^ checkBox1;
	private: System::Windows::Forms::Button^ reg;
	private: System::Windows::Forms::Button^ cancel;

	private: System::Windows::Forms::ToolTip^ toolTip2;

	private: System::Windows::Forms::ToolTip^ toolTip3;
	private: System::Windows::Forms::Label^ label1;
	private: System::Windows::Forms::Label^ passwordErrorLabel;


	private: System::ComponentModel::IContainer^ components;

	protected:



	private: System::Void OnFocus(System::Object^ sender, System::EventArgs^ e) {
		TextBox^ box = safe_cast<TextBox^>(sender);
		if (box->ForeColor == System::Drawing::Color::DimGray) {
			box->Text = ""; // Очищаем поле при фокусе
			box->ForeColor = System::Drawing::Color::Black; // Чёрный цвет для ввода
			if (box == passwordBox1 || box == passwordBox2) {
				box->UseSystemPasswordChar = !checkBox1->Checked; // Скрываем или показываем пароль
			}
		}
	}

	private: System::Void OnLostFocus(System::Object^ sender, System::EventArgs^ e) {
		TextBox^ box = safe_cast<TextBox^>(sender);
		if (box->Text == "") {
			box->ForeColor = System::Drawing::Color::DimGray; // Возвращаем серый цвет
			if (box == nameBox) box->Text = "Введите имя...";
			if (box == passwordBox1) box->Text = "Введите пароль...";
			if (box == passwordBox2) box->Text = "Повторите пароль...";
			if (box == passwordBox1 || box == passwordBox2) box->UseSystemPasswordChar = false; // Показываем символы пароля как текст
		}
	}



	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			System::ComponentModel::ComponentResourceManager^ resources = (gcnew System::ComponentModel::ComponentResourceManager(MyForm::typeid));
			this->panel1 = (gcnew System::Windows::Forms::Panel());
			this->checkBox1 = (gcnew System::Windows::Forms::CheckBox());
			this->password2 = (gcnew System::Windows::Forms::Label());
			this->passwordBox2 = (gcnew System::Windows::Forms::TextBox());
			this->passwordBox1 = (gcnew System::Windows::Forms::TextBox());
			this->password1 = (gcnew System::Windows::Forms::Label());
			this->nameBox = (gcnew System::Windows::Forms::TextBox());
			this->name = (gcnew System::Windows::Forms::Label());
			this->toolTip1 = (gcnew System::Windows::Forms::ToolTip(this->components));
			this->reg = (gcnew System::Windows::Forms::Button());
			this->cancel = (gcnew System::Windows::Forms::Button());
			this->toolTip2 = (gcnew System::Windows::Forms::ToolTip(this->components));
			this->toolTip3 = (gcnew System::Windows::Forms::ToolTip(this->components));
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->passwordErrorLabel = (gcnew System::Windows::Forms::Label());
			this->panel1->SuspendLayout();
			this->SuspendLayout();
			// 
			// panel1
			// 
			this->panel1->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(62)), static_cast<System::Int32>(static_cast<System::Byte>(11)),
				static_cast<System::Int32>(static_cast<System::Byte>(12)));
			this->panel1->Controls->Add(this->checkBox1);
			this->panel1->Controls->Add(this->password2);
			this->panel1->Controls->Add(this->passwordBox2);
			this->panel1->Controls->Add(this->passwordBox1);
			this->panel1->Controls->Add(this->password1);
			this->panel1->Controls->Add(this->nameBox);
			this->panel1->Controls->Add(this->name);
			this->panel1->ForeColor = System::Drawing::SystemColors::ActiveCaption;
			this->panel1->Location = System::Drawing::Point(48, 115);
			this->panel1->Margin = System::Windows::Forms::Padding(38, 2, 38, 2);
			this->panel1->Name = L"panel1";
			this->panel1->Size = System::Drawing::Size(524, 225);
			this->panel1->TabIndex = 0;
			// 
			// checkBox1
			// 
			this->checkBox1->AutoSize = true;
			this->checkBox1->ForeColor = System::Drawing::SystemColors::ActiveBorder;
			this->checkBox1->Location = System::Drawing::Point(207, 196);
			this->checkBox1->Name = L"checkBox1";
			this->checkBox1->Size = System::Drawing::Size(112, 17);
			this->checkBox1->TabIndex = 8;
			this->checkBox1->Text = L"показать пароль";
			this->checkBox1->UseVisualStyleBackColor = true;
			this->checkBox1->CheckedChanged += gcnew System::EventHandler(this, &MyForm::checkBox1_CheckedChanged);
			// 
			// password2
			// 
			this->password2->AutoSize = true;
			this->password2->Font = (gcnew System::Drawing::Font(L"Times New Roman", 10.8F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->password2->ForeColor = System::Drawing::SystemColors::AppWorkspace;
			this->password2->Location = System::Drawing::Point(47, 167);
			this->password2->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->password2->MinimumSize = System::Drawing::Size(75, 16);
			this->password2->Name = L"password2";
			this->password2->Size = System::Drawing::Size(142, 17);
			this->password2->TabIndex = 6;
			this->password2->Text = L"Повторите пароль:";
			this->password2->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// passwordBox2
			// 
			this->passwordBox2->BackColor = System::Drawing::Color::LightGray;
			this->passwordBox2->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->passwordBox2->Font = (gcnew System::Drawing::Font(L"Times New Roman", 10, System::Drawing::FontStyle::Italic));
			this->passwordBox2->ForeColor = System::Drawing::Color::Black;
			this->passwordBox2->Location = System::Drawing::Point(207, 167);
			this->passwordBox2->Name = L"passwordBox2";
			this->passwordBox2->Size = System::Drawing::Size(225, 23);
			this->passwordBox2->TabIndex = 5;
			this->passwordBox2->UseSystemPasswordChar = true;
			this->passwordBox2->TextChanged += gcnew System::EventHandler(this, &MyForm::ValidatePasswords);
			// 
			// passwordBox1
			// 
			this->passwordBox1->BackColor = System::Drawing::Color::LightGray;
			this->passwordBox1->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->passwordBox1->Font = (gcnew System::Drawing::Font(L"Times New Roman", 10, System::Drawing::FontStyle::Italic));
			this->passwordBox1->ForeColor = System::Drawing::Color::Black;
			this->passwordBox1->Location = System::Drawing::Point(207, 108);
			this->passwordBox1->Name = L"passwordBox1";
			this->passwordBox1->Size = System::Drawing::Size(225, 23);
			this->passwordBox1->TabIndex = 4;
			this->passwordBox1->UseSystemPasswordChar = true;
			this->passwordBox1->TextChanged += gcnew System::EventHandler(this, &MyForm::ValidatePasswords);
			// 
			// password1
			// 
			this->password1->AutoSize = true;
			this->password1->Font = (gcnew System::Drawing::Font(L"Times New Roman", 10.8F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->password1->ForeColor = System::Drawing::SystemColors::AppWorkspace;
			this->password1->Location = System::Drawing::Point(47, 108);
			this->password1->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->password1->MinimumSize = System::Drawing::Size(75, 16);
			this->password1->Name = L"password1";
			this->password1->Size = System::Drawing::Size(75, 17);
			this->password1->TabIndex = 3;
			this->password1->Text = L"Пароль :";
			this->password1->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// nameBox
			// 
			this->nameBox->BackColor = System::Drawing::Color::LightGray;
			this->nameBox->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->nameBox->Font = (gcnew System::Drawing::Font(L"Times New Roman", 12));
			this->nameBox->ForeColor = System::Drawing::Color::Black;
			this->nameBox->Location = System::Drawing::Point(207, 48);
			this->nameBox->Name = L"nameBox";
			this->nameBox->Size = System::Drawing::Size(225, 26);
			this->nameBox->TabIndex = 2;
			// 
			// name
			// 
			this->name->AutoSize = true;
			this->name->Font = (gcnew System::Drawing::Font(L"Times New Roman", 10.8F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->name->ForeColor = System::Drawing::SystemColors::AppWorkspace;
			this->name->Location = System::Drawing::Point(47, 48);
			this->name->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->name->MinimumSize = System::Drawing::Size(75, 16);
			this->name->Name = L"name";
			this->name->Size = System::Drawing::Size(75, 17);
			this->name->TabIndex = 0;
			this->name->Text = L"Имя :";
			this->name->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
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
			this->reg->Location = System::Drawing::Point(98, 363);
			this->reg->Margin = System::Windows::Forms::Padding(2);
			this->reg->Name = L"reg";
			this->reg->Size = System::Drawing::Size(195, 49);
			this->reg->TabIndex = 2;
			this->reg->Text = L"зарегитрироваться";
			this->reg->UseVisualStyleBackColor = false;
			this->reg->Click += gcnew System::EventHandler(this, &MyForm::reg_Click);
			// 
			// cancel
			// 
			this->cancel->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->cancel->AutoEllipsis = true;
			this->cancel->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(62)), static_cast<System::Int32>(static_cast<System::Byte>(11)),
				static_cast<System::Int32>(static_cast<System::Byte>(12)));
			this->cancel->FlatAppearance->BorderColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)),
				static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)));
			this->cancel->FlatAppearance->BorderSize = 3;
			this->cancel->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->cancel->Font = (gcnew System::Drawing::Font(L"Times New Roman", 10));
			this->cancel->ForeColor = System::Drawing::Color::DarkGray;
			this->cancel->Location = System::Drawing::Point(314, 363);
			this->cancel->Margin = System::Windows::Forms::Padding(2);
			this->cancel->Name = L"cancel";
			this->cancel->Size = System::Drawing::Size(195, 49);
			this->cancel->TabIndex = 3;
			this->cancel->Text = L"отмена";
			this->cancel->UseVisualStyleBackColor = false;
			this->cancel->Click += gcnew System::EventHandler(this, &MyForm::cancel_Click);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->BackColor = System::Drawing::Color::Transparent;
			this->label1->Font = (gcnew System::Drawing::Font(L"Times New Roman", 28.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label1->ForeColor = System::Drawing::Color::LightGray;
			this->label1->Location = System::Drawing::Point(197, 50);
			this->label1->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(239, 43);
			this->label1->TabIndex = 4;
			this->label1->Text = L"Регистрация";
			// 
			// passwordErrorLabel
			// 
			this->passwordErrorLabel->AutoSize = true;
			this->passwordErrorLabel->BackColor = System::Drawing::Color::DarkGray;
			this->passwordErrorLabel->Font = (gcnew System::Drawing::Font(L"Times New Roman", 10, System::Drawing::FontStyle::Italic));
			this->passwordErrorLabel->ForeColor = System::Drawing::Color::DarkRed;
			this->passwordErrorLabel->Location = System::Drawing::Point(330, 200);
			this->passwordErrorLabel->Name = L"passwordErrorLabel";
			this->passwordErrorLabel->Size = System::Drawing::Size(0, 16);
			this->passwordErrorLabel->TabIndex = 6;
			// 
			// MyForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->AutoValidate = System::Windows::Forms::AutoValidate::EnablePreventFocusChange;
			this->BackColor = System::Drawing::SystemColors::ActiveCaption;
			this->BackgroundImage = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"$this.BackgroundImage")));
			this->BackgroundImageLayout = System::Windows::Forms::ImageLayout::None;
			this->ClientSize = System::Drawing::Size(615, 430);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->cancel);
			this->Controls->Add(this->reg);
			this->Controls->Add(this->passwordErrorLabel);
			this->Controls->Add(this->panel1);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::None;
			this->Icon = (cli::safe_cast<System::Drawing::Icon^>(resources->GetObject(L"$this.Icon")));
			this->Margin = System::Windows::Forms::Padding(2);
			this->MinimizeBox = false;
			this->Name = L"MyForm";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"регистрация";
			this->panel1->ResumeLayout(false);
			this->panel1->PerformLayout();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma region UI Handlers

private: System::Void checkBox1_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
	bool showPassword = checkBox1->Checked;
	passwordBox1->UseSystemPasswordChar = !showPassword;
	passwordBox2->UseSystemPasswordChar = !showPassword;
}

private: System::Void ValidatePasswords(System::Object^ sender, System::EventArgs^ e) {
	if (passwordBox1->Text != passwordBox2->Text)
		passwordErrorLabel->Text = L"Пароли не совпадают";
	else
		passwordErrorLabel->Text = L"";
}

private: System::Void cancel_Click(System::Object^ sender, System::EventArgs^ e) {
	this->DialogResult = System::Windows::Forms::DialogResult::Abort;
	this->Close();
}

#pragma endregion

private: System::String^ regUsername;
private: System::String^ regPassword;

private: System::Void reg_Click(System::Object^ sender, System::EventArgs^ e) {
	// Проверяем совпадение паролей
	if (passwordBox1->Text != passwordBox2->Text) {
		passwordErrorLabel->Text = L"Пароли не совпадают";
		return;
	}

	passwordErrorLabel->Text = L""; // Очищаем ошибку, если всё корректно

	// Формируем команду для сервера
	String^ username = nameBox->Text;
	String^ password = passwordBox1->Text;

	if (String::IsNullOrEmpty(username) || String::IsNullOrEmpty(password)) {
		passwordErrorLabel->Text = L"Введите корректные данные";
		return;
	}

	String^ command = "REGISTRATION " + username + ":" + password;
	GameNetwork^ network = GameNetwork::getInstance("127.0.0.1", 5555);

	if (!network->isConnected()) {
		Console::WriteLine("Ошибка: соединение с сервером отсутствует!");
		return;
	}

	network->sendCommand(command);
	Console::WriteLine("Запрос отправлен: " + command);
}

};
	
}
