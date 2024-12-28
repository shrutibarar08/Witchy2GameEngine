#pragma once
#include "Bindable.h"


class VertexShader : public Bindable
{
public:
	VertexShader(const std::wstring& path);
	void Bind() noexcept override;
	ID3DBlob* GetBytecode() const noexcept;

protected:
	Microsoft::WRL::ComPtr<ID3DBlob> m_pBytecodeBlob;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_pVertexShader;
};

