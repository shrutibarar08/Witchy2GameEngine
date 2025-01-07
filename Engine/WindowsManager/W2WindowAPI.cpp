#include "W2WindowAPI.h"
#include "ImuGui/imgui_impl_win32.h"

#include <sstream>

#include "../resource.h"
#include "GuiManager/W2GuiAPI.h"


//~ Static Declarations
W2WindowAPI* W2WindowAPI::m_instance = nullptr;
W2WindowAPI::W2WindowClass W2WindowAPI::W2WindowClass::m_wndClass;

#pragma region W2Window_Implemention

W2WindowAPI::W2WindowAPI(RECT rect, const char* title)
	: m_rect(rect),
      m_titleName(title)
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
	RAWINPUTDEVICE rawDevice;
	rawDevice.usUsagePage = 0x01;
	rawDevice.usUsage = 0x02;
	rawDevice.dwFlags = 0;
	rawDevice.hwndTarget = nullptr;
	if (RegisterRawInputDevices(&rawDevice, 1, sizeof(rawDevice)) == FALSE)
	{
		throw W2WND_LAST_EXCEPT();
	}
}

W2WindowAPI::~W2WindowAPI()
{
	ImGui_ImplWin32_Shutdown();
	DestroyWindow(m_hWnd);
}

void W2WindowAPI::Init(RECT rect, const char* title)
{
	if (m_instance == nullptr)
	{
		m_instance = new W2WindowAPI(rect, title);
	}
}

W2WindowAPI* W2WindowAPI::Get()
{
	if (m_instance != nullptr) return m_instance;

	throw std::logic_error("Called To get W2WindowAPI when it is not Initialized!");
}

void W2WindowAPI::Release()
{
	if (m_instance)
	{
		delete m_instance;
		m_instance = nullptr;
	}
}

const char* W2WindowAPI::GetTitleName() const noexcept
{
	return m_titleName;
}

void W2WindowAPI::SetTitleName(const std::string& newName) const
{
	if (SetWindowText(m_hWnd, newName.c_str()) == 0)
	{
		throw W2WND_LAST_EXCEPT();
	}
}

void W2WindowAPI::EnableCursor()
{
	m_bCursorEnable = true;
	Mouse.ShowCursor();
	Mouse.FreeCursor();
	W2GuiAPI::Get()->EnableMouseInput();
}

void W2WindowAPI::DisableCursor()
{
	m_bCursorEnable = false;
	Mouse.HideCursor();
	Mouse.ConfineCursor(m_hWnd);
	W2GuiAPI::Get()->DisableMouseInput();
}

HWND W2WindowAPI::GetHandleWindow() const noexcept
{
	return m_hWnd;
}

std::optional<int> W2WindowAPI::ProcessMessages() noexcept
{
	MSG msg{};
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			return static_cast<int>(msg.wParam);
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return {};
}

LRESULT W2WindowAPI::HandleMsgSetup(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) noexcept
{
	if (message == WM_NCCREATE)
	{
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		W2WindowAPI* const pWnd = static_cast<W2WindowAPI*>(pCreate->lpCreateParams);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&W2WindowAPI::HandleMsgThunk));
		return pWnd->HandleMsg(hWnd, message, wParam, lParam);
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT W2WindowAPI::HandleMsgThunk(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) noexcept
{
	// Retrieve pointer and forward message to window class
	W2WindowAPI* const pWnd = reinterpret_cast<W2WindowAPI*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	return pWnd->HandleMsg(hWnd, message, wParam, lParam);
}

LRESULT W2WindowAPI::HandleMsg(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) noexcept
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam)) return S_OK;

	switch (message)
	{
#pragma region Events
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			return S_OK;
		}
		case WM_KILLFOCUS:
		{
			// TODO: Add Pause event or Callback
			Keyboard.ClearState();
			Mouse.Flush();
			return S_OK;
		}
		case WM_SETFOCUS:
		{
			// TODO: Add Resume Event or Callback
		}
		case WM_ACTIVATE:
		{
			if (m_bCursorEnable)
			{
				if (wParam & WA_ACTIVE)
				{
					Mouse.ConfineCursor(m_hWnd);
					Mouse.HideCursor();
				}
				else
				{
					Mouse.FreeCursor();
					Mouse.ShowCursor();
				}
			}
			return S_OK;
		}
