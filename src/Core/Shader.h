#pragma once

#include "Renderer.h"
#include <d3d11.h>
#include <DirectXMath.h>

#include <Windows.h>

#include "PixelShader.hlsl.h"
#include "VertexShader.hlsl.h"

// This include is requires for using DirectX smart pointers (ComPtr)
#include <wrl\client.h>
using Microsoft::WRL::ComPtr;

class Shader {
	private:
		ComPtr<ID3D11Buffer> m_ModelViewProjectionConstantBuffer = nullptr;

		ComPtr<ID3D11VertexShader> m_VertexShader = nullptr;
		ComPtr<ID3D11InputLayout> m_VertexLayout = nullptr;
		ComPtr<ID3D11PixelShader> m_PixelShader = nullptr;
		Renderer* m_Renderer = nullptr;

		// Create vertex shader
		void LoadVertexShader();

		// Create pixel shader
		void LoadPixelShader();

		// ModelViewProjection constant buffer
		void CreateWorldViewProjectionConstantBuffer();
	public:
		Shader(Renderer* renderer);
		virtual ~Shader() = default;

		// Load the shader
		void Load();

		// Bind shader to the pipeline
		void Use();

		// Update the model view projection constant buffer
		void UpdateModelViewProjectionBuffer(const DirectX::XMMATRIX& matrix);
};