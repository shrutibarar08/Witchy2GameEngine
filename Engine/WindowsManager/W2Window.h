#pragma once

#include <Windows.h>
#include <optional>
#include <memory>

#include "Components/W2Keyboard.h"
#include "Components/W2Mouse.h"

#include "ExceptionManager/W2Exception.h"

/**
 * @brief A Window class that encapsulates the Win32 window creation and message handling.
 */
class W2Window
{
public:
	W2Window(RECT rect, const char* title);
	~W2Window();
	W2Window(const W2Window&) = delete;
	W2Window& operator=(const W2Window&) = delete;

	const char* GetTitleName() const noexcept;
	void SetTitleName(const std::string& newName) const;

	HWND GetHandleWindow() const noexcept;

	static std::optional<int> ProcessMessages() noexcept;

	//~ Components
	W2Keyboard Keyboard;
	W2Mouse	   Mouse;

private:
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) noexcept;
	static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) noexcept;
	LRESULT HandleMsg(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) noexcept;

	//~ Members
	RECT m_rect;
	HWND m_hWnd;
	const char* m_titleName;

private:
	/**
	 * @brief A helper class that manages the registration and properties of the window class.
	 */
	class W2WindowClass
	{
	public:
		static const char* GetName() noexcept;
		static HINSTANCE GetInstance() noexcept;
	private:
		//~ Implementation
		W2WindowClass() noexcept;
		~W2WindowClass();
		W2WindowClass(const W2WindowClass&) = delete;
		W2WindowClass& operator=(const W2WindowClass&) = delete;
		
		//~ Members
		static W2WindowClass		 m_wndClass;
		static constexpr const char* m_wndClassName = "W2WindowClass";
		HINSTANCE					 m_hInstance;
	};

public:
	class Exception : public W2Exception
	{
		using W2Exception::W2Exception;
	public:
		static std::string TranslateErrorCode(HRESULT hr) noexcept;
	};
	class HRException : public Exception
	{
	public:
		HRException(int line, const char* file, HRESULT hr) noexcept;
		const char* what() const noexcept override;
		virtual const char* GetType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;
		std::string GetErrorDescription() const noexcept;
	private:
		HRESULT m_hr;
	};
	class DeviceRemovedException : public Exception
	{
	public:
		using Exception::Exception;
		const char* GetType() const noexcept override;
	};
};

//~ Error Exception Helper Macros
#define W2WND_EXCEPT(hr) W2Window::HRException(__LINE__, __FILE__, (hr))
#define W2WND_LAST_EXCEPT() W2Window::HRException(__LINE__, __FILE__, GetLastError())
#define W2WND_NO_DEVICE_EXCEPT() W2Window::DeviceRemovedException(__LINE__, __FILE__)