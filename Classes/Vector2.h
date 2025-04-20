// Vector2 Data Type Class
// Represents 2D coordinates.

#pragma once

class Vector2 {
	public:
		float x = 0.0f;
		float y = 0.0f;
		Vector2() { x = 0.0f; y = 0.0f; }
		Vector2(float new_x, float new_y) { x = new_x; y = new_y; }
};