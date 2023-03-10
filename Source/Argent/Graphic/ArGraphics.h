#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <memory>
#include <string>


//todo カメラとかライトとかを引数にしないように
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "DirectXTex.lib")

//自作クラス
#include "FrameBuffer.h"
#include "ArFrameResource.h"
#include "Dx12/ArCommand.h"
#include "Dx12/ArDescriptorHeap.h"
#include "../Component/Camera.h"
#include "../Component/Light.h"


namespace Argent::Graphics
{
	class ArGraphics
	{
	public:
		enum class CoordinateSystem
		{
			cRightYup,
			cLeftYup,
			cRightZup,
			cLeftZup,
			cNone,
		};
	public:
		ArGraphics(HWND hWnd);
		virtual ~ArGraphics()
		{
			renderingQueue->SetFence(1);
		}
		ArGraphics(const ArGraphics&) = delete;
		ArGraphics(const ArGraphics&&) = delete;
		ArGraphics operator=(const ArGraphics&) = delete;
		ArGraphics operator=(const ArGraphics&&) = delete;
		
		void Initialize();
		void Terminate();
		void Begin();
		void End();
		
		HRESULT CreateWhiteTexture(ID3D12Resource** resource);
		HRESULT CreateBlackTexture(ID3D12Resource** resource);
		HRESULT CreateGrayGradationTexture(ID3D12Resource** resource);
		HRESULT CreateNoiseTexture(ID3D12Resource** resource);
		
		void SetSceneConstant(UINT rootParameterIndex = 0);
	public:
		static ArGraphics* Instance() { return instance; }
		[[nodiscard]] ID3D12Device* GetDevice() const { return mDevice.Get(); }
		[[nodiscard]] ID3D12GraphicsCommandList* GetCommandList() const { return curFrameResource->GetCmdList(); }
		
		[[nodiscard]] float GetWidth() const { return windowWidth; }
		[[nodiscard]] float GetHeight() const { return windowHeight; }
		
		[[nodiscard]] ID3D12DescriptorHeap* GetGUIHeap() const { return imGuiHeap->GetHeapPointer(); }

		
		void SetCamera(Camera* c) { nextCamera = c; }
		[[nodiscard]] Camera* GetCamera() const { return camera; }
		void SetLight(Light* l) { light = l; }
		[[nodiscard]] Light* GetLight() const { return light; }
		
		[[nodiscard]] D3D12_VIEWPORT GetViewport() const { return viewport; }
		[[nodiscard]] D3D12_RECT GetRect() const { return scissorRect; }

		float* GetClearColor() { return clearColor; }

		[[nodiscard]] Descriptor::ArDescriptorHeap* GetHeap(D3D12_DESCRIPTOR_HEAP_TYPE type) const
		{
			Descriptor::ArDescriptorHeap* heapPointer = nullptr;
			switch (type)
			{
			case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV:
				heapPointer = srvCbvHeap.get();
				break;
			case D3D12_DESCRIPTOR_HEAP_TYPE_RTV:
				heapPointer = rtvHeap.get();
				break;
			case D3D12_DESCRIPTOR_HEAP_TYPE_DSV:
				heapPointer = dsvHeap.get();
			case D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER:
				heapPointer = nullptr;
				break;
			default:
				heapPointer = nullptr;
			}
			return heapPointer;
		}

		[[nodiscard]] Descriptor::ArDescriptorHeap* GetImGuiHeap() const { return imGuiHeap.get(); }
		[[nodiscard]] Dx12::ArCommandBundle* GetResourceCmdBundle() const { return resourceCmdBundle.get(); }
		[[nodiscard]] Dx12::ArCommandQueue* GetResourceCmdQueue() const { return resourceQueue.get(); }


	private:
		static ArGraphics* instance;
		static constexpr int NumBackBuffers = 2;
		static constexpr int NumCmdLists = 1;
		Microsoft::WRL::ComPtr<ID3D12Device> mDevice;
		Microsoft::WRL::ComPtr<ID3D12DebugDevice> mDebugDevice;
		Microsoft::WRL::ComPtr<IDXGIFactory6> mFactory;
		Microsoft::WRL::ComPtr<IDXGISwapChain4> mSwapChain;
		std::vector<std::unique_ptr<Frame::FrameResource>> frameResources;
		Frame::FrameResource* curFrameResource;

		std::unique_ptr<Dx12::ArCommandQueue> resourceQueue;
		std::unique_ptr<Dx12::ArCommandBundle> resourceCmdBundle;
		std::unique_ptr<Dx12::ArCommandQueue> renderingQueue;
		std::unique_ptr<Descriptor::ArDescriptorHeap> rtvHeap;
		std::unique_ptr<Descriptor::ArDescriptorHeap> srvCbvHeap;
		std::unique_ptr<Descriptor::ArDescriptorHeap> dsvHeap;
		std::unique_ptr<Descriptor::ArDescriptorHeap> imGuiHeap;

		D3D12_VIEWPORT viewport;
		D3D12_RECT scissorRect;
		
		
		float clearColor[4];
		float windowWidth;
		float windowHeight;

		Camera* camera;
		Camera* nextCamera;
		Light* light;

	public:
		std::unique_ptr<FrameBuffer> frameBuffer[8];
	};

	HRESULT CreateDevice(IDXGIFactory6* factory, ID3D12Device** device);

	HRESULT CreateSwapChain(HWND hWnd, IDXGIFactory2* factory, UINT windowWidth, UINT windowHeight, 
	                        UINT NumBackBuffers, ID3D12CommandQueue* cmdQueue, IDXGISwapChain4** ppSwapChain);
}
