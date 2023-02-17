#include "ArMeshRenderer.h"
#include <vector>
#include "../../../Graphic/ArGraphics.h"
#include "../../GameObject.h"
#include "../Core/Transform.h"
#include "Data/ArMeshData.h"

namespace Argent::Component::Renderer
{
	ArMeshRenderer::ArMeshRenderer() :
		ArRenderer("Mesh Renderer")
	{
		std::vector<Data::ArMeshData::Vertex> vertices
		{
			/*0	*/	{{ 1, 1, 1}, {-1,  0,  0} },//0 x
			/*1	*/	{{ 1, 1, 1}, { 0,  1,  0} },//0 y
			/*2	*/	{{ 1, 1, 1}, { 0,  0, -1} },//0 z
			/*3	*/	{{ 1, 1, 1}, {-1,  0,  0} },//1 x
			/*4	*/	{{ 1, 1, 1}, { 0,  1,  0} },//1 y
			/*5	*/	{{ 1, 1, 1}, { 0,  0,  1} },//1 z
			/*6	*/	{{ 1, 1, 1}, { 1,  0,  0} },//2 x
			/*7	*/	{{ 1, 1, 1}, { 0,  1,  0} },//2 y
			/*8	*/	{{ 1, 1, 1}, { 0,  0,  1} },//2 z
			/*9	*/	{{ 1, 1, 1}, { 1,  0,  0} },//3 x
			/*10*/	{{ 1, 1, 1}, { 0,  1,  0} },//3 y
			/*11*/	{{ 1, 1, 1}, { 0,  0, -1} },//3 z
			/*12*/	{{ 1, 1, 1}, {-1,  0,  0} },//4 x
			/*13*/	{{ 1, 1, 1}, { 0, -1,  0} },//4 y
			/*14*/	{{ 1, 1, 1}, { 0,  0, -1} },//4 z
			/*15*/	{{ 1, 1, 1}, {-1,  0,  0} },//5 x
			/*16*/	{{ 1, 1, 1}, { 0, -1,  0} },//5 y
			/*17*/	{{ 1, 1, 1}, { 0,  0,  1} },//5 z
			/*18*/	{{ 1, 1, 1}, { 1,  0,  0} },//6 x
			/*19*/	{{ 1, 1, 1}, { 0, -1,  0} },//6 y
			/*20*/	{{ 1, 1, 1}, { 0,  0,  1} },//6 z
			/*21*/	{{ 1, 1, 1}, { 1,  0,  0} },//7 x
			/*22*/	{{ 1, 1, 1}, { 0, -1,  0} },//7 y
			/*23*/	{{ 1, 1, 1}, { 0,  0, -1} }//7 z
		};
		std::vector<uint32_t> indices
		{
			//up?
			1,4,7,10,1,7,
			//1,10,7,4,1,7,
			//front
			14, 2, 11,23,14,11,
			//right—¹
			21,9,6,18,21,6,
			//back—¹
			17,20,8,5,17,8,
			//left—¹
			12, 15, 3, 0, 12, 3,
			//bottom
			13,22,19,16,13,19
		};

		data = std::make_unique<Data::ArMeshData>(vertices, indices);
		material = std::make_unique<Material::ArMaterial>(L"");
		Dx12::ArRenderingPipeline::CreateGraphicsPipeline<Dx12::Mesh::ArDefaultGraphicsPipeline>(
			&graphicsPipeline, "Resource/Shader/GeometricPrimitiveVertex.cso",
			"Resource/Shader/GeometricPrimitivePixel.cso"
			);
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
		const Transform* t = GetOwner()->GetTransform();
		
		Data::ArMeshData::Constant constant{};
		constant.world = t->GetWorld();
		constant.materialColor = material->color.color;
		
		data->UpdateConstant(constant);
		ArRenderer::Update();
	}

	void ArMeshRenderer::Render()
	{
		ID3D12GraphicsCommandList* cmdList = Graphics::ArGraphics::Instance()->GetCommandList();
		graphicsPipeline->SetOnCommand(cmdList);
		//material->Render(cmdList, 0);
		Graphics::ArGraphics::Instance()->SetSceneConstant();
		data->Render(cmdList, 1, 0, 0, 0);
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
