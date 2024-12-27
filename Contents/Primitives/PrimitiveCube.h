#pragma once

#include "PrimitiveTriangle.h"
#include <DirectXMath.h>


class PrimitiveCube
{
public:
	template<typename T>
	static PrimitiveTriangle<T> Make()
	{

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

		std::vector<T> out_vertices(vertices.size());
		for (size_t i = 0; i < vertices.size(); i++)
		{
			out_vertices[i].pos = vertices[i];
		}
		return
		{
			std::move(out_vertices),
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
};
