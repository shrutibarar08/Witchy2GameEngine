#include "TestApplication.h"
#include "Shape/Box.h"
#include "Shape/ShapePyramid.h"
#include "MathLib.h"

#include <memory>
#include <random>
#include <DirectXMath.h>
#include <algorithm>

#include "ImuGui/imgui_impl_dx11.h"


/*
 * @brief Tests Engine Functionality
 */
TestApplication::TestApplication()
	: WitchyEngine({0, 0, 1920, 1080}, 
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
		std::uniform_int_distribution<int> typedist{ 1,1};
	};

	Factory f{};
	m_objects.reserve(m_meshCount);
	std::generate_n(std::back_inserter(m_objects), m_meshCount, f);
}

void TestApplication::BeginPlay()
{
	m_availableTextures.push_back("Contents/Textures/CubTextures/pexels_didsss.dds");
	m_availableTextures.push_back("Contents/Textures/CubTextures/tie_dye.dds");
	m_availableTextures.push_back("Contents/Textures/CubTextures/crumpled_black_paper.dds");
	m_availableTextures.push_back("Contents/Textures/Coin.dds");
	m_availableTextures.push_back("Contents/Textures/rocks.dds");
	m_availableTextures.push_back("Contents/Textures/Tiles.dds");
	m_availableTextures.push_back("Contents/Textures/Wood.dds");
	currentTexture = "";
	// currentTexture = m_objects[0]->GetTexture()->GetTopTexture();
}

void TestApplication::Tick(float deltaTime)
{
	m_light.Bind();
	for (int i = 0; i < m_meshCount && i < m_objects.size(); i++)
	{
		m_objects[i]->Update(deltaTime * m_speedFactor);
		m_objects[i]->Draw();
		//if (currentTexture != m_objects[i]->GetTexture()->GetTopTexture())
		//{
		//	m_objects[i]->GetTexture()->UpdateTexture(currentTexture);
		//}
	}
	m_light.Draw();
}

void TestApplication::DebugUI()
{
	WitchyEngine::DebugUI(); // TODO: Implement another way for this please
	m_light.InitControlWindow();
	if (ImGui::Begin("Simulation"))
	{
		ImGui::SliderFloat("Speed Factor", &m_speedFactor, 0.0f, 4.0f);
		ImGui::Text("Application average %.3f", 1000.f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::SliderInt("Mesh Count", &m_meshCount, 0, 100);
		ImGui::End();
	}

	if (ImGui::BeginCombo("Select Options", currentTexture.c_str()))
	{
		for (size_t i = 0; i < m_availableTextures.size(); i++)
		{
			bool isSelected = (currentTexture == m_availableTextures[i]);
			if (ImGui::Selectable(m_availableTextures[i].c_str(), isSelected))
			{
				currentTexture = m_availableTextures[i];
			}
			if (isSelected) ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
}
