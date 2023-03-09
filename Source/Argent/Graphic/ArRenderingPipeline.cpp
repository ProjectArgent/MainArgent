#include "ArRenderingPipeline.h"

namespace Argent::Graphics::RenderingPipeline
{
	ArBaseRenderingPipeline::ArBaseRenderingPipeline(const char* vsFilePath, const char* psFilePath,
		const D3D12_ROOT_SIGNATURE_DESC* rootSigDesc, D3D12_GRAPHICS_PIPELINE_STATE_DESC* pipelineStateDesc):
		vertexShader(std::make_shared<Shader::ArShader>(vsFilePath))
	,	pixelShader(std::make_shared<Shader::ArShader>(psFilePath))
	{
		ID3D12Device* device = Graphics::ArGraphics::Instance()->GetDevice();
		CreateRootSignature(device, rootSigDesc);
		CreatePipelineState(device, pipelineStateDesc);
	}
}
