#include "ArSpriteRenderer.h"
#include "../Graphic/ArGraphics.h"
#include "Transform.h"
#include "../GameObject/GameObject.h"
#include "../Resource/ArMaterial.h"

namespace Argent::Component::Renderer
{
	ArSpriteRenderer::ArSpriteRenderer():
		ArRenderer("Sprite Renderer")
	{
		data = std::make_unique<Mesh::Sprite::ArSprite>();
		materials.emplace_back(std::make_shared<Material::ArMaterial>("Resource/Sample256.png"));


		D3D12_ROOT_SIGNATURE_DESC rootSigDesc{};
		D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc{};
		
		
		D3D12_DESCRIPTOR_RANGE range[1]{};

		range[0] = Helper::Dx12::DescriptorRange::Generate(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);
																															 
		D3D12_ROOT_PARAMETER rootParam[1]{};
		rootParam[0] = Helper::Dx12::RootParameter::Generate(1, &range[0], D3D12_SHADER_VISIBILITY_PIXEL);
																														  
		D3D12_STATIC_SAMPLER_DESC samplerDesc = Helper::Dx12::Sampler::GenerateSamplerDesc(Helper::Dx12::Sampler::FilterMode::fAnisotropic, Helper::Dx12::Sampler::WrapMode::wBorder);


		rootSigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
		rootSigDesc.NumParameters = 1;
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
			Helper::Dx12::InputElement::GenerateInputLayoutDesc("TEXCOORD", DXGI_FORMAT_R32G32_FLOAT),
			Helper::Dx12::InputElement::GenerateInputLayoutDesc("COLOR", DXGI_FORMAT_R32G32B32A32_FLOAT)
		};

		DXGI_SAMPLE_DESC sampleDesc{};
		sampleDesc.Count = 1;
		sampleDesc.Quality = 0;


		D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
		depthStencilDesc.DepthEnable = TRUE;
		depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;

		D3D12_RASTERIZER_DESC rasterizerDesc{};
		rasterizerDesc.MultisampleEnable = FALSE;
		rasterizerDesc.DepthClipEnable = FALSE;
		rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
		rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

		pipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

		pipelineStateDesc.RasterizerState = rasterizerDesc;
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
			"Resource/Shader/SpriteVertex.cso", 
			"Resource/Shader/SpritePixel.cso",
			&rootSigDesc,
			&pipelineStateDesc);
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

		//todo Centerの値をどっかで定義すること
		data->UpdateVertexMap(transform->GetPosition(), transform->GetScale(), DirectX::XMFLOAT2(), transform->GetRotation().z, materials.at(0)->textures.at(0)->GetWidth(), materials.at(0)->textures.at(0)->GetHeight(),
			materials.at(0)->color.color);
	}

	void ArSpriteRenderer::Render(ID3D12GraphicsCommandList* cmdList) const
	{
		ArRenderer::Render(cmdList);
		materials.at(0)->Render(cmdList, 0);
		cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		data->Render(cmdList);
	}

#ifdef _DEBUG
	void ArSpriteRenderer::DrawDebug()
	{
		if(ImGui::TreeNode(GetName().c_str()))
		{
			ArRenderer::DrawDebug();
			materials.at(0)->DrawDebug();
			ImGui::TreePop();
		}
	}
#endif
}
