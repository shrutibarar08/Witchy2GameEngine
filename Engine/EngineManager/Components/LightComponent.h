#pragma once
#include <Windows.h>
#include <vector>
#include <unordered_map>
#include <d3d11.h>
#include <wrl.h>


// TODO: Implement this dynamically.
// TODO: This class will act as base class for lights and will be used to
// update light and bind light in structure binding.
template<typename T>
class LightComponent
{
public:
	LightComponent(UINT slot);
	virtual ~LightComponent();

	LightComponent(const LightComponent&)			= delete;
	LightComponent(LightComponent&&)				= delete;
	LightComponent operator=(const LightComponent&) = delete;
	LightComponent operator=(LightComponent&&)		= delete;

	UINT GetBindSlot() const noexcept;
	void Bind() noexcept;
	void AddLight(T& lightSource) noexcept;

private:
	UINT m_slot{ 101 };
	std::vector<T> m_lights;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shaderRV;
	bool _update_required{ true };	// If True it creates resources else reuse existing ones
};
