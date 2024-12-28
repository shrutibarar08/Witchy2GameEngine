#include "VertexShader.h"

VertexShader::VertexShader(const std::wstring& path)
{
	D3DReadFileToBlob(path.c_str(), &m_pBytecodeBlob);

	W2RenderAPI::Get()->GetDevice()->CreateVertexShader(m_pBytecodeBlob->GetBufferPointer(),
		m_pBytecodeBlob->GetBufferSize(), nullptr, &m_pVertexShader);
}

void VertexShader::Bind() noexcept
{
	W2RenderAPI::Get()->GetDeviceContext()->VSSetShader(m_pVertexShader.Get(), nullptr, 0);
}

ID3DBlob* VertexShader::GetBytecode() const noexcept
{
	return m_pBytecodeBlob.Get();
}
