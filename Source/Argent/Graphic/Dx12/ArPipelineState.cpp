#include "ArPipelineState.h"

#include "../../Other/Misc.h"

namespace Argent::Dx12
{
	HRESULT ArRenderingPipeline::CreateRootSignature(ID3D12Device* device)
	{
		HRESULT hr{ S_OK };

		Microsoft::WRL::ComPtr<ID3DBlob> rootSigBlob;
		Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
		hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, rootSigBlob.ReleaseAndGetAddressOf(), errorBlob.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

		hr = device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(rootSignature.ReleaseAndGetAddressOf()));
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

		return hr;
	}

	HRESULT ArRenderingPipeline::CreatePipelineState(ID3D12Device* device)
	{
		HRESULT hr{ S_OK };
		hr = device->CreateGraphicsPipelineState(&pipelineStateDesc, IID_PPV_ARGS(pipelineState.ReleaseAndGetAddressOf()));
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		return hr;
	}

	void ArRenderingPipeline::SetOnCommand(ID3D12GraphicsCommandList* cmdList) const
	{
		cmdList->SetPipelineState(pipelineState.Get());
		cmdList->SetGraphicsRootSignature(rootSignature.Get());
	}

	void ArRenderingPipeline::SetUpPipelineDesc()
	{
		//todo マルチレンダーターゲットに対応できるようにすること
		pipelineStateDesc.pRootSignature = rootSignature.Get();
		pipelineStateDesc.VS.pShaderBytecode = vertexShader->GetData();
		pipelineStateDesc.VS.BytecodeLength = vertexShader->GetSize();
		pipelineStateDesc.PS.pShaderBytecode = pixelShader->GetData();
		pipelineStateDesc.PS.BytecodeLength = pixelShader->GetSize();
		pipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
		pipelineStateDesc.RasterizerState = SetUpRasterizerDesc();
		pipelineStateDesc.BlendState = SetUpBlendState();
		pipelineStateDesc.InputLayout.NumElements = static_cast<UINT>(inputElementDesc.size());
		pipelineStateDesc.InputLayout.pInputElementDescs = inputElementDesc.data();
		pipelineStateDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
		pipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		pipelineStateDesc.NumRenderTargets = 1;
		pipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		pipelineStateDesc.SampleDesc = SetUpSamplerDesc();
		pipelineStateDesc.DepthStencilState = SetUpDepthStencilState();
		pipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	}

	namespace Sprite
	{
		void ArDefaultGraphicsPipeline::SetUpRootSigDesc()
		{
			range.emplace_back(Helper::Dx12::DescriptorRange::Generate(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV));
			rootParam.emplace_back(Helper::Dx12::RootParameter::Generate(1, &range.at(0), D3D12_SHADER_VISIBILITY_PIXEL));

			samplerDesc.emplace_back( Helper::Dx12::Sampler::GenerateSamplerDesc(Helper::Dx12::Sampler::FilterMode::fAnisotropic, Helper::Dx12::Sampler::WrapMode::wBorder));

			rootSigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
			rootSigDesc.NumParameters = 1;
			rootSigDesc.pParameters = &rootParam.at(0);
			rootSigDesc.NumStaticSamplers = 1;
			rootSigDesc.pStaticSamplers = samplerDesc.data();
		}
	}



	namespace Mesh
	{
		void ArDefaultGraphicsPipeline::SetUpRootSigDesc()
		{
			range.emplace_back(Helper::Dx12::DescriptorRange::Generate(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_CBV));
			range.emplace_back(Helper::Dx12::DescriptorRange::Generate(1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_CBV));
			range.emplace_back(Helper::Dx12::DescriptorRange::Generate(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV));

			rootParam.emplace_back(Helper::Dx12::RootParameter::Generate(1, &range.at(0), D3D12_SHADER_VISIBILITY_ALL));
			rootParam.emplace_back(Helper::Dx12::RootParameter::Generate(1, &range.at(1), D3D12_SHADER_VISIBILITY_ALL));
			rootParam.emplace_back(Helper::Dx12::RootParameter::Generate(1, &range.at(2), D3D12_SHADER_VISIBILITY_ALL));


			samplerDesc.emplace_back(Helper::Dx12::Sampler::GenerateSamplerDesc(Helper::Dx12::Sampler::FilterMode::fAnisotropic, Helper::Dx12::Sampler::WrapMode::wRepeat));

			rootSigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
			rootSigDesc.NumParameters = static_cast<UINT>(rootParam.size());
			rootSigDesc.pParameters = rootParam.data();
			rootSigDesc.NumStaticSamplers = 1;
			rootSigDesc.pStaticSamplers = samplerDesc.data();
		}

		void ArDefaultGraphicsPipeline::SetUpInputElement()
		{
			inputElementDesc.emplace_back(Helper::Dx12::InputElement::GenerateInputLayoutDesc("POSITION", DXGI_FORMAT_R32G32B32_FLOAT));
			inputElementDesc.emplace_back(Helper::Dx12::InputElement::GenerateInputLayoutDesc("NORMAL", DXGI_FORMAT_R32G32B32A32_FLOAT));
			inputElementDesc.emplace_back(Helper::Dx12::InputElement::GenerateInputLayoutDesc("TEXCOORD", DXGI_FORMAT_R32G32_FLOAT));
		}
	}

	namespace SkinnedMesh
	{
		void ArDefaultGraphicsPipeline::SetUpRootSigDesc()
		{
			range.emplace_back(Helper::Dx12::DescriptorRange::Generate(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_CBV));
			range.emplace_back(Helper::Dx12::DescriptorRange::Generate(1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_CBV));
			range.emplace_back(Helper::Dx12::DescriptorRange::Generate(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV));
			range.emplace_back(Helper::Dx12::DescriptorRange::Generate(2, 1, D3D12_DESCRIPTOR_RANGE_TYPE_CBV));
			range.emplace_back(Helper::Dx12::DescriptorRange::Generate(3, 1, D3D12_DESCRIPTOR_RANGE_TYPE_CBV));
			range.emplace_back(Helper::Dx12::DescriptorRange::Generate(1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV));


			rootParam.emplace_back(Helper::Dx12::RootParameter::Generate(1, &range.at(0), D3D12_SHADER_VISIBILITY_ALL));
			rootParam.emplace_back(Helper::Dx12::RootParameter::Generate(1, &range.at(1), D3D12_SHADER_VISIBILITY_ALL));
			rootParam.emplace_back(Helper::Dx12::RootParameter::Generate(1, &range.at(2), D3D12_SHADER_VISIBILITY_ALL));
			rootParam.emplace_back(Helper::Dx12::RootParameter::Generate(1, &range.at(3), D3D12_SHADER_VISIBILITY_ALL));
			rootParam.emplace_back(Helper::Dx12::RootParameter::Generate(1, &range.at(4), D3D12_SHADER_VISIBILITY_ALL));
			rootParam.emplace_back(Helper::Dx12::RootParameter::Generate(1, &range.at(5), D3D12_SHADER_VISIBILITY_ALL));

			samplerDesc.emplace_back(Helper::Dx12::Sampler::GenerateSamplerDesc(Helper::Dx12::Sampler::FilterMode::fPoint, Helper::Dx12::Sampler::WrapMode::wRepeat));
			//samplerDesc.emplace_back(Helper::Dx12::Sampler::GenerateSamplerDesc(Helper::Dx12::Sampler::FilterMode::fAnisotropic, Helper::Dx12::Sampler::WrapMode::wRepeat));

			rootSigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
			rootSigDesc.NumParameters = static_cast<UINT>(rootParam.size());
			rootSigDesc.pParameters = rootParam.data();
			rootSigDesc.NumStaticSamplers = samplerDesc.size();
			rootSigDesc.pStaticSamplers = samplerDesc.data();
		}

		void ArDefaultGraphicsPipeline::SetUpInputElement()
		{
			inputElementDesc.emplace_back(Helper::Dx12::InputElement::GenerateInputLayoutDesc("POSITION", DXGI_FORMAT_R32G32B32_FLOAT));
			inputElementDesc.emplace_back(Helper::Dx12::InputElement::GenerateInputLayoutDesc("NORMAL", DXGI_FORMAT_R32G32B32_FLOAT));
			inputElementDesc.emplace_back(Helper::Dx12::InputElement::GenerateInputLayoutDesc("TEXCOORD", DXGI_FORMAT_R32G32_FLOAT));
			inputElementDesc.emplace_back(Helper::Dx12::InputElement::GenerateInputLayoutDesc("WEIGHTS", DXGI_FORMAT_R32G32B32A32_FLOAT));
			inputElementDesc.emplace_back(Helper::Dx12::InputElement::GenerateInputLayoutDesc("BONES", DXGI_FORMAT_R32G32B32A32_UINT));
		}

	}
}
