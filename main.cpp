#include <Windows.h>

#include "Engine/WindowSystem/W2Window.h"


int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	RECT rt{ 0, 0, 1280, 720 };
	W2Window wnd{ rt, "Witchy Game Engine" };

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}
