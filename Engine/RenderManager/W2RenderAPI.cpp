#include "W2RenderAPI.h"

#include "WindowsManager/W2WindowAPI.h"
#include "ExceptionManager/dxerr.h"

#include <stdexcept>
#include <sstream>

#include <d3dcompiler.h>
#include <DirectXMath.h>


#pragma region RenderAPI_Implemention
W2RenderAPI* W2RenderAPI::m_instance = nullptr;

W2RenderAPI::W2RenderAPI(HWND hWnd)
{
	RECT rt;
	GetClientRect(hWnd, &rt);

	DXGI_SWAP_CHAIN_DESC sd{};
	sd.BufferDesc.Width					  = rt.right - rt.left;
	sd.BufferDesc.Height				  = rt.bottom - rt.top;
	sd.BufferDesc.RefreshRate.Numerator	  = 60u;
	sd.BufferDesc.RefreshRate.Denominator = 1u;
	sd.BufferDesc.Format				  = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.ScanlineOrdering		  = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling				  = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.OutputWindow						  = hWnd;
	sd.Windowed							  = TRUE;
	sd.SwapEffect						  = DXGI_SWAP_EFFECT_DISCARD;
	sd.BufferCount						  = 1u;
	sd.SampleDesc.Count					  = 1u;
	sd.SampleDesc.Quality				  = 0u;
	sd.BufferUsage						  = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.Flags							  = D3D11_CREATE_DEVICE_DEBUG;

	HRESULT hr;
	RENDER_API_THROW(D3D11CreateDeviceAndSwapChain
	(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		m_debugFlag,
		nullptr,
		0u,
		D3D11_SDK_VERSION,
		&sd,
		&m_swapChain,
		&m_device,
		nullptr,
		&m_deviceContext
	));

	//~ Creates RenderTarget
	Microsoft::WRL::ComPtr<ID3D11Resource> pBackBuffer;
	RENDER_API_THROW(m_swapChain->GetBuffer(0u, __uuidof(ID3D11Resource), &pBackBuffer));
	RENDER_API_THROW(m_device->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &m_renderTV));

	//~ Creates Depth Stencil buffer and View
	D3D11_TEXTURE2D_DESC td{};
	td.Width = rt.right - rt.left;
	td.Height = rt.bottom - rt.top;
	td.MipLevels = 1u;
	td.ArraySize = 1u;
	td.Format = DXGI_FORMAT_D32_FLOAT;
	td.SampleDesc.Count = 1u;
	td.SampleDesc.Quality = 0u;
	td.Usage = D3D11_USAGE_DEFAULT;
	td.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	td.CPUAccessFlags = 0u;
	td.MiscFlags = 0u;

	RENDER_API_THROW(m_device->CreateTexture2D(&td, nullptr, &m_depthT2D));

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd{};
	dsvd.Format = td.Format;
	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvd.Texture2D.MipSlice = 0u;

	RENDER_API_THROW(m_device->CreateDepthStencilView(m_depthT2D.Get(), &dsvd, &m_depthSV));

	m_deviceContext->OMSetRenderTargets(1u, m_renderTV.GetAddressOf(), m_depthSV.Get());

	//~ Set Viewport
	D3D11_VIEWPORT vp{};
	vp.Width = static_cast<float>(td.Width);
	vp.Height = static_cast<float>(td.Height);
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	m_deviceContext->RSSetViewports(1u, &vp);
}

W2RenderAPI::~W2RenderAPI()
{
	if (m_instance) delete m_instance;
}

void W2RenderAPI::Init()
{
	if (m_instance == nullptr)
	{
		m_instance = new W2RenderAPI(W2WindowAPI::Get()->GetHandleWindow());
	}
}

W2RenderAPI* W2RenderAPI::Get()
{
	if (m_instance != nullptr)
	{
		return m_instance;
	}
	throw std::logic_error("RenderAPI Not Initialized!");
}

ID3D11Device* W2RenderAPI::GetDevice()
{
	return m_device.Get();
}

