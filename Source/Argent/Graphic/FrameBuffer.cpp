#include "FrameBuffer.h"

#include <cassert>
#include "d3dx12.h"
#include "ArGraphics.h"

FrameBuffer::FrameBuffer(ID3D12Device* device, D3D12_RESOURCE_DESC rDesc,
                         float clearColor[4],
                         D3D12_DESCRIPTOR_HEAP_DESC heapDesc)
{
	HRESULT hr{ S_OK };

	D3D12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	D3D12_CLEAR_VALUE clearValue = CD3DX12_CLEAR_VALUE(DXGI_FORMAT_R8G8B8A8_UNORM, clearColor);

	hr = device->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &rDesc, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, 
		&clearValue, IID_PPV_ARGS(resource.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(hr));

	heapDesc.NumDescriptors = 1;
	hr = device->CreateDescriptorHeap(&heapDesc,IID_PPV_ARGS(rtvHeap.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(hr));

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	auto handle = rtvHeap->GetCPUDescriptorHandleForHeapStart();

	device->CreateRenderTargetView(resource.Get(), &rtvDesc, handle);


	heapDesc.NumDescriptors = 1;
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	hr = device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(srvHeap.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(hr));



	handle = srvHeap->GetCPUDescriptorHandleForHeapStart();

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Format = rtvDesc.Format;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

 
	device->CreateShaderResourceView(resource.Get(), &srvDesc, handle);


	Vertex vertices[4]
	{
		{{ -1.f,  1.f, 0.0f },{ 0.0f, 0.0f }},
		{{  1.f,  1.f, 0.0f },{ 1.0f, 0.0f }},
		{{ -1.f, -1.f, 0.0f },{ 0.0f, 1.0f }},
		{{  1.f, -1.f, 0.0f },{ 1.0f, 1.0f }},
	};

	heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	rDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(vertices));



	hr = device->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &rDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(vertexBuffer.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(hr));


	vertexView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
	vertexView.SizeInBytes = sizeof(vertices);
	vertexView.StrideInBytes = sizeof(Vertex);

	Vertex* map{};
	vertexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&map));
	std::copy(std::begin(vertices), std::end(vertices), map);
	vertexBuffer->Unmap(0, nullptr);




	D3D12_INPUT_ELEMENT_DESC inElementDesc[]
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	};

	vertexShader = std::make_unique<Argent::Shader::ArShader>("./Resources/Shader/PeraVertex.cso");
	pixelShader = std::make_unique<Argent::Shader::ArShader>("./Resources/Shader/PeraPixel.cso");


	D3D12_RENDER_TARGET_BLEND_DESC rtvBlendDesc{};
	rtvBlendDesc.BlendEnable = true;
	rtvBlendDesc.BlendEnable = TRUE;
	rtvBlendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	rtvBlendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	rtvBlendDesc.BlendOp = D3D12_BLEND_OP_ADD;
	rtvBlendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	rtvBlendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;
	rtvBlendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;

	rtvBlendDesc.LogicOpEnable = FALSE;
	rtvBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;


	D3D12_DESCRIPTOR_RANGE range[1]{};
	range[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	range[0].BaseShaderRegister = 0;
	range[0].NumDescriptors = 1;


	D3D12_ROOT_PARAMETER rootParam[1]{};
	rootParam[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParam[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParam[0].DescriptorTable.pDescriptorRanges = &range[0];
	rootParam[0].DescriptorTable.NumDescriptorRanges = 1;

	
	D3D12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);


	D3D12_ROOT_SIGNATURE_DESC rsDesc{};
	rsDesc.NumParameters = 1;
	rsDesc.pParameters = rootParam;
	rsDesc.NumStaticSamplers = 1;
	rsDesc.pStaticSamplers = &samplerDesc;
	rsDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	

	Microsoft::WRL::ComPtr<ID3DBlob> rsBlob;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
	hr= D3D12SerializeRootSignature(&rsDesc, D3D_ROOT_SIGNATURE_VERSION_1, 
		rsBlob.ReleaseAndGetAddressOf(), errorBlob.ReleaseAndGetAddressOf());
	assert(SUCCEEDED(hr));
	hr = device->CreateRootSignature(0, rsBlob->GetBufferPointer(), rsBlob->GetBufferSize(),
		IID_PPV_ARGS(rootSignature.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(hr));


	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};
	pipelineDesc.pRootSignature = rootSignature.Get();
	pipelineDesc.InputLayout.NumElements = _countof(inElementDesc);
	pipelineDesc.InputLayout.pInputElementDescs = inElementDesc;
	pipelineDesc.VS.BytecodeLength = vertexShader->GetSize();
	pipelineDesc.VS.pShaderBytecode = vertexShader->GetData();
	pipelineDesc.PS.BytecodeLength = pixelShader->GetSize();
	pipelineDesc.PS.pShaderBytecode = pixelShader->GetData();
	pipelineDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	pipelineDesc.BlendState.AlphaToCoverageEnable = FALSE;
	pipelineDesc.BlendState.IndependentBlendEnable = FALSE;
	pipelineDesc.BlendState.RenderTarget[0] = rtvBlendDesc;
	pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pipelineDesc.NumRenderTargets = 1;
	pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	pipelineDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	pipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	pipelineDesc.SampleDesc.Count = 1;
	pipelineDesc.SampleDesc.Quality = 0;
	pipelineDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	hr = device->CreateGraphicsPipelineState(
		&pipelineDesc, IID_PPV_ARGS(pipeline.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(hr));
}

void FrameBuffer::Begin(Argent::Graphics::ArGraphics* gfx)
{
	ID3D12GraphicsCommandList* cmdList = gfx->GetCommandList();
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		resource.Get(), 
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, 
		D3D12_RESOURCE_STATE_RENDER_TARGET);
	cmdList->ResourceBarrier(1, &barrier);

	auto rtvHeapPointer = rtvHeap->GetCPUDescriptorHandleForHeapStart();

	//auto dsvHandle = gfx->GetDepthHandle();
	D3D12_CPU_DESCRIPTOR_HANDLE* dsvHandle = nullptr;
	//todo

	cmdList->OMSetRenderTargets(1, &rtvHeapPointer, 
		false, dsvHandle);
	cmdList->ClearRenderTargetView(rtvHeapPointer, gfx->GetClearColor(), 0, nullptr);


	D3D12_VIEWPORT viewport =gfx->GetViewport();
	D3D12_RECT rect = gfx->GetRect();
	cmdList->RSSetViewports(1, &viewport);
	cmdList->RSSetScissorRects(1, &rect);
}

void FrameBuffer::End(Argent::Graphics::ArGraphics* gfx)
{
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		resource.Get(), 
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	gfx->GetCommandList()->ResourceBarrier(1, &barrier);
}

void FrameBuffer::Draw(Argent::Graphics::ArGraphics* gfx)
{
	ID3D12GraphicsCommandList* cmdList = gfx->GetCommandList();
	cmdList->SetPipelineState(pipeline.Get());
	cmdList->SetGraphicsRootSignature(rootSignature.Get());


	cmdList->SetDescriptorHeaps(1, srvHeap.GetAddressOf());
	auto handle = srvHeap->GetGPUDescriptorHandleForHeapStart();
	cmdList->SetGraphicsRootDescriptorTable(0, handle);

	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	cmdList->IASetVertexBuffers(0, 1, &vertexView);
	cmdList->DrawInstanced(4, 1, 0, 0);
}
