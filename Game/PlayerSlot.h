#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace Game {


    [System::ComponentModel::ToolboxItem(true)]
        public ref class PlayerSlot : public System::Windows::Forms::UserControl

    {
    public:
        PlayerSlot() {
            InitializeComponent();
        }

        void SetPlayerInfo(String^ name, int balance, String^ status) {
            this->labelaName->Text = name;
            this->labelBalanse->Text = "$" + balance.ToString();
            this->status->Text = status;
        }


    private: System::Windows::Forms::Label^ labelaName;
    private: System::Windows::Forms::Label^ labelBalanse;
    private: System::Windows::Forms::Label^ status;
    private: System::Windows::Forms::PictureBox^ pictureBox;


        void InitializeComponent(void) {
            System::ComponentModel::ComponentResourceManager^ resources = (gcnew System::ComponentModel::ComponentResourceManager(PlayerSlot::typeid));
            this->labelaName = (gcnew System::Windows::Forms::Label());
            this->labelBalanse = (gcnew System::Windows::Forms::Label());
            this->status = (gcnew System::Windows::Forms::Label());
            this->pictureBox = (gcnew System::Windows::Forms::PictureBox());
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox))->BeginInit();
            this->SuspendLayout();
            // 
            // labelaName
            // 
            this->labelaName->AutoSize = true;
            this->labelaName->Font = (gcnew System::Drawing::Font(L"Times New Roman", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
                static_cast<System::Byte>(204)));
            this->labelaName->ForeColor = System::Drawing::SystemColors::ButtonHighlight;
            this->labelaName->Location = System::Drawing::Point(14, 103);
            this->labelaName->Name = L"labelaName";
            this->labelaName->Size = System::Drawing::Size(65, 23);
            this->labelaName->TabIndex = 0;
            this->labelaName->Text = L"Player";
            // 
            // labelBalanse
            // 
            this->labelBalanse->AutoSize = true;
            this->labelBalanse->ForeColor = System::Drawing::SystemColors::ButtonHighlight;
            this->labelBalanse->Location = System::Drawing::Point(15, 126);
            this->labelBalanse->Name = L"labelBalanse";
            this->labelBalanse->Size = System::Drawing::Size(21, 16);
            this->labelBalanse->TabIndex = 1;
            this->labelBalanse->Text = L"$0";
            // 
            // status
            // 
            this->status->AutoSize = true;
            this->status->ForeColor = System::Drawing::SystemColors::ButtonHighlight;
            this->status->Location = System::Drawing::Point(15, 142);
            this->status->Name = L"status";
            this->status->Size = System::Drawing::Size(0, 16);
            this->status->TabIndex = 2;
            // 
            // pictureBox
            // 
            this->pictureBox->BackColor = System::Drawing::Color::Transparent;
            this->pictureBox->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox.Image")));
            this->pictureBox->Location = System::Drawing::Point(18, 3);
            this->pictureBox->Name = L"pictureBox";
            this->pictureBox->Size = System::Drawing::Size(107, 99);
            this->pictureBox->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
            this->pictureBox->TabIndex = 3;
            this->pictureBox->TabStop = false;
            // 
            // PlayerSlot
            // 
            this->BackColor = System::Drawing::Color::Black;
            this->Controls->Add(this->pictureBox);
            this->Controls->Add(this->status);
            this->Controls->Add(this->labelBalanse);
            this->Controls->Add(this->labelaName);
            this->Name = L"PlayerSlot";
            this->Size = System::Drawing::Size(157, 172);
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox))->EndInit();
            this->ResumeLayout(false);
            this->PerformLayout();

        }

    };

}
