#include "W2Window.h"

//~ Static Declarations
W2Window::W2WindowClass W2Window::W2WindowClass::m_wndClass;

#pragma region W2Window_Implemention

W2Window::W2Window(RECT rect, const char* title) noexcept
	: m_rect(rect)
{
	AdjustWindowRect(&m_rect, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE);

	//~ Creates window
	m_hWnd = CreateWindow(
		W2WindowClass::GetName(),
		title,
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT,
		m_rect.right - m_rect.left, m_rect.bottom - m_rect.top,
		nullptr, nullptr, W2WindowClass::GetInstance(), this
	);

	ShowWindow(m_hWnd, SW_SHOWDEFAULT);
}

W2Window::~W2Window()
{
	DestroyWindow(m_hWnd);
}

LRESULT W2Window::HandleMsgSetup(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) noexcept
{
	if (message == WM_NCCREATE)
	{
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		W2Window* const pWnd = static_cast<W2Window*>(pCreate->lpCreateParams);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&W2Window::HandleMsgThunk));
		return pWnd->HandleMsg(hWnd, message, wParam, lParam);
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT W2Window::HandleMsgThunk(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) noexcept
{
	// Retrieve pointer and forward message to window class
	W2Window* const pWnd = reinterpret_cast<W2Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	return pWnd->HandleMsg(hWnd, message, wParam, lParam);
}

LRESULT W2Window::HandleMsg(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) noexcept
{
	switch (message)
	{
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return S_OK;
	}
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}
#pragma endregion

#pragma region W2WindowClass_Implemention
W2Window::W2WindowClass::W2WindowClass() noexcept
	: m_hInstance(GetModuleHandle(nullptr))
{
	WNDCLASSEX wc{};
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = HandleMsgSetup;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetInstance();
	wc.hIcon = nullptr;
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszClassName = GetName();
	wc.lpszMenuName = nullptr;
	wc.hIconSm = nullptr;

	RegisterClassEx(&wc);
}

const char* W2Window::W2WindowClass::GetName() noexcept
{
	return m_wndClassName;
}

HINSTANCE W2Window::W2WindowClass::GetInstance() noexcept
{
	return m_wndClass.m_hInstance;
}

W2Window::W2WindowClass::~W2WindowClass()
{
	UnregisterClass(m_wndClassName, GetInstance());
}
#pragma endregion
