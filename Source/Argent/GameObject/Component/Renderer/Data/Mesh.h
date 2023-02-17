#pragma once
#include <DirectXMath.h>
#include "ArMeshData.h"


class Mesh
{
public:
	Mesh(Argent::Data::ArMeshData* data):
		data(data)
		//meshData(meshData)
	{
		
	}
	//Mesh(const std::vector<Argent::Data::ArMeshData::Vertex>& vertices, const std::vector<uint32_t>& indices)
	//	//meshData(meshData)
	//{
	//	data = std::make_unique<Argent::Data::ArMeshData>(vertices, indices);
	//}

	virtual ~Mesh() = default;

	void Render(const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& color);

#ifdef _DEBUG
	void DrawDebug();
#endif
protected:

	Argent::Data::ArMeshData* data;
	//MeshData* meshData;
};