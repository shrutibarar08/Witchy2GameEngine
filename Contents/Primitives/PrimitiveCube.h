#pragma once

#include "PrimitiveBase.h"
#include "PrimitiveTriangle.h"
#include <DirectXMath.h>
#include <iostream>


class PrimitiveCube
{
public:

	template<RequirePos T>
	PrimitiveCube&& BuildPosition()&&
	{
		if (m_vertices<T>.empty()) m_vertices<T>.resize(24);
		constexpr float side = 1.0f / 2.0f;

		std::vector<DirectX::XMFLOAT3> vertices{
			{-1, 1.0, -1.0f},
			{1.0f, 1.0f, -1.0f},
			{1.0f, 1.0f, 1.0f},
			{-1.0f, 1.0f, 1.0f},

			{-1, -1.0, -1.0f},
			{1.0f, -1.0f, -1.0f},
			{1.0f, -1.0f, 1.0f},
			{-1.0f, -1.0f, 1.0f},

			{-1, -1.0, 1.0f},
			{-1.0f, -1.0f, -1.0f},
			{-1.0f, 1.0f, -1.0f},
			{-1.0f, 1.0f, 1.0f},

			{1, -1.0, 1.0f},
			{1.0f, -1.0f, -1.0f},
			{1.0f, 1.0f, -1.0f},
			{1.0f, 1.0f, 1.0f},

			{-1, -1.0, -1.0f},
			{1.0f, -1.0f, -1.0f},
			{1.0f, 1.0f, -1.0f},
			{-1.0f, 1.0f, -1.0f},

			{-1, -1.0, 1.0f},
			{1.0f, -1.0f, 1.0f},
			{1.0f, 1.0f, 1.0f},
			{-1.0f, 1.0f, 1.0f},
		};

		for (size_t i = 0; i < vertices.size(); i++)
		{
			m_vertices<T>[i].pos = vertices[i];
		}
		return std::move(*this);
	}

	template<RequireTex T>
	PrimitiveCube&& BuildTexcord()&&
	{
		if (m_vertices<T>.empty()) m_vertices<T>.resize(24);

		std::vector<DirectX::XMFLOAT2> texcoords
		{
			{ 1.0f, 0.0f },
			{ 0.0f, 0.0f },
			{ 0.0f, 1.0f },
			{ 1.0f, 1.0f },

			{ 0.0f, 0.0f },
			{ 1.0f, 0.0f },
			{ 1.0f, 1.0f },
			{ 0.0f, 1.0f },

			{ 0.0f, 1.0f },
			{ 1.0f, 1.0f },
			{ 1.0f, 0.0f },
			{ 0.0f, 0.0f },

			{ 1.0f, 1.0f },
			{ 0.0f, 1.0f },
			{ 0.0f, 0.0f },
			{ 1.0f, 0.0f },

			{ 0.0f, 1.0f },
			{ 1.0f, 1.0f },
			{ 1.0f, 0.0f },
			{ 0.0f, 0.0f },

			{ 1.0f, 1.0f },
			{ 0.0f, 1.0f },
			{ 0.0f, 0.0f },
			{ 1.0f, 0.0f }
		};

		for (int i = 0; i < texcoords.size(); i++)
		{
			m_vertices<T>[i].tex = texcoords[i];
		}

		return std::move(*this);
	}

	static std::vector<unsigned short> BuildIndices()
	{
		return
		{
			3,1,0,
			2,1,3,

			6,4,5,
			7,4,6,

			11,9,8,
			10,9,11,

			14,12,13,
			15,12,14,

			19,17,16,
			18,17,19,

			22,20,21,
			23,20,22
		};
	}

	template<class V>
	static PrimitiveTriangle<V> GetTopology()
	{
		return
		{
			std::move(m_vertices<V>),
			std::move(BuildIndices())
		};
	}

	template<RequireNorm T>
	PrimitiveCube&& BuildNorm() && noexcept(!ON_DEBUG)
	{
		using namespace DirectX;
		auto indices = BuildIndices();
		assert(indices.size() % 3 == 0 && indices.size() > 0);
		assert(!m_vertices<T>.empty());

		for (size_t i = 0; i < indices.size(); i += 3)
		{
			const auto p0 = XMLoadFloat3(&m_vertices<T>[indices[i]].pos);
			const auto p1 = XMLoadFloat3(&m_vertices<T>[indices[i + 1]].pos);
			const auto p2 = XMLoadFloat3(&m_vertices<T>[indices[i + 2]].pos);

			// Calculate vectors u and v
			const auto u = XMVectorSubtract(p1, p0);
			const auto v = XMVectorSubtract(p2, p0);

			// Compute the normalized cross product
			const auto n = XMVector3Normalize(XMVector3Cross(u, v));

			XMStoreFloat3(&m_vertices<T>[indices[i]].norm, n);
			XMStoreFloat3(&m_vertices<T>[indices[i + 1]].norm, n);
			XMStoreFloat3(&m_vertices<T>[indices[i + 2]].norm, n);
		}

		return std::move(*this);
	}

private:
	template<class V>
	static std::vector<V> m_vertices;
};
