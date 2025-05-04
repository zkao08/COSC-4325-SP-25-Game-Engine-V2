#include "Rect.h"
#include "Renderer.h"
#include "Vertex.h"
#include "WICTextureLoader.h"
#include <vector>
#include <string>
#include <fstream>
#include <filesystem>
#include <Windows.h>
#include <iostream>
#include "PathUtils.h"
#include "ResourceManager.h"

Rect::Rect(Renderer* renderer) : m_Renderer(renderer) {}

void Rect::Create(std::wstring texture_path, float pos_x, float pos_y, float size_x, float size_y, float rotation)
{
	this->CreateVertexBuffer(pos_x, pos_y, size_x, size_y);
	this->CreateIndexBuffer();
	this->LoadTexture(texture_path);
}

void Rect::CreateVertexBuffer(float pos_x, float pos_y, float size_x, float size_y, float rotation)
{
	ComPtr<ID3D11Device> device = m_Renderer->GetDevice();

	const float width = size_x / 2.0f;  // Half width
	const float height = size_y / 2.0f; // Half height
	const float depth = 0.1f;

	float pos_z = 0.0f;

	// Rotation matrix (in radians)
	DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationZ(rotation * (DirectX::XM_PI / 180.0f));

	// Vertex data defined relative to the center
	std::vector<Vertex> vertices =
	{
		{ VertexPosition(-width, -height, pos_z - depth), VertexTextureUV(0.0f, 1.0f) },
		{ VertexPosition(-width, height, pos_z - depth), VertexTextureUV(0.0f, 0.0f) },
		{ VertexPosition(width, height, pos_z - depth), VertexTextureUV(1.0f, 0.0f) },
		{ VertexPosition(width, -height, pos_z - depth), VertexTextureUV(1.0f, 1.0f) },
	};

	// Apply rotation and translation to vertices
	for (auto& vertex : vertices)
	{
		// Create a vector for the vertex position
		DirectX::XMVECTOR position = DirectX::XMVectorSet(vertex.position.x, vertex.position.y, vertex.position.z, 1.0f);

		// Apply the rotation around the origin (0, 0)
		position = DirectX::XMVector3Transform(position, rotationMatrix);

		// Translate back to the specified position
		position = DirectX::XMVectorAdd(position, DirectX::XMVectorSet(pos_x, pos_y, 0.0f, 0.0f));

		// Update the vertex position
		vertex.position.x = DirectX::XMVectorGetX(position);
		vertex.position.y = DirectX::XMVectorGetY(position);
	}

	// Create vertex buffer
	D3D11_BUFFER_DESC vertexbuffer_desc = {};
	vertexbuffer_desc.Usage = D3D11_USAGE_DEFAULT;
	vertexbuffer_desc.ByteWidth = static_cast<UINT>(sizeof(Vertex) * vertices.size());
	vertexbuffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA vertex_subdata = {};
	vertex_subdata.pSysMem = vertices.data();

	DX::Check(device->CreateBuffer(&vertexbuffer_desc, &vertex_subdata, m_VertexBuffer.ReleaseAndGetAddressOf()));
}


void Rect::CreateIndexBuffer()
{
	ComPtr<ID3D11Device> device = m_Renderer->GetDevice();

	// Set Indices
	std::vector<UINT> indices =
	{
		0, 1, 2,
		0, 2, 3,
	};

	m_IndexCount = static_cast<UINT>(indices.size());

	// Create index buffer
	D3D11_BUFFER_DESC index_buffer_desc = {};
	index_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	index_buffer_desc.ByteWidth = static_cast<UINT>(sizeof(UINT) * indices.size());
	index_buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA index_subdata = {};
	index_subdata.pSysMem = indices.data();

	DX::Check(device->CreateBuffer(&index_buffer_desc, &index_subdata, m_IndexBuffer.ReleaseAndGetAddressOf()));
}

void Rect::LoadTexture(std::wstring path)
{
	if (path == lastTexture)
		return;

	lastTexture = path;

	// Check if file exists
	if (!std::filesystem::exists(path))
	{
		std::cout << "Error loading file." << std::endl;
		return;
	}

	// Load texture into a resource shader view
	ComPtr<ID3D11Device> device = m_Renderer->GetDevice();
	ComPtr<ID3D11DeviceContext> context = m_Renderer->GetContext();

	ComPtr<ID3D11Resource> resource = nullptr;
	DX::Check(DirectX::CreateWICTextureFromFile(device.Get(), context.Get(), path.c_str(), resource.ReleaseAndGetAddressOf(), m_DiffuseTexture.ReleaseAndGetAddressOf()));
}

void Rect::Render()
{
	ComPtr<ID3D11DeviceContext> context = m_Renderer->GetContext();

	// We need to define the stride and offset
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	// Set the blend state
	context->OMSetBlendState(m_Renderer->GetBlendState().Get(), nullptr, 0xffffffff);

	// Bind the vertex buffer to the pipeline's Input Assembler stage
	context->IASetVertexBuffers(0, 1, m_VertexBuffer.GetAddressOf(), &stride, &offset);

	// Bind the index buffer to the pipeline's Input Assembler stage
	context->IASetIndexBuffer(m_IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	// Bind the geometry topology to the pipeline's Input Assembler stage
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Bind sampler to pixel shader
	context->PSSetSamplers(0, 1, m_Renderer->GetSamplerState().GetAddressOf());

	// Bind texture to the pixel shader
	context->PSSetShaderResources(0, 1, m_DiffuseTexture.GetAddressOf());

	// Render geometry
	context->DrawIndexed(m_IndexCount, 0, 0);
}

void Rect::SetTransform(float pos_x, float pos_y, float size_x, float size_y, float rotation) {
	CreateVertexBuffer(pos_x, pos_y, size_x, size_y, rotation);
}

void Rect::SetTransform(Vector2 pos, Vector2 size, float rotation) {
	CreateVertexBuffer(pos.x, pos.y, size.x, size.y, rotation);
}