#include "InputLayout.h"
#include "Exceptions/RenderAPIMacros.h"


InputLayout::InputLayout(const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout,
	ID3DBlob* pVertexShaderBytecode)
{
	INIT_INFO();

	RENDER_API_THROW(W2RenderAPI::Get()->GetDevice()->CreateInputLayout(
		layout.data(), (UINT)layout.size(),
		pVertexShaderBytecode->GetBufferPointer(),
		pVertexShaderBytecode->GetBufferSize(),
		&pInputLayout
	));
}

void InputLayout::Bind() noexcept
{
	W2RenderAPI::Get()->GetDeviceContext()->IASetInputLayout(pInputLayout.Get());
}
