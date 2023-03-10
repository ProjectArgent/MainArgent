#pragma once
#include <d3d12.h>
#include <wrl.h>
#include "../Graphic/Dx12/ArVertexBuffer.h"
#include "../Graphic/Dx12/ArIndexBuffer.h"


namespace Argent::Mesh
{
	/**
	 * \brief	���_�o�b�t�@�������Ă�N���X
				�p�����Ďg��
				�p����ł�Vertex�^���`�����_�z������
				���b�V���f�[�^�^�Ƃ��X�v���C�g�f�[�^�^�Ƃ���������������
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

		//hack �}���`�X���b�h�̎���unique_ptr�ł����v���H
		std::unique_ptr<Argent::Dx12::ArVertexBuffer<T>> vertexBuffer;
		std::unique_ptr<Argent::Dx12::ArIndexBuffer> indexBuffer;

		UINT indexCountPerInstance{};
	};
}

