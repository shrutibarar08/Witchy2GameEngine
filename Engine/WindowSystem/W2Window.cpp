#include "W2Window.h"

#include <sstream>

//~ Static Declarations
W2Window::W2WindowClass W2Window::W2WindowClass::m_wndClass;

#pragma region W2Window_Implemention

W2Window::W2Window(RECT rect, const char* title)
	: m_rect(rect)
{
	if (FAILED(AdjustWindowRect(&m_rect, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE)))
	{
		throw W2WND_LAST_EXCEPT();
	}

	//~ Creates window
	m_hWnd = CreateWindow(
		W2WindowClass::GetName(),
		title,
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT,
		m_rect.right - m_rect.left, m_rect.bottom - m_rect.top,
		nullptr, nullptr, W2WindowClass::GetInstance(), this
	);

	if (m_hWnd == nullptr)
	{
		throw W2WND_LAST_EXCEPT();
	}

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

#pragma region W2Window_Exception_Implemention

W2Window::Exception::Exception(int line, const char* file, HRESULT hr) noexcept
	: W2Exception(line, file), m_hr(hr)
{}

const char* W2Window::Exception::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] " << GetErrorCode() << std::endl
		<< "[Description] " << GetErrorString() << std::endl
		<< GetOriginString();

	m_whatBuffer = oss.str();
	return m_whatBuffer.c_str();
}

const char* W2Window::Exception::GetType() const noexcept
{
	return "W2Window Exception";
}

HRESULT W2Window::Exception::GetErrorCode() const noexcept
{
	return m_hr;
}

std::string W2Window::Exception::GetErrorString() const noexcept
{
	return TranslateErrorCode(m_hr);
}

std::string W2Window::Exception::TranslateErrorCode(HRESULT hr) noexcept
{
	char* pMsgBuf = nullptr;
	DWORD nMsgLen = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPSTR>(&pMsgBuf), 0, nullptr
	);

	if (nMsgLen == 0)
	{
		return "Unidentified error code!.";
	}

	std::string errorString = pMsgBuf;
	LocalFree(pMsgBuf);

	return errorString;
}

#pragma endregion
