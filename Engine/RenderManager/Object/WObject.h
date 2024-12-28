#pragma once

#include "RenderManager/Object/Drawable.h"
#include "RenderManager/Components/IndexBuffer.h"


template<typename T>
class WObject : public Drawable
{
protected:
	static void AddStaticBind(std::unique_ptr<Bindable> bind) noexcept(!ON_DEBUG)
	{
		assert("Must use AddStaticIndexBuffer to bind index buffer" && typeid(T) != typeid(IndexBuffer));
		m_staticBinds.push_back(std::move(bind));
	}

	void AddStaticIndexBuffer(std::unique_ptr<IndexBuffer> ibuf) noexcept(!ON_DEBUG)
	{
		assert("Attempting to add index buffer a second time" && pIndexBuffer == nullptr);
		pIndexBuffer = ibuf.get();
		m_staticBinds.push_back(std::move(ibuf));
	}

	static bool IsStaticInitialized() noexcept
	{
		return !m_staticBinds.empty();
	}

	void SetIndexFromStatic() noexcept(!ON_DEBUG)
	{
		assert("Attempting to add index buffer a second time" && pIndexBuffer == nullptr);
		for (const auto& b : m_staticBinds)
		{
			if (const auto p = dynamic_cast<IndexBuffer*>(b.get()))
			{
				pIndexBuffer = p;
				return;
			}
		}
		assert("Failed to find index buffer in static binds" && pIndexBuffer != nullptr);
	}

private:
	const std::vector<std::unique_ptr<Bindable>>& GetStaticBinds() const noexcept override
	{
		return m_staticBinds;
	}

	static std::vector<std::unique_ptr<Bindable>> m_staticBinds;
};

template<typename T>
std::vector<std::unique_ptr<Bindable>> WObject<T>::m_staticBinds;
