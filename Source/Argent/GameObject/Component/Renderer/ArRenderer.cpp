#include "ArRenderer.h"

//成瀬さんの置物

//using ID = size_t;
//
//inline ID GenerateID()
//{
//	static ID lastID{};
//	return lastID++;
//}
//
//template<typename>
//inline ID GetID()
//{
//	static ID id = GenerateID();
//	return id;
//}

/**
 * \brief 描画用の基底クラス
 */
namespace Argent::Component::Renderer
{
	ArRenderer::ArRenderer(const std::string& name):
		ArComponent(name)
		//,	pipelineState(nullptr)
		//,	rootSignature(nullptr)
	{}

	ArRenderer::~ArRenderer()
	{

	}


	void ArRenderer::Render(ID3D12GraphicsCommandList* cmdList) const
	{
		graphicsPipeline->SetOnCommand(cmdList);
	}

}
