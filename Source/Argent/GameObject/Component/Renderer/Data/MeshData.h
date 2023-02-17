#pragma once
#include <DirectXMath.h>
#include <memory>
#include <vector>
#include "ArMeshData.h"

class MeshData
{
public:
	MeshData(const std::vector<Argent::Data::ArMeshData::Vertex>& vertices, const std::vector<uint32_t>& indices);
	virtual ~MeshData() = default;

	virtual void DrawDebug();
	virtual void Render(const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& color);

	std::unique_ptr<Argent::Data::ArMeshData> data;
};