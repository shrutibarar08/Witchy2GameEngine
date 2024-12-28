#pragma once
#include <Windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <vector>

#include "ExceptionManager/W2Exception.h"
#include "ExceptionManager/DxgiInfoManager.h"
#include "ExceptionManager/RenderAPIMacros.h"


// TODO: Create PQ to manage priority for Construction and Destruction

/**
 * @brief RenderAPI Class Based on DirectX 11
 */
class W2RenderAPI
{
public:

	static void Init();
	static W2RenderAPI* Get();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();
	DxgiInfoManager& GetInfoManager() noexcept(ON_DEBUG);

	//~ Render Methods
	void RecordStart() const;
	void RecordEnd() const;
	void DrawIndexed(UINT count) noexcept(!ON_DEBUG);

	void SetProjection(DirectX::XMMATRIX projection) noexcept;
	DirectX::XMMATRIX GetProjection() const noexcept;

private:
	W2RenderAPI(HWND hWnd);
	~W2RenderAPI();
	W2RenderAPI(const W2RenderAPI&) = delete;
	W2RenderAPI& operator=(const W2RenderAPI&) = delete;
	W2RenderAPI(W2RenderAPI&&) = delete;
	W2RenderAPI& operator=(W2RenderAPI&&) = delete;

	static W2RenderAPI* m_instance;
	DirectX::XMMATRIX m_projection;

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
