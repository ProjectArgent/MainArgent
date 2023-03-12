#pragma once

#include <d3d12.h>
#include <DirectXMath.h>
#include <dxgi1_5.h>
#include <wrl.h>
#include <vector>
#include <memory>
#include "Dx12/ArDescriptor.h"
#include "Dx12/ArCommand.h"


namespace Argent::Frame
{
	struct SceneConstant
	{
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 projection;
		DirectX::XMFLOAT4 lightColor;
		DirectX::XMFLOAT3 lightPosition;
		DirectX::XMFLOAT3 cameraPosition;
	};

	class FrameResource
	{
	public:
		FrameResource(ID3D12Device* device, IDXGISwapChain4* swapChain, UINT backBufferIndex, 
		              Descriptor::ArDescriptor* rtv, Descriptor::ArDescriptor* dsv, Descriptor::ArDescriptor* cbv, 
		              UINT NumCmdLists);
		~FrameResource() = default;
		FrameResource operator=(const FrameResource&) = delete;
		FrameResource operator=(const FrameResource&&) = delete;
		FrameResource(const FrameResource&) = delete;
		FrameResource(const FrameResource&&) = delete;

		[[nodiscard]] D3D12_RESOURCE_DESC GetBackBufferDesc() const { return backBuffer->GetDesc(); }

		[[nodiscard]] ID3D12GraphicsCommandList* GetCmdList() const { return cmdBundle.at(0)->cmdList.Get(); }

		DirectX::XMFLOAT4X4 GetSceneView() const { return cbScene->view;  }
		DirectX::XMFLOAT4X4 GetSceneProjection() const { return cbScene->projection;  }

		void Begin() const;
		void UpdateSceneConstant(const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection, const DirectX::XMFLOAT4& lightColor,
		                         const DirectX::XMFLOAT3& lightPosition, const DirectX::XMFLOAT3& cameraPosition) const;
		void SetSceneConstant(UINT rootParameterIndex = 0);
		void SetBarrier(D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after) const;
	public:
		std::vector<std::unique_ptr<Dx12::ArCommandBundle>> cmdBundle;
		Microsoft::WRL::ComPtr<ID3D12Resource> backBuffer;
		Microsoft::WRL::ComPtr<ID3D12Resource> constantBuffer;
		Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource;
		Descriptor::ArDescriptor* rtv;
		Descriptor::ArDescriptor* dsv;
		Descriptor::ArDescriptor* cbv;
		UINT backBufferIndex;
		SceneConstant*	cbScene;
	public:
	};
}
			