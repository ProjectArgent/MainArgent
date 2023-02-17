#include "MeshData.h"
#include "../../../../Graphic/ArGraphics.h"



MeshData::MeshData(const std::vector<Argent::Data::ArMeshData::Vertex>& vertices, const std::vector<uint32_t>& indices)
{
	data = std::make_unique<Argent::Data::ArMeshData>(vertices, indices);
}

void MeshData::DrawDebug()
{
#ifdef _DEBUG


#endif
}

void MeshData::Render(const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& color)
{
	ID3D12GraphicsCommandList* cmdList = Argent::Graphics::ArGraphics::Instance()->GetCommandList();

	Argent::Graphics::ArGraphics::Instance()->SetSceneConstant();
	data->UpdateConstant(world, color);


	data->Render(cmdList);
}