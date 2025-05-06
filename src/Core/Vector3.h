// Vector2 Data Type Class
// Represents 3D coordinates.
// This isn't strictly used for actual 3D, and may be used for other applications that require 3 values.

#pragma once

class Vector3 {
	public:
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
		Vector3() { x = 0.0f; y = 0.0f; z = 0.0f; }
		Vector3(float new_x, float new_y, float new_z) { x = new_x; y = new_y; z = new_z; }
};