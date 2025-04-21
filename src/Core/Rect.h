#pragma once

#include "Vector2.h"

#include <Windows.h>
#include <d3d11.h>
#include <string>

// This include is requires for using DirectX smart pointers (ComPtr)
#include <wrl\client.h>
using Microsoft::WRL::ComPtr;

class Renderer;

class Rect
{
	Renderer* m_Renderer = nullptr;

public:
	Rect(Renderer* renderer);
	virtual ~Rect() = default;

	// Create the model
	void Create();
	void Create(std::wstring path);
	void Create(std::wstring path, float pos_x, float pos_y);

	// Render the model
	void Render();

	void ChangePosition(float x, float y);
	void ChangePosition(Vector2 pos);

private:
	// Number of indices to draw
	UINT m_IndexCount = 0;

	// Vertex buffer
	void CreateVertexBuffer(float pos_x, float pos_y);
	ComPtr<ID3D11Buffer> m_VertexBuffer = nullptr;

	// Index buffer
	void CreateIndexBuffer();
	ComPtr<ID3D11Buffer> m_IndexBuffer = nullptr;

	// Texture buffer
	void LoadTexture(std::wstring path);
	ComPtr<ID3D11ShaderResourceView> m_DiffuseTexture = nullptr;
};