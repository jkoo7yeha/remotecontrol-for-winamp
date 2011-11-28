#pragma once

namespace gen_RemoteControl {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Zusammenfassung für UI
	/// </summary>
	public ref class UI : public System::Windows::Forms::Form
	{
	public:
		UI();

	protected:
		/// <summary>
		/// Verwendete Ressourcen bereinigen.
		/// </summary>
		~UI();

	public: System::Windows::Forms::Label^  statusLabel;
	public: System::Windows::Forms::CheckBox^  autoRestartCheckBox;
	public: System::Windows::Forms::TextBox^  logTextBox;
	public: System::Windows::Forms::Button^  saveButton;
	private: System::Windows::Forms::Label^  staticIPLabel;
	public: System::Windows::Forms::CheckBox^  showConfigOnStartupCheckBox;
	private: System::Windows::Forms::GroupBox^  logGroupBox;
	public: System::Windows::Forms::CheckBox^  autostartCheckBox;
	public: System::ComponentModel::BackgroundWorker^  backgroundWorker;
	public: System::Windows::Forms::TextBox^  portEditBox;
	public: System::Windows::Forms::Button^  connectButton;
	private: System::Windows::Forms::GroupBox^  settingsGroupBox;
	private: System::Windows::Forms::Label^  staticPortLabel;
	private: System::Windows::Forms::LinkLabel^  linkLabel;
	private: System::Windows::Forms::Label^  staticStatusLabel;
	public: System::Windows::Forms::Label^  ipLabel;
	private: System::Windows::Forms::Label^  copyrightLabel;
	public: System::Windows::Forms::CheckBox^  keepAliveCheckBox;
	private: System::Windows::Forms::Label^  newVersionLabel;
	private: System::Windows::Forms::PictureBox^  donatePictureBox;
	private: System::Windows::Forms::Label^  donateLabel;
	private: System::Windows::Forms::Label^  donateLabel2;

	private:
		/// <summary>
		/// Erforderliche Designervariable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Erforderliche Methode für die Designerunterstützung.
		/// Der Inhalt der Methode darf nicht mit dem Code-Editor geändert werden.
		/// </summary>
		void InitializeComponent(void)
		{
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(UI::typeid));
			this->statusLabel = (gcnew System::Windows::Forms::Label());
			this->autoRestartCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->logTextBox = (gcnew System::Windows::Forms::TextBox());
			this->saveButton = (gcnew System::Windows::Forms::Button());
			this->staticIPLabel = (gcnew System::Windows::Forms::Label());
			this->showConfigOnStartupCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->logGroupBox = (gcnew System::Windows::Forms::GroupBox());
			this->autostartCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->backgroundWorker = (gcnew System::ComponentModel::BackgroundWorker());
			this->portEditBox = (gcnew System::Windows::Forms::TextBox());
			this->connectButton = (gcnew System::Windows::Forms::Button());
			this->settingsGroupBox = (gcnew System::Windows::Forms::GroupBox());
			this->keepAliveCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->staticPortLabel = (gcnew System::Windows::Forms::Label());
			this->linkLabel = (gcnew System::Windows::Forms::LinkLabel());
			this->staticStatusLabel = (gcnew System::Windows::Forms::Label());
			this->ipLabel = (gcnew System::Windows::Forms::Label());
			this->copyrightLabel = (gcnew System::Windows::Forms::Label());
			this->newVersionLabel = (gcnew System::Windows::Forms::Label());
			this->donatePictureBox = (gcnew System::Windows::Forms::PictureBox());
			this->donateLabel = (gcnew System::Windows::Forms::Label());
			this->donateLabel2 = (gcnew System::Windows::Forms::Label());
			this->logGroupBox->SuspendLayout();
			this->settingsGroupBox->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->donatePictureBox))->BeginInit();
			this->SuspendLayout();
			// 
			// statusLabel
			// 
			this->statusLabel->AutoSize = true;
			this->statusLabel->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->statusLabel->Location = System::Drawing::Point(57, 9);
			this->statusLabel->Name = L"statusLabel";
			this->statusLabel->Size = System::Drawing::Size(73, 13);
			this->statusLabel->TabIndex = 112;
			this->statusLabel->Text = L"Disconnected";
			// 
			// autoRestartCheckBox
			// 
			this->autoRestartCheckBox->AutoSize = true;
			this->autoRestartCheckBox->Location = System::Drawing::Point(11, 90);
			this->autoRestartCheckBox->Name = L"autoRestartCheckBox";
			this->autoRestartCheckBox->Size = System::Drawing::Size(229, 17);
			this->autoRestartCheckBox->TabIndex = 10;
			this->autoRestartCheckBox->Text = L"Auto restart server when client disconnects";
			this->autoRestartCheckBox->UseVisualStyleBackColor = true;
			this->autoRestartCheckBox->CheckedChanged += gcnew System::EventHandler(this, &UI::autoRestartCheckBox_CheckedChanged);
			// 
			// logTextBox
			// 
			this->logTextBox->Location = System::Drawing::Point(6, 15);
			this->logTextBox->Multiline = true;
			this->logTextBox->Name = L"logTextBox";
			this->logTextBox->ScrollBars = System::Windows::Forms::ScrollBars::Vertical;
			this->logTextBox->Size = System::Drawing::Size(278, 142);
			this->logTextBox->TabIndex = 3;
			this->logTextBox->TextChanged += gcnew System::EventHandler(this, &UI::logTextBox_TextChanged);
			// 
			// saveButton
			// 
			this->saveButton->Enabled = false;
			this->saveButton->Location = System::Drawing::Point(6, 134);
			this->saveButton->Name = L"saveButton";
			this->saveButton->Size = System::Drawing::Size(75, 23);
			this->saveButton->TabIndex = 9;
			this->saveButton->Text = L"Save";
			this->saveButton->UseVisualStyleBackColor = true;
			this->saveButton->Click += gcnew System::EventHandler(this, &UI::saveButton_Click);
			// 
			// staticIPLabel
			// 
			this->staticIPLabel->AutoSize = true;
			this->staticIPLabel->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->staticIPLabel->Location = System::Drawing::Point(384, 9);
			this->staticIPLabel->Name = L"staticIPLabel";
			this->staticIPLabel->Size = System::Drawing::Size(89, 13);
			this->staticIPLabel->TabIndex = 115;
			this->staticIPLabel->Text = L"Local IP address:";
			// 
			// showConfigOnStartupCheckBox
			// 
			this->showConfigOnStartupCheckBox->AutoSize = true;
			this->showConfigOnStartupCheckBox->Location = System::Drawing::Point(11, 67);
			this->showConfigOnStartupCheckBox->Name = L"showConfigOnStartupCheckBox";
			this->showConfigOnStartupCheckBox->Size = System::Drawing::Size(210, 17);
			this->showConfigOnStartupCheckBox->TabIndex = 7;
			this->showConfigOnStartupCheckBox->Text = L"Show this window when Winamp starts";
			this->showConfigOnStartupCheckBox->UseVisualStyleBackColor = true;
			this->showConfigOnStartupCheckBox->CheckedChanged += gcnew System::EventHandler(this, &UI::showConfigOnStartupCheckBox_CheckedChanged);
			// 
			// logGroupBox
			// 
			this->logGroupBox->Controls->Add(this->logTextBox);
			this->logGroupBox->Location = System::Drawing::Point(301, 36);
			this->logGroupBox->Name = L"logGroupBox";
			this->logGroupBox->Size = System::Drawing::Size(290, 163);
			this->logGroupBox->TabIndex = 116;
			this->logGroupBox->TabStop = false;
			this->logGroupBox->Text = L"Log";
			// 
			// autostartCheckBox
			// 
			this->autostartCheckBox->AutoSize = true;
			this->autostartCheckBox->Location = System::Drawing::Point(11, 44);
			this->autostartCheckBox->Name = L"autostartCheckBox";
			this->autostartCheckBox->Size = System::Drawing::Size(144, 17);
			this->autostartCheckBox->TabIndex = 6;
			this->autostartCheckBox->Text = L"Start server with Winamp";
			this->autostartCheckBox->UseVisualStyleBackColor = true;
			this->autostartCheckBox->CheckedChanged += gcnew System::EventHandler(this, &UI::autostartCheckBox_CheckedChanged);
			// 
			// backgroundWorker
			// 
			this->backgroundWorker->RunWorkerCompleted += gcnew System::ComponentModel::RunWorkerCompletedEventHandler(this, &UI::backgroundWorker_RunWorkerCompleted);
			// 
			// portEditBox
			// 
			this->portEditBox->Location = System::Drawing::Point(55, 18);
			this->portEditBox->MaxLength = 5;
			this->portEditBox->Name = L"portEditBox";
			this->portEditBox->Size = System::Drawing::Size(106, 20);
			this->portEditBox->TabIndex = 8;
			this->portEditBox->TextChanged += gcnew System::EventHandler(this, &UI::portEditBox_TextChanged);
			// 
			// connectButton
			// 
			this->connectButton->Location = System::Drawing::Point(303, 4);
			this->connectButton->Name = L"connectButton";
			this->connectButton->Size = System::Drawing::Size(75, 23);
			this->connectButton->TabIndex = 114;
			this->connectButton->Text = L"Start";
			this->connectButton->UseVisualStyleBackColor = true;
			this->connectButton->Click += gcnew System::EventHandler(this, &UI::connectButton_Click);
			// 
			// settingsGroupBox
			// 
			this->settingsGroupBox->Controls->Add(this->keepAliveCheckBox);
			this->settingsGroupBox->Controls->Add(this->autoRestartCheckBox);
			this->settingsGroupBox->Controls->Add(this->saveButton);
			this->settingsGroupBox->Controls->Add(this->showConfigOnStartupCheckBox);
			this->settingsGroupBox->Controls->Add(this->autostartCheckBox);
			this->settingsGroupBox->Controls->Add(this->portEditBox);
			this->settingsGroupBox->Controls->Add(this->staticPortLabel);
			this->settingsGroupBox->Location = System::Drawing::Point(7, 36);
			this->settingsGroupBox->Name = L"settingsGroupBox";
			this->settingsGroupBox->Size = System::Drawing::Size(290, 163);
			this->settingsGroupBox->TabIndex = 113;
			this->settingsGroupBox->TabStop = false;
			this->settingsGroupBox->Text = L"Settings";
			// 
			// keepAliveCheckBox
			// 
			this->keepAliveCheckBox->AutoSize = true;
			this->keepAliveCheckBox->Location = System::Drawing::Point(11, 113);
			this->keepAliveCheckBox->Name = L"keepAliveCheckBox";
			this->keepAliveCheckBox->Size = System::Drawing::Size(160, 17);
			this->keepAliveCheckBox->TabIndex = 11;
			this->keepAliveCheckBox->Text = L"enable keep-alive-messages";
			this->keepAliveCheckBox->UseVisualStyleBackColor = true;
			this->keepAliveCheckBox->CheckedChanged += gcnew System::EventHandler(this, &UI::keepAliveCheckBox_CheckedChanged);
			// 
			// staticPortLabel
			// 
			this->staticPortLabel->AutoSize = true;
			this->staticPortLabel->Location = System::Drawing::Point(8, 21);
			this->staticPortLabel->Name = L"staticPortLabel";
			this->staticPortLabel->Size = System::Drawing::Size(26, 13);
			this->staticPortLabel->TabIndex = 5;
			this->staticPortLabel->Text = L"Port";
			// 
			// linkLabel
			// 
			this->linkLabel->AutoSize = true;
			this->linkLabel->Location = System::Drawing::Point(332, 217);
			this->linkLabel->Name = L"linkLabel";
			this->linkLabel->Size = System::Drawing::Size(259, 13);
			this->linkLabel->TabIndex = 117;
			this->linkLabel->TabStop = true;
			this->linkLabel->Text = L"http://code.google.com/p/remotecontrol-for-winamp/";
			this->linkLabel->VisitedLinkColor = System::Drawing::Color::Blue;
			this->linkLabel->LinkClicked += gcnew System::Windows::Forms::LinkLabelLinkClickedEventHandler(this, &UI::linkLabel_LinkClicked);
			// 
			// staticStatusLabel
			// 
			this->staticStatusLabel->AutoSize = true;
			this->staticStatusLabel->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, 
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->staticStatusLabel->Location = System::Drawing::Point(15, 9);
			this->staticStatusLabel->Name = L"staticStatusLabel";
			this->staticStatusLabel->Size = System::Drawing::Size(40, 13);
			this->staticStatusLabel->TabIndex = 111;
			this->staticStatusLabel->Text = L"Status:";
			// 
			// ipLabel
			// 
			this->ipLabel->AutoSize = true;
			this->ipLabel->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->ipLabel->Location = System::Drawing::Point(477, 9);
			this->ipLabel->Name = L"ipLabel";
			this->ipLabel->Size = System::Drawing::Size(0, 13);
			this->ipLabel->TabIndex = 119;
			// 
			// copyrightLabel
			// 
			this->copyrightLabel->AutoSize = true;
			this->copyrightLabel->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->copyrightLabel->Location = System::Drawing::Point(10, 217);
			this->copyrightLabel->Name = L"copyrightLabel";
			this->copyrightLabel->Size = System::Drawing::Size(212, 13);
			this->copyrightLabel->TabIndex = 118;
			this->copyrightLabel->Text = L"Copyright © 2010 - 2011 Martin Schlodinski";
			// 
			// newVersionLabel
			// 
			this->newVersionLabel->AutoSize = true;
			this->newVersionLabel->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->newVersionLabel->ForeColor = System::Drawing::Color::Red;
			this->newVersionLabel->Location = System::Drawing::Point(384, 202);
			this->newVersionLabel->Name = L"newVersionLabel";
			this->newVersionLabel->Size = System::Drawing::Size(147, 13);
			this->newVersionLabel->TabIndex = 120;
			this->newVersionLabel->Text = L"NEW VERSION AVAILABLE:";
			this->newVersionLabel->Visible = false;
			// 
			// donatePictureBox
			// 
			this->donatePictureBox->ErrorImage = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"donatePictureBox.ErrorImage")));
			this->donatePictureBox->ImageLocation = L"donate.png";
			this->donatePictureBox->InitialImage = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"donatePictureBox.InitialImage")));
			this->donatePictureBox->Location = System::Drawing::Point(163, 202);
			this->donatePictureBox->Name = L"donatePictureBox";
			this->donatePictureBox->Size = System::Drawing::Size(53, 15);
			this->donatePictureBox->TabIndex = 121;
			this->donatePictureBox->TabStop = false;
			this->donatePictureBox->Click += gcnew System::EventHandler(this, &UI::donatePictureBox_Click);
			// 
			// donateLabel
			// 
			this->donateLabel->AutoSize = true;
			this->donateLabel->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->donateLabel->Location = System::Drawing::Point(10, 202);
			this->donateLabel->Name = L"donateLabel";
			this->donateLabel->Size = System::Drawing::Size(139, 13);
			this->donateLabel->TabIndex = 122;
			this->donateLabel->Text = L"If you like this plugin, please";
			// 
			// donateLabel2
			// 
			this->donateLabel2->AutoSize = true;
			this->donateLabel2->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->donateLabel2->Location = System::Drawing::Point(222, 202);
			this->donateLabel2->Name = L"donateLabel2";
			this->donateLabel2->Size = System::Drawing::Size(48, 13);
			this->donateLabel2->TabIndex = 123;
			this->donateLabel2->Text = L"- thanks!";
			// 
			// UI
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(598, 234);
			this->Controls->Add(this->donateLabel2);
			this->Controls->Add(this->donateLabel);
			this->Controls->Add(this->donatePictureBox);
			this->Controls->Add(this->newVersionLabel);
			this->Controls->Add(this->statusLabel);
			this->Controls->Add(this->staticIPLabel);
			this->Controls->Add(this->logGroupBox);
			this->Controls->Add(this->connectButton);
			this->Controls->Add(this->settingsGroupBox);
			this->Controls->Add(this->linkLabel);
			this->Controls->Add(this->staticStatusLabel);
			this->Controls->Add(this->ipLabel);
			this->Controls->Add(this->copyrightLabel);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
			this->Icon = (cli::safe_cast<System::Drawing::Icon^  >(resources->GetObject(L"$this.Icon")));
			this->Name = L"UI";
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &UI::UI_FormClosing);
			this->Load += gcnew System::EventHandler(this, &UI::UI_Load);
			this->logGroupBox->ResumeLayout(false);
			this->logGroupBox->PerformLayout();
			this->settingsGroupBox->ResumeLayout(false);
			this->settingsGroupBox->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->donatePictureBox))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

