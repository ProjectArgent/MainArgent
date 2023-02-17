#include "ArMeshData.h"
#include "../../../../Graphic/ArGraphics.h"
#include "../../../../Other/ArHelper.h"


namespace Argent::Data
{
	ArMeshData::ArMeshData(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
	{
		ID3D12Device* device = Graphics::ArGraphics::Instance()->GetDevice();
		Vertex* vertexMap{};
		vertexView = Argent::Helper::Dx12::Buffer::CreateVertex<Vertex>(device, static_cast<UINT>(vertices.size()), vertices.begin(), 
			vertices.end(), &vertexMap, vertexBuffer.ReleaseAndGetAddressOf());
		indexView = Argent::Helper::Dx12::Buffer::CreateIndex<uint32_t>(device, static_cast<UINT>(indices.size()), indices.begin(),
			indices.end(), indexBuffer.ReleaseAndGetAddressOf());

		descriptor = Graphics::ArGraphics::Instance()->GetHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)->PopDescriptor();
		Helper::Dx12::Buffer::CreateConstant(device, &constantMap, descriptor->GetCPUHandle(), constantBuffer.ReleaseAndGetAddressOf());
		indexCountPerInstance = static_cast<UINT>(indices.size());
	}

	void ArMeshData::Render(ID3D12GraphicsCommandList* cmdList, UINT instanceCount, UINT indexOffset, INT vertexOffset,
		UINT instanceOffset) const
	{
		cmdList->SetDescriptorHeaps(1, descriptor->GetDescriptorHeap()->GetHeapDoublePointer());
		cmdList->SetGraphicsRootDescriptorTable(1, descriptor->GetGPUHandle());
		cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		ArData::Render(cmdList, instanceCount, indexOffset, vertexOffset, instanceOffset);
	}
}
