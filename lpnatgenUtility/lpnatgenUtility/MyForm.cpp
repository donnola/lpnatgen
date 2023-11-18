#include "MyForm.h"

#include <Windows.h>
using namespace lpnatgenUtility; // Название проекта
[STAThread]
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
  Application::EnableVisualStyles();
  Application::SetCompatibleTextRenderingDefault(false);
  Application::Run(gcnew MyForm);
  return 0;
}

