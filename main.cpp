#include <Windows.h>

#include "WindowSystem/W2Window.h"
#include "Exceptions/W2Exception.h"


int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	try
	{
		RECT rt{ 0, 0, 1280, 720 };
		W2Window wnd{ rt, "Witchy Game Engine" };

		MSG msg;
		while (GetMessage(&msg, NULL, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (wnd.Keyboard.KeyIsPressed(VK_MENU))
			{
				MessageBox(nullptr, "Space is pressed", "Space", MB_OK);
			}
		}
		return msg.wParam;
	}
	catch (const W2Exception& e)
	{
		MessageBox(nullptr, e.what(), e.GetType(), MB_OK | MB_ICONEXCLAMATION);
	}
	catch (const std::exception& e)
	{
		MessageBox(nullptr, e.what(), "Standard Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	catch (...)
	{
		MessageBox(nullptr, "No details available", "Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
	}

	return E_FAIL;
}
