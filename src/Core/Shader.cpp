// Shader class
// Handles compiled shaders used to determine how objects are visually rendered.

#include "Shader.h"

namespace
{
	struct ModelViewProjectionBuffer
	{
		DirectX::XMMATRIX modelViewProjection;
	};
}

Shader::Shader(Renderer* renderer) {
	m_Renderer = renderer;
}

// Initialize shader
void Shader::Load() {
	this->LoadVertexShader();
	this->LoadPixelShader();
	this->CreateWorldViewProjectionConstantBuffer();
}

// Update shader
void Shader::Use() {
	ComPtr<ID3D11DeviceContext> context = m_Renderer->GetContext();

	// Bind the input layout to the pipeline's Input Assembler stage
	context->IASetInputLayout(m_VertexLayout.Get());

	// Bind the vertex shader to the pipeline's Vertex Shader stage
	context->VSSetShader(m_VertexShader.Get(), nullptr, 0);

	// Bind the pixel shader to the pipeline's Pixel Shader stage
	context->PSSetShader(m_PixelShader.Get(), nullptr, 0);

	// Bind the world constant buffer to the vertex shader
	const int constant_buffer_slot = 0;
	context->VSSetConstantBuffers(constant_buffer_slot, 1, m_ModelViewProjectionConstantBuffer.GetAddressOf());
}

// Load vertex shader
// Handles how shapes are rendered.
void Shader::LoadVertexShader() {
	ComPtr<ID3D11Device> device = m_Renderer->GetDevice();

	// Create the vertex shader
	DX::Check(device->CreateVertexShader(g_VertexShader, sizeof(g_VertexShader), nullptr, m_VertexShader.ReleaseAndGetAddressOf()));

	// Describe the memory layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXTURE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT number_elements = ARRAYSIZE(layout);
	DX::Check(device->CreateInputLayout(layout, number_elements, g_VertexShader, sizeof(g_VertexShader), m_VertexLayout.ReleaseAndGetAddressOf()));
}

// Load pixel shader
// Handles how colors are rendered.
void Shader::LoadPixelShader() {
	ComPtr<ID3D11Device> device = m_Renderer->GetDevice();
	device->CreatePixelShader(g_PixelShader, sizeof(g_PixelShader), nullptr, m_PixelShader.ReleaseAndGetAddressOf());
}


// Initialize projection
void Shader::CreateWorldViewProjectionConstantBuffer()
{
	ComPtr<ID3D11Device> device = m_Renderer->GetDevice();

	// Create world constant buffer
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ModelViewProjectionBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	DX::Check(device->CreateBuffer(&bd, nullptr, m_ModelViewProjectionConstantBuffer.ReleaseAndGetAddressOf()));
}

// Update projection
void Shader::UpdateModelViewProjectionBuffer(const DirectX::XMMATRIX& matrix)
{
	ModelViewProjectionBuffer buffer = {};
	buffer.modelViewProjection = DirectX::XMMatrixTranspose(matrix);

	ComPtr<ID3D11DeviceContext> context = m_Renderer->GetContext();
	context->UpdateSubresource(m_ModelViewProjectionConstantBuffer.Get(), 0, nullptr, &buffer, 0, 0);
}