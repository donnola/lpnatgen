#include "MyForm.h"

#include <Windows.h>
using namespace lpnatgenUtilityTemp; // Название проекта
[STAThread]
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
  Application::EnableVisualStyles();
  Application::SetCompatibleTextRenderingDefault(false);
  Application::Run(gcnew MyForm);
  return 0;
}

