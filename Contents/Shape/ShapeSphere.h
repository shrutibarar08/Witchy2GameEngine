#pragma once
#include "RenderManager/Object/WObject.h"


class ShapeSphere : public WObject<ShapeSphere>
{
public:
	ShapeSphere(float radius);
	void Update(float deltaTime) noexcept override;
	void SetTranslation(DirectX::XMFLOAT3 translationVec) noexcept;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
private:
	DirectX::XMFLOAT3 m_translation{ 1.0f, 1.0f, 1.0f };
};
