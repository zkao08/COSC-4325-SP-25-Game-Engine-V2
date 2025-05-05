#define NOMINMAX // May need for Windows specifically when using algorithm::min and algorithm::max (Windows already uses min/max keywords)

#include "Camera.h"

const float MIN_ZOOM = 2.0f; // Smallest distance user can zoom. Helps prevent camera clipping through 2D plane.
const float MAX_ZOOM = 180.0f; // Max distance user can zoom. Keep under MAX_RENDER_RANGE to prevent everything from disappearing.
const float MAX_RENDER_RANGE = 200.0f; // Maximum distance objects render relative to camera position and zoom.
const float DEFAULT_ZOOM = 10.0f; // Default zoom of the camera on initialization.

Camera::Camera(int width, int height)
{
	this->UpdateAspectRatio(width, height);
	Move(0, 0, DEFAULT_ZOOM);
}

void Camera::Move(float delta_x, float delta_y, float delta_z) {
	m_x += delta_x;
	m_y += delta_y;
	m_z += delta_z;

	if (m_z < MIN_ZOOM)
		m_z = MIN_ZOOM;
	if (m_z > MAX_ZOOM)
		m_z = MAX_ZOOM;

	m_View = DirectX::XMMatrixTranslation(m_x, m_y, m_z);
}

void Camera::Set(float x, float y, float z) {
	m_x = x;
	m_y = y;
	m_z = z;

	if (m_z < MIN_ZOOM)
		m_z = MIN_ZOOM;
	if (m_z > MAX_ZOOM)
		m_z = MAX_ZOOM;

	m_View = DirectX::XMMatrixTranslation(m_x, m_y, m_z);
}

void Camera::FocusOnObject(Object* object) {
	focusedObject = object;
}

void Camera::UpdateObjectPosition() {
	if (focusedObject != nullptr && focusedObject->GetProperty("Position") != "") {
		Vector2 position = StringToVector2(focusedObject->GetProperty("Position"));
		m_x = -position.x;
		m_y = -position.y;

		m_View = DirectX::XMMatrixTranslation(m_x, m_y, m_z);
		CalculateProjection();
	}
}

void Camera::Reset() {
	m_x = 0;
	m_y = 0;
	m_z = DEFAULT_ZOOM;
}

// Designed for 3D. May remove later.
/*void Camera::Rotate(float pitch_radians, float yaw_radians)
{
	m_PitchRadians += pitch_radians;
	m_YawRadians += yaw_radians;
	m_PitchRadians = std::max<float>(-(DirectX::XM_PIDIV2 - 0.1f), std::min(m_PitchRadians, DirectX::XM_PIDIV2 - 0.1f));

	// Convert Spherical to Cartesian coordinates.
	const float radius = -8.0f;
	DirectX::XMMATRIX rotation_matrix = DirectX::XMMatrixRotationRollPitchYaw(m_PitchRadians, m_YawRadians, 0);
	DirectX::XMVECTOR position = DirectX::XMVectorSet(0.0f, 0.0f, radius, 0.0f);
	position = XMVector3TransformCoord(position, rotation_matrix);

	// Calculate camera's view
	DirectX::XMVECTOR eye = position;
	DirectX::XMVECTOR at = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	m_View = DirectX::XMMatrixLookAtLH(eye, at, up);
}*/

void Camera::UpdateAspectRatio(int width, int height)
{
	// Calculate window aspect ratio
	m_AspectRatio = static_cast<float>(width) / height;
	CalculateProjection();
}

void Camera::UpdateFov(float fov)
{
	m_FieldOfViewDegrees += fov;
	m_FieldOfViewDegrees = std::max(0.1f, std::min(m_FieldOfViewDegrees, 179.9f));
	CalculateProjection();
}

void Camera::CalculateProjection()
{
	// Convert degrees to radians
	float field_of_view_radians = DirectX::XMConvertToRadians(m_FieldOfViewDegrees);

	// Calculate camera's perspective
	m_Projection = DirectX::XMMatrixPerspectiveFovLH(field_of_view_radians, m_AspectRatio, 0.01f, MAX_RENDER_RANGE);
}

Vector3 Camera::GetPosition() {
	return Vector3(-m_x, -m_y, m_z);
}