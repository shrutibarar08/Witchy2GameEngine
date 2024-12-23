#include "W2RenderAPI.h"
#include "Exceptions/dxerr.h"

#include <stdexcept>
#include <sstream>

#include <d3dcompiler.h>
#include <DirectXMath.h>


#pragma region RenderAPI_Implemention

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

	m_deviceContext->OMSetRenderTargets(1u, m_renderTV.GetAddressOf(), nullptr);
}

void W2RenderAPI::ClearBuffer() const
{
	m_deviceContext->ClearRenderTargetView(m_renderTV.Get(), _defaultColor);
}

void W2RenderAPI::PresentFrame() const
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

void W2RenderAPI::TestDraw()
{
	HRESULT hr;

	//~ Implement Vertex Shader
	Microsoft::WRL::ComPtr<ID3DBlob> pBlobVS;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
	RENDER_API_THROW(D3DReadFileToBlob(L"Shaders/Compiled/VertexShader.cso", &pBlobVS));
	RENDER_API_THROW(m_device->CreateVertexShader(pBlobVS->GetBufferPointer(),
		pBlobVS->GetBufferSize(), nullptr, &pVertexShader));

	//~ Implement Pixel Shader
	Microsoft::WRL::ComPtr<ID3DBlob> pBlobPS;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
	RENDER_API_THROW(D3DReadFileToBlob(L"Shaders/Compiled/PixelShader.cso", &pBlobPS));
	RENDER_API_THROW(m_device->CreatePixelShader(pBlobPS->GetBufferPointer(),
		pBlobPS->GetBufferSize(), nullptr, &pPixelShader));

	//~ Creates Input Layout
	D3D11_INPUT_ELEMENT_DESC pInputLayout[]
	{
		{
			"POSITION", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u,
			0u, D3D11_INPUT_PER_VERTEX_DATA, 0u
		},
	};
	Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayoutObj;
	RENDER_API_THROW(m_device->CreateInputLayout(pInputLayout, 1u,
		pBlobVS->GetBufferPointer(), pBlobVS->GetBufferSize(),
		&pInputLayoutObj));

	//~ Creates Vertices
	struct Vertex
	{
		float x;
		float y;
		float z;
	};

	Vertex vertices[]
	{
		{-1, 1, -1},
		{1, 1, -1},
		{1, 1, 1},
		{-1, 1, 1},
		{-1, -1, -1},
		{1, -1, -1},
		{1, -1, 1},
		{-1, -1, 1},
	};

	D3D11_BUFFER_DESC bufferDesc{};
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.ByteWidth = sizeof(vertices);
	bufferDesc.CPUAccessFlags = 0u;
	bufferDesc.MiscFlags = 0u;
	bufferDesc.StructureByteStride = sizeof(Vertex);
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA subData{};
	subData.pSysMem = vertices;

	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer{ nullptr };

	RENDER_API_THROW(m_device->CreateBuffer(&bufferDesc, &subData, &vertexBuffer));
	m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//~ Creates Indices
	UINT indices[]
	{
		3, 1, 0,
		2, 1, 3,

		0, 5, 4,
		1, 5, 0,

		3, 4, 7,
		0, 4, 3,

		1, 6, 5,
		2, 6, 1,

		2, 7, 6,
		3, 7, 2,

		6, 4, 5,
		7, 4, 6,
	};
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.ByteWidth = sizeof(indices);
	bufferDesc.StructureByteStride = sizeof(UINT);

	subData.pSysMem = indices;

	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer{ nullptr };
	RENDER_API_THROW(m_device->CreateBuffer(&bufferDesc, &subData, &indexBuffer));

	//~ Set Viewport
	D3D11_VIEWPORT vp{};
	vp.Width = 1280;
	vp.Height = 720;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	m_deviceContext->RSSetViewports(1u, &vp);

	//~ Draw (additional Update Constant Buffers for vertex shader or pixel shader).
	m_deviceContext->VSSetShader(pVertexShader.Get(), nullptr, 0u);
	m_deviceContext->PSSetShader(pPixelShader.Get(), nullptr, 0u);

	m_deviceContext->IASetInputLayout(pInputLayoutObj.Get());
	UINT strides = sizeof(Vertex);
	UINT offset = 0u;

	m_deviceContext->IASetVertexBuffers(0u, 1u, vertexBuffer.GetAddressOf(), &strides, &offset);
	m_deviceContext->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0u);

	//~ Create and Update Constant buffer
	struct ConstantBuffer
	{
		DirectX::XMMATRIX World;
		DirectX::XMMATRIX View;
		DirectX::XMMATRIX Projection;
	};

	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(ConstantBuffer);
	bufferDesc.CPUAccessFlags = 0u;
	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;

	RENDER_API_THROW(m_device->CreateBuffer(&bufferDesc, nullptr, &constantBuffer));

	//~ Update Constant Buffer
	auto world = DirectX::XMMatrixIdentity();
	DirectX::XMVECTOR eye = DirectX::XMVectorSet(0.0f, 1.0f, -5.0f, 0.0f);
	DirectX::XMVECTOR at = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, -5.0f, 0.0f);
	auto view = DirectX::XMMatrixLookAtLH(eye, at, up);
	auto projection = DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV2, 1280.f / 720.f, 0.01f, 100.f);

	auto rotationY = DirectX::XMMatrixRotationY(0.4f);

	ConstantBuffer cb;
	cb.World = DirectX::XMMatrixTranspose(world);
	cb.World *= rotationY;
	cb.View = DirectX::XMMatrixTranspose(view);
	cb.Projection = DirectX::XMMatrixTranspose(projection);

	m_deviceContext->UpdateSubresource(constantBuffer.Get(), 0u, nullptr, &cb, 0, 0);
	m_deviceContext->VSSetConstantBuffers(0u, 1u, constantBuffer.GetAddressOf());

	RENDER_API_INFO_ONLY(m_deviceContext->DrawIndexed(36u, 0u, 0u));
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
