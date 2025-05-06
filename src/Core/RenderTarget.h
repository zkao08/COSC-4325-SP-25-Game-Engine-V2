// RenderTarget class
// Configures rendering result and sets it to an image

#pragma once

#include "Renderer.h"

#include <DirectXColors.h>

class RenderTarget {
	private:
		// Components
		Renderer* m_Renderer = nullptr;
		ComPtr<ID3D11Device> m_Device = nullptr;
		ComPtr<ID3D11DeviceContext> m_DeviceContext = nullptr;

		// Render texture
		ComPtr<ID3D11Texture2D> m_Texture = nullptr;
		void CreateRenderTexture(int width, int height);

		// Render target and depth stencil
		ComPtr<ID3D11RenderTargetView> m_TextureRenderTargetView = nullptr;
		ComPtr<ID3D11DepthStencilView> m_TextureDepthStencilView = nullptr;
		void CreateRenderTargetAndDepthStencilView(int width, int height);

		// Shader resource view
		ComPtr<ID3D11ShaderResourceView> m_TextureShaderResource = nullptr;
		void CreateShaderResource();
	public:
		RenderTarget(Renderer* renderer);
		virtual ~RenderTarget() = default;

		// Initializer and updater
		void Create(int width, int height);
		void Use();

		// Get render texture
		ID3D11ShaderResourceView* GetTexture() const { return m_TextureShaderResource.Get(); }
};