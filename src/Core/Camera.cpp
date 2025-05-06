// Camera class
// Defines a point in the game world to render.

#define NOMINMAX // May need for Windows specifically when using algorithm::min and algorithm::max (Windows already uses min/max keywords)

#include "Camera.h"

const float MIN_ZOOM = 2.0f; // Smallest distance user can zoom. Helps prevent camera clipping through 2D plane.
const float MAX_ZOOM = 180.0f; // Max distance user can zoom. Keep under MAX_RENDER_RANGE to prevent everything from disappearing.
const float MAX_RENDER_RANGE = 200.0f; // Maximum distance objects render relative to camera position and zoom.
const float DEFAULT_ZOOM = 10.0f; // Default zoom of the camera on initialization.

// Initializes camera
Camera::Camera(int width, int height)
{
	this->UpdateAspectRatio(width, height);
	Move(0, 0, DEFAULT_ZOOM);
}

// Updates the camera's position
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

// Sets the camera's position
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

// Sets the camera to focus on a specific object
void Camera::FocusOnObject(Object* object) {
	focusedObject = object;
}

// Called every frame to update the camera's position to the set focused object, if any.
void Camera::UpdateObjectPosition() {
	if (focusedObject != nullptr && focusedObject->GetProperty("Position") != "") {
		Vector2 position = StringToVector2(focusedObject->GetProperty("Position"));
		m_x = -position.x;
		m_y = -position.y;

		m_View = DirectX::XMMatrixTranslation(m_x, m_y, m_z);
		CalculateProjection();
	}
}

// Resets the camera's position to the center world coordinates.
void Camera::Reset() {
	m_x = 0;
	m_y = 0;
	m_z = DEFAULT_ZOOM;
}

// Updates the camera's aspect ratio. Used to keep objects visually consistent regardless of system resolution and window size.
void Camera::UpdateAspectRatio(int width, int height)
{
	// Calculate window aspect ratio
	m_AspectRatio = static_cast<float>(width) / height;
	CalculateProjection();
}

// Updates the camera's field of view
void Camera::UpdateFov(float fov)
{
	m_FieldOfViewDegrees += fov;
	m_FieldOfViewDegrees = std::max(0.1f, std::min(m_FieldOfViewDegrees, 179.9f));
	CalculateProjection();
}

// Creates a projection to ensure the camera's properties render the game world properly
void Camera::CalculateProjection()
{
	// Convert degrees to radians
	float field_of_view_radians = DirectX::XMConvertToRadians(m_FieldOfViewDegrees);

	// Calculate camera's perspective
	m_Projection = DirectX::XMMatrixPerspectiveFovLH(field_of_view_radians, m_AspectRatio, 0.01f, MAX_RENDER_RANGE);
}

// Returns the current world position of the camera
Vector3 Camera::GetPosition() {
	return Vector3(-m_x, -m_y, m_z);
}