public: System::Void linkLabel_LinkClicked(System::Object^  sender, System::Windows::Forms::LinkLabelLinkClickedEventArgs^  e);
public: System::Void portEditBox_TextChanged(System::Object^  sender, System::EventArgs^  e);
public: System::Void logTextBox_TextChanged(System::Object^  sender, System::EventArgs^  e);
public: System::Void newVersionFound(String^);

private: System::Void autostartCheckBox_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
private: System::Void showConfigOnStartupCheckBox_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
private: System::Void autoRestartCheckBox_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
private: System::Void keepAliveCheckBox_CheckedChanged(System::Object^  sender, System::EventArgs^  e);

private: System::Void saveButton_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void connectButton_Click(System::Object^  sender, System::EventArgs^  e);

private: System::Void UI_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e);
private: System::Void UI_Load(System::Object^  sender, System::EventArgs^  e);

private: System::Void donatePictureBox_Click(System::Object^  sender, System::EventArgs^  e);

public:	 // delegate needed for thread safe UI operations
		 delegate void SetTextDelegate(System::String^ text);
		 
		 System::Void addLogText(System::String^ text);
		 System::Void setStatusText(System::String^ text);
		 System::Void setButtonText(System::String^ text);
		 System::Void setIP(System::String^ text);

private: System::Void backgroundWorker_RunWorkerCompleted(System::Object^  sender, System::ComponentModel::RunWorkerCompletedEventArgs^  e);

};
}

