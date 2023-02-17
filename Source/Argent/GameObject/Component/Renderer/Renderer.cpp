#include "Renderer.h"
#include "../../../Graphic/ArGraphics.h"
#include "../../GameObject.h"

MeshRenderer::MeshRenderer(Mesh* mesh):
	Renderer("Demo Mesh Renderer")
,	mesh(mesh)
{
	ID3D12Device* device = Argent::Graphics::ArGraphics::Instance()->GetDevice();
	HRESULT hr{ S_OK };
	Argent::Dx12::ArRenderingPipeline::CreateGraphicsPipeline<Argent::Dx12::Mesh::ArDefaultGraphicsPipeline>(&renderingPipeline,
		"./Resource/Shader/GeometricPrimitiveVertex.cso",
		"./Resource/Shader/GeometricPrimitivePixel.cso"
		);
	material = std::make_unique<Argent::Material::ArMaterial>(L"");
}

void MeshRenderer::Render()
{
	ID3D12GraphicsCommandList* cmdList = Argent::Graphics::ArGraphics::Instance()->GetCommandList();
	renderingPipeline->SetOnCommand(cmdList);

	mesh->Render(GetOwner()->GetTransform()->GetWorld(), material->color.color);
	Renderer::Render();
}

#ifdef _DEBUG
void MeshRenderer::DrawDebug()
{
	mesh->DrawDebug();
	Renderer::DrawDebug();
}
#endif