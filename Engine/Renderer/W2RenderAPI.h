#pragma once
#include <Windows.h>
#include <d3d11.h>
#include <wrl.h>
#include <vector>

#include "Exceptions/W2Exception.h"
#include "Exceptions/DxgiInfoManager.h"


// TODO: Create PQ to manage priority for Construction and Destruction

/**
 * @brief RenderAPI Class Based on DirectX 11
 */
class W2RenderAPI
{
public:

	static void Init(HWND hWnd);
	static W2RenderAPI* Get();

	//~ Render Methods
	void ClearBuffer() const;
	void PresentFrame() const;

private:
	W2RenderAPI(HWND hWnd);
	~W2RenderAPI();
	W2RenderAPI(const W2RenderAPI&) = delete;
	W2RenderAPI& operator=(const W2RenderAPI&) = delete;
	W2RenderAPI(W2RenderAPI&&) = delete;
	W2RenderAPI& operator=(W2RenderAPI&&) = delete;

	static W2RenderAPI* m_instance;

public:

	Microsoft::WRL::ComPtr<ID3D11Device>		   m_device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>    m_deviceContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain>		   m_swapChain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTV;
	Microsoft::WRL::ComPtr<ID3D11Texture2D>		   m_depthT2D;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthSV;

#ifdef _DEBUG
	DxgiInfoManager m_dxgiInfoManager;
	UINT m_debugFlag = D3D11_CREATE_DEVICE_DEBUG;
#else
	UINT m_debugFlag = 0u;
#endif
	const float _defaultColor[4]{ 0.3f, 0.2f, 0.1f, 1.f };

public:
	class Exception : public W2Exception
	{
		using W2Exception::W2Exception;
	};
	class HRException : public Exception
	{
	public:
		HRException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMessages = {}) noexcept;
		const char* what() const noexcept override;
		virtual const char* GetType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;
		std::string GetErrorDescription() const noexcept;
		std::string GetErrorInfo() const noexcept;
	private:
		HRESULT m_hr;
		std::string m_info;
	};
	class DeviceRemovedException : public HRException
	{
		using HRException::HRException;
	public:
		virtual const char* GetType() const noexcept override;
	private:
		std::string m_description;
	};
	class InfoException : public Exception
	{
	public:
		InfoException(int line, const char* file, std::vector<std::string> infoMessages) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		std::string GetErrorInfo() const noexcept;
	private:
		std::string m_info;
	};
};
