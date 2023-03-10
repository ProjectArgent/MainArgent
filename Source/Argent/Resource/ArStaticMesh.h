#pragma once
#include "ArMesh.h"
#include <vector>
#include <DirectXMath.h>
#include "../Graphic/Dx12/ArConstantBuffer.h"

namespace Argent::Mesh::StaticMesh
{
	struct Vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 texcoord;
	};

	class ArStaticMesh:
		public ArMesh<Vertex>
	{
	public:
		struct Constant
		{
			DirectX::XMFLOAT4X4 world;
			DirectX::XMFLOAT4 materialColor;
		};
public:
		
		ArStaticMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);

		void UpdateConstant(const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& color) const
		{
			Constant tmp{};
			tmp.world = world;
			tmp.materialColor = color;
			UpdateConstant(tmp);
		}
		void UpdateConstant(const Constant& constant) const
		{
			constantBuffer->UpdateConstantBuffer(constant);
			//constantMap->world = constant.world;
			//constantMap->materialColor = constant.materialColor;
		}

		void Render(ID3D12GraphicsCommandList* cmdList, const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& color,
			UINT instanceCount = 1, UINT indexOffset = 0, INT vertexOffset = 0, UINT instanceOffset = 0) const;

	private:
		std::unique_ptr<Argent::Dx12::ArConstantBuffer<Constant>> constantBuffer;
		//Constant* constantMap{};
		//Microsoft::WRL::ComPtr<ID3D12Resource> constantBuffer;
		//Descriptor::ArDescriptor* descriptor;
	};

}
