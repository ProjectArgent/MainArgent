#include "ArMeshRenderer.h"
#include "../Graphic/ArGraphics.h"
#include "../GameObject/GameObject.h"
#include "Transform.h"
#include "../Resource/ArStaticMesh.h"
#include "../Other/ArHelper.h"
#include "../Resource/ArMaterial.h"
#include "../Resource/ArStaticMesh.h"

namespace Argent::Component::Renderer
{
	ArMeshRenderer::ArMeshRenderer(Argent::Mesh::StaticMesh::ArStaticMesh* data):
		ArRenderer("Mesh Renderer")
	,	mesh(data)
	{
		ID3D12Device* device = Argent::Graphics::ArGraphics::Instance()->GetDevice();
		HRESULT hr{ S_OK };

		D3D12_ROOT_SIGNATURE_DESC rootSigDesc{};
		D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc{};
		
		D3D12_DESCRIPTOR_RANGE range[3]{};

		range[0] = Helper::Dx12::DescriptorRange::Generate(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_CBV);
		range[1] = Helper::Dx12::DescriptorRange::Generate(1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_CBV);
		range[2] = Helper::Dx12::DescriptorRange::Generate(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);
																															 
		D3D12_ROOT_PARAMETER rootParam[3]{};
		rootParam[0] = Helper::Dx12::RootParameter::Generate(1, &range[0], D3D12_SHADER_VISIBILITY_ALL);
		rootParam[1] = Helper::Dx12::RootParameter::Generate(1, &range[1], D3D12_SHADER_VISIBILITY_ALL);
		rootParam[2] = Helper::Dx12::RootParameter::Generate(1, &range[2], D3D12_SHADER_VISIBILITY_ALL);
																														  
		D3D12_STATIC_SAMPLER_DESC samplerDesc = Helper::Dx12::Sampler::GenerateSamplerDesc(Helper::Dx12::Sampler::FilterMode::fPoint, Helper::Dx12::Sampler::WrapMode::wRepeat);


		rootSigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
		rootSigDesc.NumParameters = 3;
		rootSigDesc.pParameters = rootParam;
		rootSigDesc.NumStaticSamplers = 1;
		rootSigDesc.pStaticSamplers = &samplerDesc;


		//パイプライン

		D3D12_BLEND_DESC blendDesc{};
		blendDesc.AlphaToCoverageEnable = FALSE;
		blendDesc.IndependentBlendEnable = FALSE;
		blendDesc.RenderTarget[0] = Helper::Dx12::Blend::GenerateRenderTargetBlendDesc(Helper::Dx12::Blend::BlendMode::bAlpha);

		D3D12_INPUT_ELEMENT_DESC inputElementDesc[]
		{
			Helper::Dx12::InputElement::GenerateInputLayoutDesc("POSITION", DXGI_FORMAT_R32G32B32_FLOAT),
			Helper::Dx12::InputElement::GenerateInputLayoutDesc("NORMAL", DXGI_FORMAT_R32G32B32_FLOAT),
			Helper::Dx12::InputElement::GenerateInputLayoutDesc("TEXCOORD", DXGI_FORMAT_R32G32_FLOAT),
		};

		DXGI_SAMPLE_DESC sampleDesc{};
		sampleDesc.Count = 1;
		sampleDesc.Quality = 0;


		D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
		depthStencilDesc.DepthEnable = TRUE;
		depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;


		pipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

		pipelineStateDesc.RasterizerState = Helper::Dx12::Rasterizer::Generate();
		pipelineStateDesc.BlendState = blendDesc;
		pipelineStateDesc.InputLayout.NumElements = _countof(inputElementDesc);
		pipelineStateDesc.InputLayout.pInputElementDescs = inputElementDesc;
		pipelineStateDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
		pipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		pipelineStateDesc.NumRenderTargets = 1;
		pipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		pipelineStateDesc.SampleDesc = sampleDesc;
		pipelineStateDesc.DepthStencilState = depthStencilDesc;
		pipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		

		renderingPipeline = std::make_shared<Graphics::RenderingPipeline::ArBaseRenderingPipeline>(
			"./Resource/Shader/GeometricPrimitiveVertex.cso",
			"./Resource/Shader/GeometricPrimitivePixel.cso",
			&rootSigDesc,
			&pipelineStateDesc
			);
		materials.at(0) = std::make_unique<Argent::Material::ArMaterial>("");
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

		if(mesh)
		mesh->Render(cmdList,
			GetOwner()->GetTransform()->GetWorld(), materials.at(0)->color.color);
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
