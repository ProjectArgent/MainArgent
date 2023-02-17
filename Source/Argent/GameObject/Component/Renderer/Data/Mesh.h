#pragma once
#include <DirectXMath.h>
#include "MeshData.h"


class Mesh
{
public:
	Mesh(MeshData* meshData):
		meshData(meshData)
	{}
	virtual ~Mesh() = default;

	void Render(const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& color);
	void DrawDebug();
protected:

	MeshData* meshData;
};