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

		std::vector<T> vertices(24);
		vertices[0].pos = { -side,-side,-side };// 0 near side
		vertices[1].pos = { side,-side,-side };// 1
		vertices[2].pos = { -side,side,-side };// 2
		vertices[3].pos = { side,side,-side };// 3
		vertices[4].pos = { -side,-side,side };// 4 far side
		vertices[5].pos = { side,-side,side };// 5
		vertices[6].pos = { -side,side,side };// 6
		vertices[7].pos = { side,side,side };// 7
		vertices[8].pos = { -side,-side,-side };// 8 left side
		vertices[9].pos = { -side,side,-side };// 9
		vertices[10].pos = { -side,-side,side };// 10
		vertices[11].pos = { -side,side,side };// 11
		vertices[12].pos = { side,-side,-side };// 12 right side
		vertices[13].pos = { side,side,-side };// 13
		vertices[14].pos = { side,-side,side };// 14
		vertices[15].pos = { side,side,side };// 15
		vertices[16].pos = { -side,-side,-side };// 16 bottom side
		vertices[17].pos = { side,-side,-side };// 17
		vertices[18].pos = { -side,-side,side };// 18
		vertices[19].pos = { side,-side,side };// 19
		vertices[20].pos = { -side,side,-side };// 20 top side
		vertices[21].pos = { side,side,-side };// 21
		vertices[22].pos = { -side,side,side };// 22
		vertices[23].pos = { side,side,side };// 23

		for (size_t i = 0; i < vertices.size(); i++)
		{
			m_vertices<T>[i].pos = vertices[i].pos;
		}
		return std::move(*this);
	}

	template<RequireTex T>
	PrimitiveCube&& BuildTexcord()&&
	{
		if (m_vertices<T>.empty()) m_vertices<T>.resize(24);

		std::vector<DirectX::XMFLOAT2> texcoords
		{
			{ 0.0f, 0.0f },
			{ 1.0f, 0.0f },
			{ 0.0f, 1.0f },
			{ 1.0f, 1.0f },

			{ 0.0f, 0.0f },
			{ 1.0f, 0.0f },
			{ 0.0f, 1.0f },
			{ 1.0f, 1.0f },

			{ 0.0f, 0.0f },
			{ 1.0f, 0.0f },
			{ 0.0f, 1.0f },
			{ 1.0f, 1.0f },

			{ 0.0f, 0.0f },
			{ 1.0f, 0.0f },
			{ 0.0f, 1.0f },
			{ 1.0f, 1.0f },

			{ 0.0f, 0.0f },
			{ 1.0f, 0.0f },
			{ 0.0f, 1.0f },
			{ 1.0f, 1.0f },

			{ 0.0f, 0.0f },
			{ 1.0f, 0.0f },
			{ 0.0f, 1.0f },
			{ 1.0f, 1.0f }
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
			0,2, 1,    2,3,1,
			4,5, 7,    4,7,6,
			8,10, 9,  10,11,9,
			12,13,15, 12,15,14,
			16,17,18, 18,17,19,
			20,23,21, 20,22,23
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
