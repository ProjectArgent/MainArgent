#include "ArStaticMesh.h"
#include "../Graphic/ArGraphics.h"



namespace Argent::Mesh::StaticMesh
{
	ArStaticMesh::ArStaticMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
	{
		ID3D12Device* device = Graphics::ArGraphics::Instance()->GetDevice();
		vertexBuffer = std::make_unique<Argent::Dx12::ArVertexBuffer<Vertex>>(device, vertices);
		indexBuffer = std::make_unique<Argent::Dx12::ArIndexBuffer>(device, indices);

		indexCountPerInstance = static_cast<UINT>(indices.size());
	}

	void ArStaticMesh::Render(ID3D12GraphicsCommandList* cmdList, const DirectX::XMFLOAT4X4& world,
		const DirectX::XMFLOAT4& color, UINT instanceCount, UINT indexOffset, INT vertexOffset,
		UINT instanceOffset) const
	{
		UpdateConstant(world, color);
		constantBuffer->SetOnCommandList(cmdList, 1);
		cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		ArMesh::Render(cmdList);
	}
}
