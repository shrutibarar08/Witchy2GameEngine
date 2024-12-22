#include "W2RenderAPI.h"

#include <stdexcept>


W2RenderAPI::W2RenderAPI(HWND hWnd)
{
	RECT rt;
	GetClientRect(hWnd, &rt);

	DXGI_SWAP_CHAIN_DESC sd{};
	sd.BufferDesc.Width					  = rt.right - rt.left;
	sd.BufferDesc.Height				  = rt.bottom - rt.top;
	sd.BufferDesc.RefreshRate.Numerator	  = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format				  = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.ScanlineOrdering		  = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling				  = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.OutputWindow						  = hWnd;
	sd.Windowed							  = TRUE;
	sd.SwapEffect						  = DXGI_SWAP_EFFECT_DISCARD;
	sd.BufferCount						  = 1;
	sd.SampleDesc.Count					  = 1;
	sd.SampleDesc.Quality				  = 0;
	sd.BufferUsage						  = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.Flags = 0;

	HRESULT hr = D3D11CreateDeviceAndSwapChain
	(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0u,
		nullptr,
		0u,
		D3D11_SDK_VERSION,
		&sd,
		&m_swapChain,
		&m_device,
		nullptr,
		&m_deviceContext
	);

	if (FAILED(hr))
	{
		throw std::runtime_error("Failed to create device and swap chain");
	}

	//~ Creates RenderTarget
	Microsoft::WRL::ComPtr<ID3D11Resource> pBackBuffer;
	hr = m_swapChain->GetBuffer(0u, __uuidof(ID3D11Resource), &pBackBuffer);

	if (FAILED(hr))
	{
		throw std::runtime_error("Failed to Create RenderTargetView!");
	}

	hr = m_device->CreateRenderTargetView(pBackBuffer.Get(),
		nullptr, &m_renderTV);

	if (FAILED(hr))
	{
		throw std::runtime_error("Failed to Create RenderTargetView!");
	}
}

void W2RenderAPI::ClearBuffer() const
{
	m_deviceContext->ClearRenderTargetView(m_renderTV.Get(), _testColor);
}

void W2RenderAPI::PresentFrame()
{

	const HRESULT hr = m_swapChain->Present(1u, 0u);

	if (FAILED(hr))
	{
		throw std::runtime_error("Failed to Present Frame!");
	}
}
