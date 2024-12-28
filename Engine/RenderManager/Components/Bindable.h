#pragma once

#include "RenderManager/W2RenderAPI.h"


class Bindable
{
public:
	//~ Will be called to bind operations on RenderAPI
	virtual void Bind() noexcept = 0;
	virtual ~Bindable() = default;
};

