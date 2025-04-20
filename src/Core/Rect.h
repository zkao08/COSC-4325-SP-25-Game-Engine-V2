#pragma once

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
	void Create(std::wstring path);

	// Render the model
	void Render();

private:
	// Number of indices to draw
	UINT m_IndexCount = 0;

	// Vertex buffer
	void CreateVertexBuffer();
	ComPtr<ID3D11Buffer> m_VertexBuffer = nullptr;

	// Index buffer
	void CreateIndexBuffer();
	ComPtr<ID3D11Buffer> m_IndexBuffer = nullptr;

	// Texture buffer
	void LoadTexture(std::wstring path);
	ComPtr<ID3D11ShaderResourceView> m_DiffuseTexture = nullptr;
};