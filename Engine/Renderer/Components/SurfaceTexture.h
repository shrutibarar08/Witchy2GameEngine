#pragma once

#include "Bindable.h"
#include <vector>
#include <wrl.h>

class SurfaceTexture final: public Bindable
{
public:
	SurfaceTexture() = default;

	SurfaceTexture(const SurfaceTexture&) = delete;
	SurfaceTexture& operator=(const SurfaceTexture&) = delete;
	SurfaceTexture(SurfaceTexture&&) = delete;
	SurfaceTexture&& operator=(SurfaceTexture&&) = delete;

	void Bind() noexcept override;
	void AddTexture(const std::wstring& texturePath);
	void AddSampler();

private:
	std::vector<ID3D11ShaderResourceView*> m_shaderRV;
	std::vector<ID3D11SamplerState*> m_samplerState;
};
