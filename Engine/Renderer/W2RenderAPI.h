#pragma once
#include <Windows.h>
#include <d3d11.h>
#include <wrl.h>
#include <vector>

#include "Exceptions/W2Exception.h"
#include "Exceptions/DxgiInfoManager.h"


/**
 * @brief RenderAPI Class Based on DirectX 11
 */
class W2RenderAPI
{
public:
	W2RenderAPI(HWND hWnd);
	~W2RenderAPI() = default;
	W2RenderAPI(const W2RenderAPI&) = delete;
	W2RenderAPI& operator=(const W2RenderAPI&) = delete;
	W2RenderAPI(W2RenderAPI&&) = delete;
	W2RenderAPI& operator=(W2RenderAPI&&) = delete;

	//~ Render Methods
	void ClearBuffer() const;
	void PresentFrame() const;

	//~ Just For Testing Everything Working Fine or not
	void TestDraw(float time);

private:
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

#define RENDER_API_EXCEPT_NOINFO(hr) W2RenderAPI::HRException(__LINE__, __FILE__, (hr))
#define RENDER_API_THROW_NOINFO(hrCall) if (FAILED(hr=(hrCall))) throw RENDER_API_EXCEPT_NOINFO(hr)

#ifdef _DEBUG
#define RENDER_API_EXCEPT(hr) W2RenderAPI::HRException(__LINE__, __FILE__, (hr), m_dxgiInfoManager.GetMessage())
#define RENDER_API_THROW(hrCall) m_dxgiInfoManager.Set(); if (FAILED(hr=(hrCall))) throw RENDER_API_EXCEPT(hr)
#define RENDER_API_REMOVED_EXCEPT(hr) W2RenderAPI::DeviceRemovedException(__LINE__, __FILE__, (hr), m_dxgiInfoManager.GetMessage())
#define RENDER_API_INFO_ONLY(hrCall) m_dxgiInfoManager.Set(); (hrCall); { auto v = m_dxgiInfoManager.GetMessage(); if (!v.empty()) throw W2RenderAPI::InfoException(__LINE__, __FILE__, std::move(v)); }
#else
#define RENDER_API_EXCEPT(hr) W2RenderAPI::HRException(__LINE__, __FILE__, (hr))
#define RENDER_API_THROW(hrCall) RENDER_API_THROW_NOINFO(hrCall)
#define RENDER_API_REMOVED_EXCEPT(hr) W2RenderAPI::DeviceRemovedException(__LINE__, __FILE__, (hr))
#endif
