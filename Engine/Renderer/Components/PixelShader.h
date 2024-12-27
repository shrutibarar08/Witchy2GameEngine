#pragma once
#include "Bindable.h"

class PixelShader : public Bindable
{
public:
	PixelShader(const std::wstring& path);
	void Bind() noexcept override;
protected:
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
};