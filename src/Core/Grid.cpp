#include "Grid.h"
#include "Vertex.h"
#include "Renderer.h"

#include <vector>

#include <d3d11.h>
#include <DirectXMath.h>

struct GridVertex
{
    DirectX::XMFLOAT3 position; // Vertex position
    DirectX::XMFLOAT4 color;    // Vertex color
};

Grid::Grid(Renderer* renderer) : m_Renderer(renderer) {}

void Grid::Create(int gridSize, float spacing)
{
    ComPtr<ID3D11Device> device = m_Renderer->GetDevice();

    std::vector<GridVertex> vertices;
    std::vector<UINT> indices;

    // Create grid lines
    for (int i = -gridSize; i <= gridSize; ++i)
    {
        // Horizontal lines
        vertices.push_back({ DirectX::XMFLOAT3(-gridSize * spacing, 0.0f, i * spacing), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) });
        vertices.push_back({ DirectX::XMFLOAT3(gridSize * spacing, 0.0f, i * spacing), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) });

        // Vertical lines
        vertices.push_back({ DirectX::XMFLOAT3(i * spacing, 0.0f, -gridSize * spacing), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) });
        vertices.push_back({ DirectX::XMFLOAT3(i * spacing, 0.0f, gridSize * spacing), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) });
    }

    // Create indices
    for (int i = 0; i < vertices.size() / 2; ++i)
    {
        indices.push_back(i * 2);
        indices.push_back(i * 2 + 1);
    }

    // Create vertex buffer
    D3D11_BUFFER_DESC vertexBufferDesc = {};
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(Vertex) * vertices.size();
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA vertexData = {};
    vertexData.pSysMem = vertices.data();
    DX::Check(device->CreateBuffer(&vertexBufferDesc, &vertexData, m_VertexBuffer.GetAddressOf()));

    // Create index buffer
    D3D11_BUFFER_DESC indexBufferDesc = {};
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(UINT) * indices.size();
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;

    m_IndexCount = static_cast<UINT>(indices.size());

    D3D11_SUBRESOURCE_DATA indexData = {};
    indexData.pSysMem = indices.data();
    DX::Check(device->CreateBuffer(&indexBufferDesc, &indexData, m_IndexBuffer.GetAddressOf()));
}

void Grid::Render()
{
    ComPtr<ID3D11DeviceContext> context = m_Renderer->GetContext();

    UINT stride = sizeof(Vertex);
    UINT offset = 0;

    // Bind the vertex buffer
    context->IASetVertexBuffers(0, 1, m_VertexBuffer.GetAddressOf(), &stride, &offset);
    // Bind the index buffer
    context->IASetIndexBuffer(m_IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    // Set the primitive topology
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Draw the grid
    context->DrawIndexed(m_IndexCount, 0, 0);
}
