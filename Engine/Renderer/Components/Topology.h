#pragma once

#include "Bindable.h"


class Topology : public Bindable
{
public:
	Topology(D3D11_PRIMITIVE_TOPOLOGY type);
	void Bind() noexcept override;

private:
	D3D11_PRIMITIVE_TOPOLOGY m_type;
};

