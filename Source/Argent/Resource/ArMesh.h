#pragma once
#include <d3d12.h>
#include "../Graphic/Dx12/ArVertexBuffer.h"
#include "../Graphic/Dx12/ArIndexBuffer.h"
#include "ArMaterial.h"


namespace Argent::Mesh
{
	/**
	 * \brief 頂点バッファとインデックスバッファを持つ
	 *
	 * \tparam T Vertex構造体を指定
	 */
	template<class T>
	class ArMesh
	{
	public:
		ArMesh() = default;
		virtual ~ArMesh() = default;

		void SetIndexCountPerInstance(UINT i) { indexCountPerInstance = i; }
		virtual void Render(ID3D12GraphicsCommandList* cmdList, UINT vertexStartSlot = 0, UINT numVertexViews = 1) const
		{
			vertexBuffer->SetOnCommandList(cmdList, vertexStartSlot, numVertexViews);
			indexBuffer->SetOnCommandList(cmdList);
			cmdList->DrawIndexedInstanced(indexCountPerInstance, 1, 0, 0, 0);
		}

	protected:

		//hack マルチスレッドの時にunique_ptrでも大丈夫か？
		std::unique_ptr<Argent::Dx12::ArVertexBuffer<T>> vertexBuffer;
		std::unique_ptr<Argent::Dx12::ArIndexBuffer> indexBuffer;

		UINT indexCountPerInstance{};
	};
}

