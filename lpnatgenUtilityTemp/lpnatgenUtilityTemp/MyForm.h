#pragma once
//#include "lpnatgen.h"
//namespace raylib
//{
//#ifdef __cplusplus
//	extern "C" {
//#endif
//#include "raylib.h"
//#ifdef __cplusplus
//	}
//#endif
//}

namespace lpnatgenUtilityTemp{

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::IO;

	/// <summary>
	/// —водка дл€ MyForm
	/// </summary>
	public ref class MyForm : public System::Windows::Forms::Form
	{
	public:
		MyForm(void)
		{
			InitializeComponent();
			//
			//TODO: добавьте код конструктора
			//
		}

	protected:
		/// <summary>
		/// ќсвободить все используемые ресурсы.
		/// </summary>
		~MyForm()
		{
			if (components)
			{
				delete components;
			}
			//if (obj != nullptr)
			//{
			//	delete obj;
			//}
		}

	private: System::Windows::Forms::Button^ CreateModel;
	private: System::Windows::Forms::ComboBox^ ChooseModel;
	private: System::Windows::Forms::Label^ ModelType;
	private: System::Windows::Forms::Label^ ModelSize;
	private: System::Windows::Forms::CheckBox^ ModelSeed;
	private: System::Windows::Forms::Label^ SavePath;
	private: System::Windows::Forms::Label^ FileName;
	private: System::Windows::Forms::Button^ OpenFolderBrowserDialog;
	private: System::Windows::Forms::TextBox^ FilePath;
	private: System::Windows::Forms::FolderBrowserDialog^ folderBrowserDialog1;
	private: System::Windows::Forms::TextBox^ SeedNumTextBox;
	private: System::Windows::Forms::TextBox^ SizeNumTextBox;
	private: System::Windows::Forms::TextBox^ FileNameTextBox;
	private: System::Windows::Forms::Button^ SaveModel;
	private: System::Windows::Forms::Button^ Test;

	private:
		/// <summary>
		/// ќб€зательна€ переменна€ конструктора.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// “ребуемый метод дл€ поддержки конструктора Ч не измен€йте 
		/// содержимое этого метода с помощью редактора кода.
		/// </summary>
		void InitializeComponent(void)
		{
			this->CreateModel = (gcnew System::Windows::Forms::Button());
			this->ChooseModel = (gcnew System::Windows::Forms::ComboBox());
			this->ModelType = (gcnew System::Windows::Forms::Label());
			this->ModelSize = (gcnew System::Windows::Forms::Label());
			this->ModelSeed = (gcnew System::Windows::Forms::CheckBox());
			this->SavePath = (gcnew System::Windows::Forms::Label());
			this->FileName = (gcnew System::Windows::Forms::Label());
			this->OpenFolderBrowserDialog = (gcnew System::Windows::Forms::Button());
			this->FilePath = (gcnew System::Windows::Forms::TextBox());
			this->folderBrowserDialog1 = (gcnew System::Windows::Forms::FolderBrowserDialog());
			this->SeedNumTextBox = (gcnew System::Windows::Forms::TextBox());
			this->SizeNumTextBox = (gcnew System::Windows::Forms::TextBox());
			this->FileNameTextBox = (gcnew System::Windows::Forms::TextBox());
			this->SaveModel = (gcnew System::Windows::Forms::Button());
			this->Test = (gcnew System::Windows::Forms::Button());
			this->SuspendLayout();
			// 
			// CreateModel
			// 
			this->CreateModel->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->CreateModel->Location = System::Drawing::Point(153, 264);
			this->CreateModel->Name = L"CreateModel";
			this->CreateModel->Size = System::Drawing::Size(101, 39);
			this->CreateModel->TabIndex = 8;
			this->CreateModel->Text = L"Create";
			this->CreateModel->UseVisualStyleBackColor = true;
			this->CreateModel->Click += gcnew System::EventHandler(this, &MyForm::Test_Click);
			// 
			// ChooseModel
			// 
			this->ChooseModel->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->ChooseModel->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 15.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->ChooseModel->FormattingEnabled = true;
			this->ChooseModel->ItemHeight = 25;
			this->ChooseModel->Items->AddRange(gcnew cli::array< System::Object^  >(3) { L"Tree", L"Bush", L"Stone" });
			this->ChooseModel->Location = System::Drawing::Point(139, 31);
			this->ChooseModel->Name = L"ChooseModel";
			this->ChooseModel->Size = System::Drawing::Size(104, 33);
			this->ChooseModel->TabIndex = 1;
			this->ChooseModel->SelectedIndexChanged += gcnew System::EventHandler(this, &MyForm::ChooseModel_SelectedIndexChanged);
			// 
			// ModelType
			// 
			this->ModelType->AutoSize = true;
			this->ModelType->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 15.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->ModelType->Location = System::Drawing::Point(12, 34);
			this->ModelType->Name = L"ModelType";
			this->ModelType->Size = System::Drawing::Size(71, 25);
			this->ModelType->TabIndex = 4;
			this->ModelType->Text = L"Model";
			// 
			// ModelSize
			// 
			this->ModelSize->AutoSize = true;
			this->ModelSize->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 15.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->ModelSize->Location = System::Drawing::Point(12, 81);
			this->ModelSize->Name = L"ModelSize";
			this->ModelSize->Size = System::Drawing::Size(54, 25);
			this->ModelSize->TabIndex = 5;
			this->ModelSize->Text = L"Size";
			// 
			// ModelSeed
			// 
			this->ModelSeed->AutoSize = true;
			this->ModelSeed->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 15.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->ModelSeed->Location = System::Drawing::Point(17, 127);
			this->ModelSeed->Name = L"ModelSeed";
			this->ModelSeed->Size = System::Drawing::Size(81, 29);
			this->ModelSeed->TabIndex = 3;
			this->ModelSeed->Text = L"Seed";
			this->ModelSeed->UseVisualStyleBackColor = true;
			this->ModelSeed->CheckedChanged += gcnew System::EventHandler(this, &MyForm::ModelSeed_CheckedChanged);
			// 
			// SavePath
			// 
			this->SavePath->AutoSize = true;
			this->SavePath->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 15.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->SavePath->Location = System::Drawing::Point(12, 226);
			this->SavePath->Name = L"SavePath";
			this->SavePath->Size = System::Drawing::Size(111, 25);
			this->SavePath->TabIndex = 7;
			this->SavePath->Text = L"Save Path";
			// 
			// FileName
			// 
			this->FileName->AutoSize = true;
			this->FileName->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 15.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->FileName->Location = System::Drawing::Point(12, 178);
			this->FileName->Name = L"FileName";
			this->FileName->Size = System::Drawing::Size(109, 25);
			this->FileName->TabIndex = 8;
			this->FileName->Text = L"File Name";
			// 
			// OpenFolderBrowserDialog
			// 
			this->OpenFolderBrowserDialog->Location = System::Drawing::Point(316, 222);
			this->OpenFolderBrowserDialog->Name = L"OpenFolderBrowserDialog";
			this->OpenFolderBrowserDialog->Size = System::Drawing::Size(34, 32);
			this->OpenFolderBrowserDialog->TabIndex = 7;
			this->OpenFolderBrowserDialog->Text = L"...";
			this->OpenFolderBrowserDialog->UseVisualStyleBackColor = true;
			this->OpenFolderBrowserDialog->Click += gcnew System::EventHandler(this, &MyForm::OpenFolderBrowserDialog_Click);
			// 
			// FilePath
			// 
			this->FilePath->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 15.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->FilePath->Location = System::Drawing::Point(139, 223);
			this->FilePath->Name = L"FilePath";
			this->FilePath->Size = System::Drawing::Size(160, 31);
			this->FilePath->TabIndex = 6;
			// 
			// SeedNumTextBox
			// 
			this->SeedNumTextBox->Enabled = false;
			this->SeedNumTextBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 15.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->SeedNumTextBox->Location = System::Drawing::Point(139, 125);
			this->SeedNumTextBox->Name = L"SeedNumTextBox";
			this->SeedNumTextBox->Size = System::Drawing::Size(90, 31);
			this->SeedNumTextBox->TabIndex = 4;
			// 
			// SizeNumTextBox
			// 
			this->SizeNumTextBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 15.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->SizeNumTextBox->Location = System::Drawing::Point(139, 78);
			this->SizeNumTextBox->Name = L"SizeNumTextBox";
			this->SizeNumTextBox->Size = System::Drawing::Size(90, 31);
			this->SizeNumTextBox->TabIndex = 2;
			// 
			// FileNameTextBox
			// 
			this->FileNameTextBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 15.75F, System::Drawing::FontStyle::Regular,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(204)));
			this->FileNameTextBox->Location = System::Drawing::Point(139, 175);
			this->FileNameTextBox->Name = L"FileNameTextBox";
			this->FileNameTextBox->Size = System::Drawing::Size(160, 31);
			this->FileNameTextBox->TabIndex = 5;
			// 
			// SaveModel
			// 
			this->SaveModel->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->SaveModel->Location = System::Drawing::Point(260, 264);
			this->SaveModel->Name = L"SaveModel";
			this->SaveModel->Size = System::Drawing::Size(101, 39);
			this->SaveModel->TabIndex = 9;
			this->SaveModel->Text = L"Save";
			this->SaveModel->UseVisualStyleBackColor = true;
			// 
			// Test
			// 
			this->Test->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->Test->Location = System::Drawing::Point(87, 264);
			this->Test->Name = L"Test";
			this->Test->Size = System::Drawing::Size(60, 39);
			this->Test->TabIndex = 10;
			this->Test->Text = L"Test";
			this->Test->UseVisualStyleBackColor = true;
			// 
			// MyForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(366, 315);
			this->Controls->Add(this->Test);
			this->Controls->Add(this->SaveModel);
			this->Controls->Add(this->FileNameTextBox);
			this->Controls->Add(this->SizeNumTextBox);
			this->Controls->Add(this->SeedNumTextBox);
			this->Controls->Add(this->FilePath);
			this->Controls->Add(this->OpenFolderBrowserDialog);
			this->Controls->Add(this->FileName);
			this->Controls->Add(this->SavePath);
			this->Controls->Add(this->ModelSeed);
			this->Controls->Add(this->ModelSize);
			this->Controls->Add(this->ModelType);
			this->Controls->Add(this->ChooseModel);
			this->Controls->Add(this->CreateModel);
			this->MaximizeBox = false;
			this->Name = L"MyForm";
			this->Text = L"3DCreator";
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	//private: lpng::GenerateObject* obj = nullptr;

