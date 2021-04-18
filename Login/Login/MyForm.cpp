#include "MyForm.h"
#include <iostream>
#include "MyForm.h"
using namespace System;
using namespace System::Windows::Forms;
[STAThread]
void main(array<String^>^ args)
{
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);
	Login::MyForm form;
	Application::Run(% form);
	std::cout << "hello world";
}