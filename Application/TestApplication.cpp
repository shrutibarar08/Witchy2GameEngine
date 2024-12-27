#include "TestApplication.h"
#include "Shape/Box.h"
#include "Shape/ShapePyramid.h"
#include "Utils/MathLib.h"

#include <memory>
#include <random>
#include <DirectXMath.h>
#include <algorithm>


TestApplication::TestApplication()
	: WitchyEngine({0, 0, 1280, 720}, 
		"Application_1")
{
	class Factory
	{
	public:
		Factory() = default;
		std::unique_ptr<Drawable> operator()()
		{
			switch (typedist(rng))
			{
			case 0:
				return std::make_unique<ShapePyramid>(
					rng, adist, ddist,
					odist, rdist
				);
			case 1:
				return std::make_unique<Box>(
					rng, adist, ddist,
					odist, rdist, bdist
				);
			default:
				assert(false && "bad drawable type in factory");
				return {};
			}
		}
	private:
		std::mt19937 rng{ std::random_device{}() };
		std::uniform_real_distribution<float> adist{ 0.0f,PI<float> * 2.0f };
		std::uniform_real_distribution<float> ddist{ 0.0f,PI<float> * 0.5f };
		std::uniform_real_distribution<float> odist{ 0.0f,PI<float> * 0.08f };
		std::uniform_real_distribution<float> rdist{ 6.0f,20.0f };
		std::uniform_real_distribution<float> bdist{ 0.4f,3.0f };
		std::uniform_int_distribution<int> latdist{ 5,20 };
		std::uniform_int_distribution<int> longdist{ 10,40 };
		std::uniform_int_distribution<int> typedist{ 0,1};
	};

	Factory f{};
	m_objects.reserve(120);
	std::generate_n(std::back_inserter(m_objects), 120, f);


	W2RenderAPI::Get()->SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 40.0f));
}

void TestApplication::BeginPlay()
{

}

void TestApplication::Tick(float deltaTime)
{
	for (auto& b: m_objects)
	{
		b->Update(deltaTime);
		b->Draw();
	}
}
