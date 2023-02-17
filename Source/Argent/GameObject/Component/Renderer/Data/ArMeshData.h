#pragma once
#include "ArData.h"
#include <vector>
#include <DirectXMath.h>
#include "../../../../Graphic/Dx12/ArDescriptor.h"

namespace Argent::Data
{
	class ArMeshData:
		public ArData
	{
	public:
		struct Vertex
		{
			DirectX::XMFLOAT3 position;
			DirectX::XMFLOAT3 normal;
			DirectX::XMFLOAT2 texcoord;
		};
		struct Constant
		{
			DirectX::XMFLOAT4X4 world;
			DirectX::XMFLOAT4 materialColor;
		};

		struct Subset
		{
			uint32_t startVertex{};
			uint32_t vertexCount{};
			uint32_t startIndex{};
			uint32_t indexCount{};
		};
	public:
		
		ArMeshData(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);

		void UpdateConstant(const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& color) const
		{
			Constant tmp{};
			tmp.world = world;
			tmp.materialColor = color;
			UpdateConstant(tmp);
		}
		void UpdateConstant(const Constant& constant) const
		{
			constantMap->world = constant.world;
			constantMap->materialColor = constant.materialColor;
		}

		void Render(ID3D12GraphicsCommandList* cmdList, UINT instanceCount = 1, UINT indexOffset = 0, INT vertexOffset = 0, UINT instanceOffset = 0) const override;

	private:
		Constant* constantMap{};
		Microsoft::WRL::ComPtr<ID3D12Resource> constantBuffer;
		Descriptor::ArDescriptor* descriptor;
	};
}
