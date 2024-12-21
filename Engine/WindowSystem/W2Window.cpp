#include "W2Window.h"

#include "../resource.h"

#include <sstream>

//~ Static Declarations
W2Window::W2WindowClass W2Window::W2WindowClass::m_wndClass;

#pragma region W2Window_Implemention

W2Window::W2Window(RECT rect, const char* title)
	: m_rect(rect)
{
	if (AdjustWindowRect(&m_rect, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE) == 0)
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

void W2Window::SetTitle(const std::string& newName)
{
	if (SetWindowText(m_hWnd, newName.c_str()) == 0)
	{
		throw W2WND_LAST_EXCEPT();
	}
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
		case WM_KILLFOCUS:
		{
			Keyboard.ClearState();
			Mouse.Flush();
			return S_OK;
		}
#pragma region Keyboard_Input
		case WM_SYSKEYDOWN:
		case WM_KEYDOWN:
		{
			if (!(lParam & 0x40000000) || Keyboard.AutoRepeatIsEnabled())
			{
				Keyboard.OnKeyPressed(static_cast<unsigned char>(wParam));
			}
			return S_OK;
		}
		case WM_SYSKEYUP:
		case WM_KEYUP:
		{
			Keyboard.OnKeyReleased(static_cast<unsigned char>(wParam));
			return S_OK;
		}
		case WM_CHAR:
		{
			Keyboard.OnChar(static_cast<unsigned char>(wParam));
			return S_OK;
		}
#pragma endregion Keyboard_Input

#pragma region Mouse_Input

		case WM_MOUSEMOVE:
		{
			const POINTS pt = MAKEPOINTS(lParam);
			if (pt.x >= 0 && pt.x < (m_rect.right - m_rect.left) 
				&& pt.y >= 0 && pt.y < (m_rect.bottom - m_rect.top))
			{
				Mouse.OnMouseMove(pt.x, pt.y);
				if (!Mouse.IsInWindow())
				{
					SetCapture(hWnd);
					Mouse.OnMouseEnter();
				}
			}
			else
			{
				if (wParam & (MK_LBUTTON | MK_RBUTTON))
				{
					Mouse.OnMouseMove(pt.x, pt.y);
				}
				else
				{
					ReleaseCapture();
					Mouse.OnMouseLeave();
				}
			}
			return S_OK;
		}
		case WM_LBUTTONDOWN:
		{
			const POINTS pt = MAKEPOINTS(lParam);
			Mouse.OnLeftPressed(pt.x, pt.y);
			return S_OK;
		}
		case WM_RBUTTONDOWN:
		{
			const POINTS pt = MAKEPOINTS(lParam);
			Mouse.OnRightPressed(pt.x, pt.y);
			return S_OK;
		}
		case WM_LBUTTONUP:
		{
			const POINTS pt = MAKEPOINTS(lParam);
			Mouse.OnLeftReleased(pt.x, pt.y);
			return S_OK;
		}
		case WM_RBUTTONUP:
		{
			const POINTS pt = MAKEPOINTS(lParam);
			Mouse.OnRightReleased(pt.x, pt.y);
			return S_OK;
		}
		case WM_MOUSEWHEEL:
		{
			const POINTS pt = MAKEPOINTS(lParam);
			const int delta = GET_WHEEL_DELTA_WPARAM(wParam);
			Mouse.OnWheelDelta(pt.x, pt.y, delta);
		}
#pragma endregion Mouse_Input
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
	wc.hIcon = static_cast<HICON>(LoadImage(m_hInstance, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 16, 16, 0));
	wc.hCursor = static_cast<HCURSOR>(LoadImage(m_hInstance, MAKEINTRESOURCE(IDI_ICON2), IMAGE_ICON, 48, 48, 0));
	wc.hbrBackground = nullptr;
	wc.lpszClassName = GetName();
	wc.lpszMenuName = nullptr;
	wc.hIconSm = static_cast<HICON>(LoadImage(m_hInstance, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 32, 32, 0));;

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
