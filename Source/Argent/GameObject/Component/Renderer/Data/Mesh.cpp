#include "Mesh.h"
#include "../../../../Graphic/ArGraphics.h"
void Mesh::Render(const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& color)
{
	ID3D12GraphicsCommandList* cmdList = Argent::Graphics::ArGraphics::Instance()->GetCommandList();

	
	data->Render(cmdList, world, color);
}

#ifdef _DEBUG
void Mesh::DrawDebug()
{
}
#endif