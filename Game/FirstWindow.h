#pragma once
#include "Entry.h" 
#include "MyForm.h" 


namespace Game {	
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for FirstWindow
	/// </summary>
	public ref class FirstWindow : public System::Windows::Forms::Form
	{
	public:
		FirstWindow(void)
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
		~FirstWindow()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Button^ entry;
	private: System::Windows::Forms::Button^ registration;
	private: System::Windows::Forms::Button^ exit;
	private: System::Windows::Forms::Panel^ panel1;


	public:
		void ShowWindow() {
			this->ShowDialog();
		}



	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container^ components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			System::ComponentModel::ComponentResourceManager^ resources = (gcnew System::ComponentModel::ComponentResourceManager(FirstWindow::typeid));
			this->entry = (gcnew System::Windows::Forms::Button());
			this->registration = (gcnew System::Windows::Forms::Button());
			this->exit = (gcnew System::Windows::Forms::Button());
			this->panel1 = (gcnew System::Windows::Forms::Panel());
			this->SuspendLayout();
			// 
			// entry
			// 
			this->entry->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->entry->AutoEllipsis = true;
			this->entry->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(89)), static_cast<System::Int32>(static_cast<System::Byte>(0)),
				static_cast<System::Int32>(static_cast<System::Byte>(2)));
			this->entry->FlatAppearance->BorderColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)),
				static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)));
			this->entry->FlatAppearance->BorderSize = 3;
			this->entry->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->entry->Font = (gcnew System::Drawing::Font(L"Times New Roman", 10));
			this->entry->ForeColor = System::Drawing::Color::DarkGray;
			this->entry->Location = System::Drawing::Point(83, 240);
			this->entry->Margin = System::Windows::Forms::Padding(2);
			this->entry->Name = L"entry";
			this->entry->Size = System::Drawing::Size(196, 36);
			this->entry->TabIndex = 0;
			this->entry->Text = L"войти";
			this->entry->UseVisualStyleBackColor = false;
			this->entry->Click += gcnew System::EventHandler(this, &FirstWindow::entry_Click);
			// 
			// registration
			// 
			this->registration->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->registration->AutoEllipsis = true;
			this->registration->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(89)), static_cast<System::Int32>(static_cast<System::Byte>(0)),
				static_cast<System::Int32>(static_cast<System::Byte>(2)));
			this->registration->FlatAppearance->BorderColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)),
				static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)));
			this->registration->FlatAppearance->BorderSize = 3;
			this->registration->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->registration->Font = (gcnew System::Drawing::Font(L"Times New Roman", 10));
			this->registration->ForeColor = System::Drawing::Color::DarkGray;
			this->registration->Location = System::Drawing::Point(83, 283);
			this->registration->Margin = System::Windows::Forms::Padding(2);
			this->registration->Name = L"registration";
			this->registration->Size = System::Drawing::Size(196, 36);
			this->registration->TabIndex = 2;
			this->registration->Text = L"зарегистрироваться";
			this->registration->UseVisualStyleBackColor = false;
			this->registration->Click += gcnew System::EventHandler(this, &FirstWindow::registration_Click);
			// 
			// exit
			// 
			this->exit->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->exit->AutoEllipsis = true;
			this->exit->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				static_cast<System::Int32>(static_cast<System::Byte>(64)));
			this->exit->FlatAppearance->BorderColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)),
				static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)));
			this->exit->FlatAppearance->BorderSize = 3;
			this->exit->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->exit->Font = (gcnew System::Drawing::Font(L"Times New Roman", 10));
			this->exit->ForeColor = System::Drawing::Color::Gray;
			this->exit->Location = System::Drawing::Point(83, 384);
			this->exit->Margin = System::Windows::Forms::Padding(2);
			this->exit->Name = L"exit";
			this->exit->Size = System::Drawing::Size(196, 36);
			this->exit->TabIndex = 3;
			this->exit->Text = L"выйти";
			this->exit->UseVisualStyleBackColor = false;
			this->exit->Click += gcnew System::EventHandler(this, &FirstWindow::exit_Click);
			// 
			// panel1
			// 
			this->panel1->BackColor = System::Drawing::Color::Transparent;
			this->panel1->BackgroundImage = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"panel1.BackgroundImage")));
			this->panel1->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Zoom;
			this->panel1->Location = System::Drawing::Point(21, 14);
			this->panel1->Margin = System::Windows::Forms::Padding(2);
			this->panel1->Name = L"panel1";
			this->panel1->Size = System::Drawing::Size(331, 222);
			this->panel1->TabIndex = 4;
			// 
			// FirstWindow
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::SystemColors::ActiveCaptionText;
			this->BackgroundImage = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"$this.BackgroundImage")));
			this->BackgroundImageLayout = System::Windows::Forms::ImageLayout::None;
			this->ClientSize = System::Drawing::Size(373, 488);
			this->Controls->Add(this->panel1);
			this->Controls->Add(this->exit);
			this->Controls->Add(this->registration);
			this->Controls->Add(this->entry);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::None;
			this->Margin = System::Windows::Forms::Padding(2);
			this->MinimizeBox = false;
			this->Name = L"FirstWindow";
			this->Text = L"FirstWindow";
			this->ResumeLayout(false);

		}

#pragma endregion
	private: System::Void entry_Click(System::Object^ sender, System::EventArgs^ e) {
		this->Hide();
		this->DialogResult = System::Windows::Forms::DialogResult::OK; // Исправленный вариант
		this->Close();
	}

	private: System::Void exit_Click(System::Object^ sender, System::EventArgs^ e) {
		Application::Exit(); // Корректное завершение всего приложения
	}

	private: System::Void registration_Click(System::Object^ sender, System::EventArgs^ e) {
		this->Hide();
		this->DialogResult = System::Windows::Forms::DialogResult::Retry; // Исправленный вариант
		this->Close();
	}
};
#pragma endregion

}
