#pragma once

#include "PrimitiveTriangle.h"
#include <DirectXMath.h>

#include "PrimitiveBase.h"
#include "MathLib.h"


class PrimitiveCone
{
public:
	template<RequirePos T>
	static PrimitiveTriangle<T> Tessellate(int longDiv)
	{
		assert(longDiv >= 3 && "Triangle Must be containing at least 3 points");
		namespace dx = DirectX;

		const auto base = dx::XMVectorSet(1.0f, 0.0f, -1.0f, 0.0f);
		const float longitudeAngle = 2.0f * PI<float> / longDiv;

		// base vertices
		std::vector<T> vertices;
		for (int i = 0; i < longDiv; i++)
		{
			vertices.emplace_back();
			auto v = dx::XMVector3Transform(
				base,
				dx::XMMatrixRotationZ(longitudeAngle * i)
			);
			dx::XMStoreFloat3(&vertices.back().pos, v);
		}

		// the center
		vertices.emplace_back();
		vertices.back().pos = { 0.0f,0.0f,-1.0f };

		const auto iCenter = static_cast<unsigned short>(vertices.size() - 1);

		vertices.emplace_back();
		vertices.back().pos = { 0.0f,0.0f,1.0f };
		const auto iTip = static_cast<unsigned short>(vertices.size() - 1);

		// base indices
		std::vector<unsigned short> indices;
		for (unsigned short i = 0; i < longDiv; i++)
		{
			indices.push_back(iCenter);
			indices.push_back((i + 1) % longDiv);
			indices.push_back(i);
		}

		// cone indices
		for (unsigned short i = 0; i < longDiv; i++)
		{
			indices.push_back(i);
			indices.push_back((i + 1) % longDiv);
			indices.push_back(iTip);
		}

		return { std::move(vertices),std::move(indices) };
	}

	template<RequirePos T>
	static PrimitiveTriangle<T> Make()
	{
		return Tessellate<T>(24);
	}

};