#pragma endregion

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

			if (!m_bCursorEnable)
			{
				if (!Mouse.IsInWindow())
				{
					SetCapture(hWnd);
					Mouse.OnMouseEnter();
					Mouse.HideCursor();
				}
				break;
			}

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
			SetForegroundWindow(m_hWnd);
			if (!m_bCursorEnable)
			{
				Mouse.ConfineCursor(m_hWnd);
				Mouse.HideCursor();
			}
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
		case WM_INPUT:
		{
			UINT size;
			if (GetRawInputData(
				reinterpret_cast<HRAWINPUT>(lParam),
				RID_INPUT,
				nullptr,
				&size,
				sizeof(RAWINPUTHEADER)) == -1)
			{
				break;
			}
			m_rawBuffer.resize(size);
			if (GetRawInputData(
				reinterpret_cast<HRAWINPUT>(lParam),
				RID_INPUT,
				m_rawBuffer.data(),
				&size,
				sizeof(RAWINPUTHEADER)) != size)
			{
				break;
			}
			auto& ri = reinterpret_cast<const RAWINPUT&>(*m_rawBuffer.data());
			if (ri.header.dwType == RIM_TYPEMOUSE &&
				(ri.data.mouse.lLastX != 0 || ri.data.mouse.lLastY != 0))
			{
				Mouse.OnMouseDelta(ri.data.mouse.lLastX, ri.data.mouse.lLastY);
			}
			break;
		}
		default:
			break;
#pragma endregion Mouse_Input
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}
#pragma endregion

#pragma region W2WindowClass_Implemention
W2WindowAPI::W2WindowClass::W2WindowClass() noexcept
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

const char* W2WindowAPI::W2WindowClass::GetName() noexcept
{
	return m_wndClassName;
}

HINSTANCE W2WindowAPI::W2WindowClass::GetInstance() noexcept
{
	return m_wndClass.m_hInstance;
}

W2WindowAPI::W2WindowClass::~W2WindowClass()
{
	UnregisterClass(m_wndClassName, GetInstance());
}
#pragma endregion

#pragma region Exception_Implemention

std::string W2WindowAPI::Exception::TranslateErrorCode(HRESULT hr) noexcept
{
	char* pMsgBuf = nullptr;
	DWORD nMsgLen = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPSTR>(&pMsgBuf), 0, nullptr
	);
	if (nMsgLen == 0)
	{
		return "Unidentified Error Code";
	}
	std::string errorString = pMsgBuf;
	LocalFree(pMsgBuf);
	return errorString;
}

W2WindowAPI::HRException::HRException(int line, const char* file, HRESULT hr) noexcept
	: Exception(line, file), m_hr(hr)
{}

const char* W2WindowAPI::HRException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << "\n"
		<< "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
		<< std::dec << " (" << static_cast<unsigned long>(GetErrorCode()) << ")" << "\n"
		<< "[Error String] " << GetErrorString() << "\n"
		<< "[Description] " << GetErrorString() << "\n"
		<< GetOriginString();
	m_whatBuffer = oss.str();
	return m_whatBuffer.c_str();
}

const char* W2WindowAPI::HRException::GetType() const noexcept
{
	return "Witchy Window Exception";
}

HRESULT W2WindowAPI::HRException::GetErrorCode() const noexcept
{
	return m_hr;
}

std::string W2WindowAPI::HRException::GetErrorString() const noexcept
{
	return TranslateErrorCode(m_hr);
}

std::string W2WindowAPI::HRException::GetErrorDescription() const noexcept
{
	char buf[512];
	FormatMessage(
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr, m_hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		buf, sizeof(buf), nullptr
	);
	return buf;
}

const char* W2WindowAPI::DeviceRemovedException::GetType() const noexcept
{
	return "Witchy Window Exception [Removed Device]";
}

#pragma endregion
