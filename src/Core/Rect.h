#pragma once

#include "Vector2.h"

#include <Windows.h>
#include <d3d11.h>
#include <string>
#include <DirectXMath.h>
#include "Utility.h"

// This include is requires for using DirectX smart pointers (ComPtr)
#include <wrl\client.h>
using Microsoft::WRL::ComPtr;

class Renderer;

class Rect
{
	private:
		Renderer* m_Renderer = nullptr;

		// Number of indices to draw
		UINT m_IndexCount = 0;

		// Vertex buffer
		void CreateVertexBuffer(float pos_x = 0.0f, float pos_y = 0.0f, float size_x = 1.0f, float size_y = 1.0f, float rotation = 0.0f);
		ComPtr<ID3D11Buffer> m_VertexBuffer = nullptr;

		// Index buffer
		void CreateIndexBuffer();
		ComPtr<ID3D11Buffer> m_IndexBuffer = nullptr;

		// Texture buffer
		void LoadTexture(std::wstring path);
		ComPtr<ID3D11ShaderResourceView> m_DiffuseTexture = nullptr;

		float lastPosX = -1.0f;
		float lastPosY = -1.0f;
		float lastSizeX = -1.0f;
		float lastSizeY = -1.0f;
		float lastRotation = -1.0f;
		std::wstring lastTexture;
	public:
		Rect(Renderer* renderer);
		virtual ~Rect() = default;

		// Create the model
		void Create(std::wstring path = GetProjectRootWString() + L"/assets/Square.png", float pos_x = 0.0f, float pos_y = 0.0f, float size_x = 1.0f, float size_y = 1.0f, float rotation = 0.0f);

		// Render the model
		void Render();

		void SetTransform(float pos_x = 0.0f, float pos_y = 0.0f, float size_x = 1.0f, float size_y = 1.0f, float rotation = 0.0f);
		void SetTransform(Vector2 pos = Vector2(0, 0), Vector2 size = Vector2(1, 1), float rotation = 0.0f);
		void SetTexture(std::string path);
};