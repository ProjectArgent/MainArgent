#include "ArMeshRenderer.h"
#include <vector>
#include "../../../Graphic/ArGraphics.h"
#include "../../GameObject.h"
#include "../Core/Transform.h"
#include "Data/ArMeshData.h"

namespace Argent::Component::Renderer
{
	ArMeshRenderer::ArMeshRenderer(Argent::Data::ArMeshData* data):
		ArRenderer("Mesh Renderer")
	,	data(data)
	{
		ID3D12Device* device = Argent::Graphics::ArGraphics::Instance()->GetDevice();
		HRESULT hr{ S_OK };
		Argent::Dx12::ArRenderingPipeline::CreateGraphicsPipeline<Argent::Dx12::Mesh::ArDefaultGraphicsPipeline>(&renderingPipeline,
			"./Resource/Shader/GeometricPrimitiveVertex.cso",
			"./Resource/Shader/GeometricPrimitivePixel.cso"
			);
		material = std::make_unique<Argent::Material::ArMaterial>(L"");
	}

	void ArMeshRenderer::Initialize()
	{
		ArRenderer::Initialize();
	}

	void ArMeshRenderer::Finalize()
	{
		ArRenderer::Finalize();
	}

	void ArMeshRenderer::Begin()
	{
		ArRenderer::Begin();
	}

	void ArMeshRenderer::End()
	{
		ArRenderer::End();
	}

	void ArMeshRenderer::Update()
	{

	}

	void ArMeshRenderer::Render()
	{
		ArRenderer::Render();
		Argent::Graphics::ArGraphics::Instance()->SetSceneConstant();
		ID3D12GraphicsCommandList* cmdList = Argent::Graphics::ArGraphics::Instance()->GetCommandList();

		if(data)
		data->Render(cmdList,
			GetOwner()->GetTransform()->GetWorld(), material->color.color);
	}

#ifdef _DEBUG
	void ArMeshRenderer::DrawDebug()
	{
		if(ImGui::TreeNode(GetName().c_str()))
		{

			ImGui::TreePop();
		}
		ArRenderer::DrawDebug();
	}
#endif
}
