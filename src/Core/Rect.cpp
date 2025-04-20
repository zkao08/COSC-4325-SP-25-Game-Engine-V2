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

Rect::Rect(Renderer* renderer) : m_Renderer(renderer) {}

void Rect::Create(std::wstring texture_path)
{
	CreateVertexBuffer();
	CreateIndexBuffer();
	LoadTexture(texture_path);
}

void Rect::CreateVertexBuffer()
{
	ComPtr<ID3D11Device> device = m_Renderer->GetDevice();

	const float width = 1.0f;
	const float height = 1.0f;
	const float depth = 0.1f;

	// Vertex data
	std::vector<Vertex> vertices =
	{
		{ VertexPosition(-width, -height, -depth), VertexTextureUV(0.0f, 1.0f) },
		{ VertexPosition(-width, +height, -depth), VertexTextureUV(0.0f, 0.0f) },
		{ VertexPosition(+width, +height, -depth), VertexTextureUV(1.0f, 0.0f) },
		{ VertexPosition(+width, -height, -depth), VertexTextureUV(1.0f, 1.0f) },
	};

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
	//std::wstring path = L"../../../assets/WoodTexture.jpg";

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

	// Bind the vertex buffer to the pipeline's Input Assembler stage
	context->IASetVertexBuffers(0, 1, m_VertexBuffer.GetAddressOf(), &stride, &offset);

	// Bind the index buffer to the pipeline's Input Assembler stage
	context->IASetIndexBuffer(m_IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	// Bind the geometry topology to the pipeline's Input Assembler stage
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Bind texture to the pixel shader
	context->PSSetShaderResources(0, 1, m_DiffuseTexture.GetAddressOf());

	// Render geometry
	context->DrawIndexed(m_IndexCount, 0, 0);
}