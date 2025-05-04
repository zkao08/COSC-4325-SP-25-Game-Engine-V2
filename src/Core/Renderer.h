#pragma once

#include <d3d11_1.h>
#include <exception>

#include "imgui.h"
#include "imgui_internal.h"

// This include is requires for using DirectX smart pointers (ComPtr)
#include <wrl\client.h>
using Microsoft::WRL::ComPtr;

class Application;

namespace DX
{
	// Throw if the function result has failed. For ease of error handling
	inline void Check(HRESULT hr)
	{
#ifdef _DEBUG
		if (FAILED(hr))
		{
			throw std::exception();
		}
#endif
	}
}

class Renderer
{
	Application* m_Application = nullptr;

public:
	float savedScaleFactor = 1.0f;

	Renderer(Application* application);
	~Renderer();

	// Creates the rendering device and context
	void Create();

	// Clear the buffers
	void Clear();

	// Display the rendered scene
	void Present();

	// Resizing
	void Resize(int width, int height);

	bool LoadTextureFromMemory(const void* data, size_t data_size, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height);
	bool LoadTextureFromFile(const char* file_name, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height);

	bool CreateImageButton(char* id, char* path, ImVec2 size);

	float GetScaleFactor();
	float GetScaleFactor(float width, float height);

	ComPtr<ID3D11Device> GetDevice();
	ComPtr<ID3D11DeviceContext> GetContext();
	ComPtr<ID3D11BlendState> GetBlendState();
	ComPtr<ID3D11SamplerState> GetSamplerState();

private:
	// Device and device context
	ComPtr<ID3D11Device> m_Device = nullptr;
	ComPtr<ID3D11DeviceContext> m_DeviceContext = nullptr;
	void CreateDeviceAndContext();

	// Swapchain
	ComPtr<IDXGISwapChain> m_SwapChain = nullptr;
	ComPtr<IDXGISwapChain1> m_SwapChain1 = nullptr;
	void CreateSwapChain(int width, int height);

	// Render target and depth stencil view
	ComPtr<ID3D11RenderTargetView> m_RenderTargetView = nullptr;
	ComPtr<ID3D11DepthStencilView> m_DepthStencilView = nullptr;
	void CreateRenderTargetAndDepthStencilView(int width, int height);

	ComPtr<ID3D11BlendState> m_BlendState = nullptr;
	void CreateBlendState();

	ComPtr<ID3D11SamplerState> m_SamplerState = nullptr;
	void CreateSamplerState();

	// Viewport
	void SetViewport(int width, int height);
};