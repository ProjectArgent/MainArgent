#pragma once
#include <d3d12.h>
#include <memory>
#include <wrl.h>
#include <DirectXMath.h>
#include "ArShader.h"

namespace Argent::Graphics
{
	class ArGraphics;
}

class ArGraphics;

class FrameBuffer
{
	struct Vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 texcoord;
	};
public:
	FrameBuffer(ID3D12Device* device, D3D12_RESOURCE_DESC rDesc, float clearColor[4],
		D3D12_DESCRIPTOR_HEAP_DESC heapDesc);
	virtual ~FrameBuffer() = default;


	void Begin(Argent::Graphics::ArGraphics* gfx);
	void End(Argent::Graphics::ArGraphics* gfx);
	void Draw(Argent::Graphics::ArGraphics* gfx);
private:
	Microsoft::WRL::ComPtr<ID3D12Resource> resource;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> cbvHeap;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipeline;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer;
	std::unique_ptr<Argent::Shader::ArShader> vertexShader;
	std::unique_ptr<Argent::Shader::ArShader> pixelShader;
	D3D12_VERTEX_BUFFER_VIEW vertexView{};
};

