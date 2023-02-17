#pragma once
#include <d3d12.h>
#include <wrl.h>
#include "d3dx12.h"

namespace Argent::Data
{
	/**
	 * \brief	���_�o�b�t�@�������Ă�N���X
				�p�����Ďg��
				�p����ł�Vertex�^���`�����_�z������
				���b�V���f�[�^�^�Ƃ��X�v���C�g�f�[�^�^�Ƃ���������������
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

