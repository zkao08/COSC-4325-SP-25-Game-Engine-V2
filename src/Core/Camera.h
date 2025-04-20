#pragma once

#include <DirectXMath.h>

// Perspective orbital camera
class Camera
{
public:
	Camera(int width, int height);
	virtual ~Camera() = default;

	void Move(float delta_x, float delta_y, float z);

	// Recalculates the view based on the pitch and yaw
	void Rotate(float pitch, float yaw);

	void Reset();

	// Update aspect ratio
	void UpdateAspectRatio(int width, int height);

	// Set field of view
	void UpdateFov(float fov);

	// Get projection matrix
	inline DirectX::XMMATRIX GetProjection() const { return m_Projection; }

	// Get view matrix
	inline DirectX::XMMATRIX GetView() const { return m_View; }

private:
	// Projection matrix
	DirectX::XMMATRIX m_Projection;

	// View matrix
	DirectX::XMMATRIX m_View;

	float m_x = 0.0f;
	float m_y = 0.0f;
	float m_z = 0.0f;

	// Camera pitch in radians
	float m_PitchRadians = 0.0f;

	// Camera yaw in radians
	float m_YawRadians = 0.0f;

	// Camera field of view in degrees
	float m_FieldOfViewDegrees = 50.0f;

	// Aspect ratio
	float m_AspectRatio = 0.0f;

	// Recalculates the projection based on the new window size
	void CalculateProjection();
};