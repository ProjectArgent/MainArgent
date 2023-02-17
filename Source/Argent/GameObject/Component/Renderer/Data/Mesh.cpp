#include "Mesh.h"

void Mesh::Render(const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& color)
{
	meshData->Render(world, color);
}

void Mesh::DrawDebug()
{
}
