// RasterState class
// Handles toggling wireframe visibility in the viewport.

#pragma once

#include <d3d11.h>

#include <wrl\client.h>
using Microsoft::WRL::ComPtr;

class Renderer;

class RasterState {
	private:
		// Pointer to Renderer
		Renderer* m_Renderer = nullptr;
		
		// Wireframe toggle
		bool m_RenderWireframe = false;

		// DirectX states
		ComPtr<ID3D11RasterizerState> m_RasterStateSolid = nullptr;
		void CreateRasterStateSolid();

		ComPtr<ID3D11RasterizerState> m_RasterStateWireframe = nullptr;
		void CreateRasterStateWireframe();
	public:
		// Constructor
		RasterState(Renderer* renderer);
		virtual ~RasterState() = default;

		// Bind the raster state to the pipeline
		void Use();

		// Toggle between solid and wireframe
		void ToggleWireframe();
};