#pragma once
#include <Windows.h>
#include <d3d11.h>
#include <wrl.h>

#include "Exceptions/W2Exception.h"



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

private:
	Microsoft::WRL::ComPtr<ID3D11Device>		   m_device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>    m_deviceContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain>		   m_swapChain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTV;

	const float _testColor[4]{ 0.3f, 0.2f, 0.1f, 1.f };

private:
	class Exception : public W2Exception
	{
		using W2Exception::W2Exception;
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
	class DeviceRemovedException : public HRException
	{
		using HRException::HRException;
	public:
		virtual const char* GetType() const noexcept override;
	};
};

#define RENDER_API_FAILED(hrCall) if (FAILED(hr = (hrCall))) throw W2RenderAPI::HRException(__LINE__, __FILE__, hr)
#define RENDER_API_REMOVED_EXCEPT(hr) W2RenderAPI::DeviceRemovedException(__LINE__, __FILE__, (hr))
