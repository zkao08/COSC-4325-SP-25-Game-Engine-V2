// Shader class
// Handles compiled shaders used to determine how objects are visually rendered.

#pragma once

#include "Renderer.h"

#include "PixelShader.hlsl.h"
#include "VertexShader.hlsl.h"

#include <Windows.h>
#include <d3d11.h>
#include <DirectXMath.h>

#include <wrl\client.h>
using Microsoft::WRL::ComPtr;

class Shader {
	private:
		// Components
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