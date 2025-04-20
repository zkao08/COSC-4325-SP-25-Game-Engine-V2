// Object Class
// 2D triangle object.

#include "Renderer.h"
#include "Object.h"
#include "d3d11.h"
#include <vector>

// This include is required for using DirectX smart pointers (ComPtr)
#include <wrl\client.h>
using Microsoft::WRL::ComPtr;

class Triangle : public Object {
	private:
		UINT m_VertexCount = 0;
		ComPtr<ID3D11Buffer> m_VertexBuffer = nullptr;
		Renderer* m_Renderer;

		struct Vertex {
			float x = 0.0f;
			float y = 0.0f;
			float z = 0.0f;
		};
	public:
		char* className = "Triangle";

		std::map<char*, void*> properties{
			{"Name", &name},
			{"Class Name", &className},
			{"Position", &position},
			{"Rotation", &rotation}
		};

		Triangle(char* new_name, Renderer* renderer) {
			name = new_name;
			position = Vector2(0.0f, 0.0f);
			rotation = 0.0f;
			m_Renderer = renderer;
		}

		void Create() {
			ComPtr<ID3D11Device> device = m_Renderer->GetDevice();
			std::vector<Vertex> vertices =
			{
				{ +0.0f, +0.5f, 0.0f }, // Top vertex
				{ +0.5f, -0.5f, 0.0f }, // Right vertex
				{ -0.5f, -0.5f, 0.0f }  // Left vertex
			};

			m_VertexCount = static_cast<UINT>(vertices.size());

			// Create vertex buffer
			D3D11_BUFFER_DESC vertexbuffer_desc = {};
			vertexbuffer_desc.Usage = D3D11_USAGE_DEFAULT;
			vertexbuffer_desc.ByteWidth = static_cast<UINT>(sizeof(Vertex) * vertices.size());
			vertexbuffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

			D3D11_SUBRESOURCE_DATA vertex_subdata = {};
			vertex_subdata.pSysMem = vertices.data();

			DX::Check(device->CreateBuffer(&vertexbuffer_desc, &vertex_subdata, m_VertexBuffer.ReleaseAndGetAddressOf()));
		}

		void Render() {
			UINT stride = sizeof(Vertex);
			UINT offset = 0;

			ComPtr<ID3D11DeviceContext> context = m_Renderer->GetContext();

			// Bind the vertex buffer to the pipeline's Input Assembler stage
			context->IASetVertexBuffers(0, 1, m_VertexBuffer.GetAddressOf(), &stride, &offset);
			// Bind the geometry topology to the pipeline's Input Assembler stage
			context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			// Render geometry
			context->Draw(m_VertexCount, 0);
		}
};