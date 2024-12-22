#pragma once
#include <Windows.h>
#include <d3d11.h>
#include <wrl.h>

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
	void PresentFrame();

private:
	Microsoft::WRL::ComPtr<ID3D11Device>		   m_device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>    m_deviceContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain>		   m_swapChain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTV;

	const float _testColor[4]{ 0.3f, 0.2f, 0.1f, 1.f };
};