	private: System::Void CreateModel_Click(System::Object^ sender, System::EventArgs^ e)
	{
		//delete obj;
	}
	private: System::Void SaveModel_Click(System::Object^ sender, System::EventArgs^ e)
	{
		//if (obj != nullptr)
		//	obj->SaveModel();
	}
	private: System::Void Test_Click(System::Object^ sender, System::EventArgs^ e)
	{
		//delete obj;
		//obj = new lpng::GenerateObjectTest();
		//obj->Generate();

		//raylib::InitWindow(800, 450, "raylib [core] example - basic window");

		//while (!raylib::WindowShouldClose())
		//{
		//	raylib::BeginDrawing();
		//	raylib::ClearBackground(raylib::RAYWHITE);
		//	raylib::DrawText("Congrats! You created your first window!", 190, 200, 20, raylib::LIGHTGRAY);
		//	raylib::EndDrawing();
		//}

		//raylib::CloseWindow();
		//obj->ShowModel();
	}
	private: System::Void ChooseModel_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e)
	{}
  private: System::Void ModelSeed_CheckedChanged(System::Object^ sender, System::EventArgs^ e)
  {
		SeedNumTextBox->Enabled = ModelSeed->Checked ? true : false;
	}
  private: System::Void OpenFolderBrowserDialog_Click(System::Object^ sender, System::EventArgs^ e)
  {
		if (folderBrowserDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK)
		{
			FilePath->Text = folderBrowserDialog1->SelectedPath;
		}
	}
};
}
