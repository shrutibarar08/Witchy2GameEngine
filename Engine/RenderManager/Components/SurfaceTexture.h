#pragma once

#include "Bindable.h"
#include <vector>
#include <unordered_map>
#include <wrl.h>

class SurfaceTexture final: public Bindable
{
public:
	SurfaceTexture() = default;
	~SurfaceTexture() override;

	SurfaceTexture(const SurfaceTexture&) = delete;
	SurfaceTexture& operator=(const SurfaceTexture&) = delete;
	SurfaceTexture(SurfaceTexture&&) = delete;
	SurfaceTexture&& operator=(SurfaceTexture&&) = delete;

	void Bind() noexcept override;
	void AddTexture(const std::wstring& texturePath);
	void UpdateTexture(const std::wstring& texturePath);
	void UpdateTexture(const std::string& texturePath);
	std::string GetTopTexture() const;
	void AddSampler();

private:
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> m_shaderRV;
	std::vector<Microsoft::WRL::ComPtr<ID3D11SamplerState>> m_samplerState;
	std::unordered_map<std::wstring, bool> m_cacheInfo;
	std::wstring m_topTexture;
};
