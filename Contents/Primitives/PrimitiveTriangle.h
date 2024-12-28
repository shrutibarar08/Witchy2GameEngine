#pragma once
#include "PrimitiveBase.h"
#include <DirectXMath.h>
#include <vector>



template<Requirements T>
class PrimitiveTriangle
{
public:
	PrimitiveTriangle() = default;
	PrimitiveTriangle(std::vector<T> in_vertices, std::vector<unsigned short> in_indices)
		: m_vertices(std::move(in_vertices)), m_indices(std::move(in_indices))
	{
		assert(m_vertices.size() && "Input Vertices contains no vertex data");
		assert(m_indices.size() && "Input Indices contains no index data");
	}

	void Transform(DirectX::XMMATRIX matrix)
	{
		for (auto& v: m_vertices)
		{
			const DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&v.pos);
			DirectX::XMStoreFloat3
			(
				&v.pos,
				DirectX::XMVector3Transform(pos, matrix)
			);
		}
	}

public:
	std::vector<T> m_vertices;
	std::vector<unsigned short> m_indices;
};
