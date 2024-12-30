#pragma once

#include "PrimitiveBase.h"
#include "PrimitiveTriangle.h"
#include <DirectXMath.h>


class PrimitiveCube
{
public:

	template<RequirePos T>
	PrimitiveCube&& BuildPosition() &&
	{
		if (m_vertices<T>.empty()) m_vertices<T>.resize(24);
		constexpr float side = 1.0f / 2.0f;

		std::vector<DirectX::XMFLOAT3> vertices
		{
			DirectX::XMFLOAT3{-1.0f, 1.0f, -1.0f},
			DirectX::XMFLOAT3{1.0f, 1.0f, -1.0f},
			DirectX::XMFLOAT3{1.0f, 1.0f, 1.0f},
			DirectX::XMFLOAT3{-1.0f, 1.0f, 1.0f},
			DirectX::XMFLOAT3{-1.0f, -1.0f, -1.0f},
			DirectX::XMFLOAT3{1.0f, -1.0f, -1.0f},
			DirectX::XMFLOAT3{1.0f, -1.0f, 1.0f},
			DirectX::XMFLOAT3{-1.0f, -1.0f, 1.0f},
			DirectX::XMFLOAT3{-1.0f, -1.0f, 1.0f},
			DirectX::XMFLOAT3{-1.0f, -1.0f, -1.0f},
			DirectX::XMFLOAT3{-1.0f, 1.0f, -1.0f},
			DirectX::XMFLOAT3{-1.0f, 1.0f, 1.0f},
			DirectX::XMFLOAT3{1.0f, -1.0f, 1.0f},
			DirectX::XMFLOAT3{1.0f, -1.0f, -1.0f},
			DirectX::XMFLOAT3{1.0f, 1.0f, -1.0f},
			DirectX::XMFLOAT3{1.0f, 1.0f, 1.0f},
			DirectX::XMFLOAT3{-1.0f, -1.0f, -1.0f},
			DirectX::XMFLOAT3{1.0f, -1.0f, -1.0f},
			DirectX::XMFLOAT3{1.0f, 1.0f, -1.0f},
			DirectX::XMFLOAT3{-1.0f, 1.0f, -1.0f},
			DirectX::XMFLOAT3{-1.0f, -1.0f, 1.0f},
			DirectX::XMFLOAT3{1.0f, -1.0f, 1.0f},
			DirectX::XMFLOAT3{1.0f, 1.0f, 1.0f},
			DirectX::XMFLOAT3{-1.0f, 1.0f, 1.0f}
		};

		for (size_t i = 0; i < vertices.size(); i++)
		{
			m_vertices<T>[i].pos = vertices[i];
		}
		return std::move(*this);
	}

	template<RequireTex T>
	PrimitiveCube&& BuildTexcord() &&
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

	template<class V>
	static PrimitiveTriangle<V> GetTopology()
	{
		return
		{
			std::move(m_vertices<V>),
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
			}
		};
	}

private:
	template<class V>
	static std::vector<V> m_vertices;
};
