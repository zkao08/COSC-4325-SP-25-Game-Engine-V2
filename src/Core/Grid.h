#pragma once

#include <d3d11.h>

#include <wrl\client.h>
using Microsoft::WRL::ComPtr;

class Renderer;

class Grid
{
public:
	Grid(Renderer* renderer);
	virtual ~Grid() = default;

	// Create the model
	void Create(int gridSize = 100, float spacing = 5.0);

	// Render the model
	void Render();

private:
	// Number of indices to draw
	UINT m_IndexCount = 0;

	ComPtr<ID3D11Buffer> m_VertexBuffer = nullptr;
	ComPtr<ID3D11Buffer> m_IndexBuffer = nullptr;

	Renderer* m_Renderer = nullptr;
};