ID3D11DeviceContext* W2RenderAPI::GetDeviceContext()
{
	return m_deviceContext.Get();
}

DxgiInfoManager& W2RenderAPI::GetInfoManager() noexcept(ON_DEBUG)
{
#ifdef _DEBUG
	return m_dxgiInfoManager;
#endif
	throw std::logic_error("Called Debug Functionality on release build!");
}

void W2RenderAPI::RecordStart() const
{
	m_deviceContext->ClearRenderTargetView(m_renderTV.Get(), _defaultColor);
	m_deviceContext->ClearDepthStencilView(m_depthSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0u);
}

void W2RenderAPI::RecordEnd() const
{
	HRESULT hr;
	if (FAILED(hr = m_swapChain->Present(1u, 0u)))
	{
		if (hr == DXGI_ERROR_DEVICE_REMOVED)
		{
			throw RENDER_API_REMOVED_EXCEPT(hr);
		}
		else
		{
			throw RENDER_API_EXCEPT(hr);
		}
	}
}

void W2RenderAPI::DrawIndexed(UINT count) noexcept(!ON_DEBUG)
{
	RENDER_API_INFO_ONLY(m_deviceContext->DrawIndexed(count, 0u, 0u));
}

void W2RenderAPI::SetProjection(DirectX::XMMATRIX projection) noexcept
{
	m_projection = projection;
}

DirectX::XMMATRIX W2RenderAPI::GetProjection() const noexcept
{
	return m_projection;
}

#pragma endregion


#pragma region Exception_Implemention

W2RenderAPI::HRException::HRException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMessages) noexcept
	: Exception(line, file), m_hr(hr)
{
	for (const auto& m : infoMessages)
	{
		m_info += m;
		m_info.push_back('\n');
	}
	if (!m_info.empty())
	{
		m_info.pop_back();
	}
}

const char* W2RenderAPI::HRException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << "\n"
		<< "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
		<< std::dec << " (" << static_cast<unsigned long>(GetErrorCode()) << ")" << "\n"
		<< "[Error String] " << GetErrorString() << "\n"
		<< "[Description] " << GetErrorDescription() << "\n";
		if (!m_info.empty())
		{
			oss << "\n[Error Info]\n" << GetErrorInfo() << "\n\n";
		}
		oss << GetOriginString();

	m_whatBuffer = oss.str();

	return m_whatBuffer.c_str();
}

const char* W2RenderAPI::HRException::GetType() const noexcept
{
	return "Witchy RenderAPI Exception";
}

HRESULT W2RenderAPI::HRException::GetErrorCode() const noexcept
{
	return m_hr;
}

std::string W2RenderAPI::HRException::GetErrorString() const noexcept
{
	return DXGetErrorString(m_hr);
}

std::string W2RenderAPI::HRException::GetErrorDescription() const noexcept
{
	char buf[512];
	DXGetErrorDescription(m_hr, buf, sizeof(buf));
	return buf;
}

std::string W2RenderAPI::HRException::GetErrorInfo() const noexcept
{
	return m_info;
}

const char* W2RenderAPI::DeviceRemovedException::GetType() const noexcept
{
	return "Witchy RenderAPI Exception [Removed Device]";
}

W2RenderAPI::InfoException::InfoException(int line, const char* file, std::vector<std::string> infoMessages) noexcept
	: Exception(line, file)
{
	for (const auto& m: infoMessages)
	{
		m_info += m;
		m_info.push_back('\n');
	}
	if (!m_info.empty()) m_info.pop_back(); // Remove Extra '\n'
}

const char* W2RenderAPI::InfoException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << "\n"
		<< "\n[Error Info]\n" << GetErrorInfo() << "\n\n";;
	oss << GetOriginString();
	m_whatBuffer = oss.str();
	return m_whatBuffer.c_str();
}

const char* W2RenderAPI::InfoException::GetType() const noexcept
{
	return "Witchy RenderAPI Info Exception";
}

std::string W2RenderAPI::InfoException::GetErrorInfo() const noexcept
{
	return m_info;
}

#pragma endregion
