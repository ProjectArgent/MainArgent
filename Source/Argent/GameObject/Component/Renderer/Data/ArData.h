#pragma once
#include <d3d12.h>
#include <wrl.h>
#include "d3dx12.h"

namespace Argent::Data
{
	/**
	 * \brief	頂点バッファを持ってるクラス
				継承して使う
				継承先ではVertex型を定義し頂点配列を作る
				メッシュデータ型とかスプライトデータ型とかそういった感じ
	 */
	class ArData
	{
	public:
		ArData() = default;

		void SetIndexCountPerInstance(UINT i) { indexCountPerInstance = i; }
		virtual void Render(ID3D12GraphicsCommandList* cmdList, UINT instanceCount, UINT indexOffset = 0, INT vertexOffset = 0, UINT instanceOffset = 0) const;

	protected:

		Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D12Resource> indexBuffer;
		D3D12_VERTEX_BUFFER_VIEW vertexView{};
		D3D12_INDEX_BUFFER_VIEW indexView{};
		UINT indexCountPerInstance{};
	};
}

