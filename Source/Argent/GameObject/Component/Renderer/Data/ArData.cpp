#include "ArData.h"

namespace Argent::Data
{
	void ArData::Render(ID3D12GraphicsCommandList* cmdList, UINT instanceCount, UINT indexOffset, INT vertexOffset, UINT instanceOffset) const
	{
		cmdList->IASetVertexBuffers(0, 1, &vertexView);
		cmdList->IASetIndexBuffer(&indexView);
		cmdList->DrawIndexedInstanced(indexCountPerInstance, instanceCount, indexOffset, vertexOffset, instanceOffset);
	}
}
