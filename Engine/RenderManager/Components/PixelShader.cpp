#include "PixelShader.h"
#include "ExceptionManager/RenderAPIMacros.h"

#include <d3dcompiler.h>


PixelShader::PixelShader(const std::wstring& path)
{
	INIT_INFO();

	Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
	RENDER_API_THROW(D3DReadFileToBlob(path.c_str(), &pBlob));
	RENDER_API_THROW(W2RenderAPI::Get()->GetDevice()->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader));
}

void PixelShader::Bind() noexcept
{
	W2RenderAPI::Get()->GetDeviceContext()->PSSetShader(pPixelShader.Get(), nullptr, 0u);
}
