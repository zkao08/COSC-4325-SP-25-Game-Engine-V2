// Camera class
// Defines a point in the game world to render.

#pragma once

#include "Vector3.h"
#include "Object.h"

#include <DirectXMath.h>
#include <algorithm>

class Object;

class Camera {
	private:
		// Projection matrix
		DirectX::XMMATRIX m_Projection;

		// View matrix
		DirectX::XMMATRIX m_View;

		// Object that camera follows
		Object* focusedObject = nullptr;

		// Variables
		float m_x = 0.0f;
		float m_y = 0.0f;
		float m_z = 0.0f;

		float m_PitchRadians = 0.0f;
		float m_YawRadians = 0.0f;
		float m_FieldOfViewDegrees = 50.0f;
		float m_AspectRatio = 0.0f;

		// Get camera projection
		void CalculateProjection();
	public:
		// Constructor and Destructor
		Camera(int width, int height);
		virtual ~Camera() = default;

		// Transform methods
		void Move(float delta_x, float delta_y, float z);
		void Set(float x, float y, float z);

		// Set camera to focus on object
		void Camera::FocusOnObject(Object* object = nullptr);

		// Set camera position to focused object position
		void Camera::UpdateObjectPosition();

		// Reset camera position
		void Reset();

		// Update aspect ratio
		void UpdateAspectRatio(int width, int height);

		// Set field of view
		void UpdateFov(float fov);

		// Get camera world position
		Vector3 GetPosition();

		// Get projection matrix
		inline DirectX::XMMATRIX GetProjection() const { return m_Projection; }

		// Get view matrix
		inline DirectX::XMMATRIX GetView() const { return m_View; }
};