#include "ArRenderingPipeline.h"

namespace Argent::Graphics::RenderingPipeline
{
	ArRenderingPipeline::ArRenderingPipeline(const char* vsFilePath, const char* psFilePath,
		const D3D12_ROOT_SIGNATURE_DESC* rootSigDesc, D3D12_GRAPHICS_PIPELINE_STATE_DESC* pipelineStateDesc)
	{
		vertexShader = std::make_shared<Shader::ArShader>(vsFilePath);
		pixelShader = std::make_shared<Shader::ArShader>(psFilePath);
		CreateRootSignature(rootSigDesc);
		CreatePipelineState(pipelineStateDesc);
	}
}
