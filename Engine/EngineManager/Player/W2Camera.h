#pragma once
#include <DirectXMath.h>


// TODO: Create Engine Component Factory Interface.
class W2Camera
{
public:

	struct W2Camera_INIT_DESC
	{
		DirectX::XMMATRIX WorldMatrix;	// expecting Identity Matrix with xyz...
		DirectX::XMMATRIX ProjectionMatrix;
	};

	static void Init(const W2Camera_INIT_DESC& desc) noexcept;	// TODO: do something for saving and loading initial camera location
	static W2Camera* Get();
	static void Release();
	static void UpdateFreeCamera(float deltaTime);
	void UpdateFocusCamera(float deltaTime); // TODO: THIS SHOULD STAY FOCUSED ON A MESH TRANSFORM.

	void InitControlWindow() noexcept;

	DirectX::XMMATRIX GetViewMatrix() const noexcept;
	DirectX::XMMATRIX GetWorldMatrix() const noexcept;
	DirectX::XMMATRIX GetProjectionMatrix() const noexcept;

	W2Camera(const W2Camera&) = delete;
	W2Camera& operator=(const W2Camera&) = delete;
	W2Camera(W2Camera&&) = delete;
	W2Camera& operator=(W2Camera&&) = delete;


	//~ Features
	void Rotate(float dx, float dy) noexcept;
	void Translate(DirectX::XMFLOAT3 translation) noexcept;

private:
	W2Camera(const W2Camera_INIT_DESC& desc);
	~W2Camera() = default;

private:
	static W2Camera* m_instance;
	// TODO: Replace it with W2Camera Position, Rotation and quaternion vectors
	DirectX::XMFLOAT3 m_translation{0, 0, 0};
	float m_pitch = 0.0f;
	float m_yaw = 0.0f;

	//~ Test
	static constexpr float m_moveSpeed{ 10.f };
	static constexpr float m_rotationSpeed{ 0.0035f };

	DirectX::XMMATRIX mWorldMatrix;
	DirectX::XMMATRIX mProjectionMatrix;
};
