#pragma once
#include <d3d12.h>
#include <wrl.h>
#include "../Graphic/Dx12/ArVertexBuffer.h"
#include "../Graphic/Dx12/ArIndexBuffer.h"


namespace Argent::Mesh
{
	/**
	 * \brief	頂点バッファを持ってるクラス
				継承して使う
				継承先ではVertex型を定義し頂点配列を作る
				メッシュデータ型とかスプライトデータ型とかそういった感じ
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

