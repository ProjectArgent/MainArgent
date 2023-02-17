#pragma once
#include <DirectXMath.h>
#include "../ArComponent.h"
#include "Transform.h"

class GameObject;

class Camera final:
	public Argent::Component::ArComponent
{
public:
	Camera(bool isSceneCamera, float width, float height, float nearZ = 0.01f, float farZ = 100.0f, float fov = 60.0f,
		const Transform* target = nullptr);
	~Camera() override = default;

	void Reset() override;
	void Initialize() override;
	void Update() override;
#ifdef _DEBUG
	void DrawDebug() override;
#endif
	[[nodiscard]] DirectX::XMMATRIX GetViewProjectionMatrix() const;
	[[nodiscard]] DirectX::XMMATRIX GetViewMatrix() const;
	[[nodiscard]] DirectX::XMMATRIX GetProjectionMatrix() const;

	bool GetIsSceneCamera() const { return isSceneCamera; }

	void OnSceneCamera() { isSceneCamera = true; }
	void OffSceneCamera() { isSceneCamera = false; }

	DirectX::XMFLOAT3 GetForward() { return forward; }
	DirectX::XMFLOAT3 GetRight() { return right; }
	DirectX::XMFLOAT3 GetUp() { return up; }

	void SetTarget(const Transform* t) { target = t; }


private:
	float width;
	float height;
	float nearZ;
	float farZ;
	float fov;
	float aspectRatio;

	bool isSceneCamera;

	DirectX::XMFLOAT3 forward;
	DirectX::XMFLOAT3 right;
	DirectX::XMFLOAT3 up;
	static constexpr DirectX::XMVECTOR ForwardVec = { 0, 0, 1 };
	static constexpr DirectX::XMVECTOR RightVec = { 1, 0, 0 };
	static constexpr DirectX::XMVECTOR UpVec = { 0, 1, 0 };
	const Transform* target;
};


class CameraController:
	public Argent::Component::ArComponent
{
public:
	CameraController();
	~CameraController() override = default;

	void Update();
#ifdef _DEBUG
	void DrawDebug() override;
#endif
private:
	float rotationSpeed;
	float vertAndHorMoveSpeed;
	float depthMoveSpeed;
};