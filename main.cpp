#include "Application/TestApplication.h"
#include <iostream>
#include <fstream>

void EnableTerminal()
{
    // Allocate a new console for the application
    if (AllocConsole())
    {
        // Redirect standard output to the console
        FILE* newStdout = nullptr;
        freopen_s(&newStdout, "CONOUT$", "w", stdout);

        // Redirect standard input to the console
        FILE* newStdin = nullptr;
        freopen_s(&newStdin, "CONIN$", "r", stdin);

        // Redirect standard error to the console
        FILE* newStderr = nullptr;
        freopen_s(&newStderr, "CONOUT$", "w", stderr);

        // Optional: Set the console title
        SetConsoleTitle("Debug Console");

        // Optional: Print an initialization message
        std::cout << "Terminal initialized successfully!" << std::endl;
    }
    else
    {
        MessageBox(nullptr, "Failed to allocate console.", "Error", MB_ICONERROR);
    }
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	try
	{
#ifdef _DEBUG
        EnableTerminal();
#endif
		return TestApplication{}.Execute();
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
