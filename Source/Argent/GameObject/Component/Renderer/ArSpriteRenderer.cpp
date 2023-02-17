#include "ArSpriteRenderer.h"
#include "../../../Graphic/ArGraphics.h"
#include "../Core/Transform.h"
#include "../../GameObject.h"

namespace Argent::Component::Renderer
{
	ArSpriteRenderer::ArSpriteRenderer():
		ArRenderer("Sprite Renderer")
	{
		data = std::make_unique<Data::Sprite::ArSpriteData>();
		material = std::make_unique<Material::ArMaterial>(L"Resource/Sample256.png");
		Dx12::ArRenderingPipeline::CreateGraphicsPipeline<Dx12::Sprite::ArDefaultGraphicsPipeline>(&renderingPipeline, "Resource/Shader/SpriteVertex.cso", "Resource/Shader/SpritePixel.cso");
	}

	ArSpriteRenderer::~ArSpriteRenderer()
	{

	}

	void ArSpriteRenderer::Initialize()
	{
		ArRenderer::Initialize();
	}

	void ArSpriteRenderer::Finalize()
	{
		ArRenderer::Finalize();
	}

	void ArSpriteRenderer::Begin()
	{
		ArRenderer::Begin();
	}

	void ArSpriteRenderer::End()
	{
		ArRenderer::End();
	}

	void ArSpriteRenderer::Update()
	{
		ArRenderer::Update();
		const Transform* transform = GetOwner()->GetTransform();

		//todo Center‚Ì’l‚ð‚Ç‚Á‚©‚Å’è‹`‚·‚é‚±‚Æ
		data->UpdateVertexMap(transform->GetPosition(), transform->GetScale(), DirectX::XMFLOAT2(), transform->GetRotation().z, material->texture->GetWidth(), material->texture->GetHeight(),
			material->color.color);
	}

	void ArSpriteRenderer::Render(ID3D12GraphicsCommandList* cmdList) const
	{
		ArRenderer::Render(cmdList);
		material->Render(cmdList, 0);
		data->Render(cmdList, 1, 0, 0, 0);

	}

#ifdef _DEBUG
	void ArSpriteRenderer::DrawDebug()
	{
		if(ImGui::TreeNode(GetName().c_str()))
		{
			ArRenderer::DrawDebug();
			material->DrawDebug();
			ImGui::TreePop();
		}
	}
#endif
}
