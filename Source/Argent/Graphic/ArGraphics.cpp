#include "ArGraphics.h"
#include <DirectXTex.h>
#include <vector>
#include <string>
#include <d3dx12.h>
#include "../Other/Misc.h"
#include "../GameObject/GameObject.h"
#include "../Other/ArHelper.h"
#include "../Core/ArRandom.h"

namespace Argent::Graphics
{

	ArGraphics* ArGraphics::instance =  nullptr;

	ArGraphics::ArGraphics(HWND hWnd):
		camera(nullptr)
		,	light(nullptr)
		,	nextCamera(nullptr)
		,	curFrameResource(nullptr)
		,	renderingQueue(nullptr)
		,	resourceQueue(nullptr)
	{
		if(instance) _ASSERT_EXPR(FALSE, L"Already Instantiated");
		instance = this;
		HRESULT hr{ S_OK };
		
		
		RECT rc;
		GetClientRect(hWnd, &rc);
		windowWidth = static_cast<float>(rc.right - rc.left);
		windowHeight = static_cast<float>(rc.bottom - rc.top);
		clearColor[0] = 0.7f;
		clearColor[1] = 0.4f;
		clearColor[2] = 0.6f;
		clearColor[3] = 1.0f;
		
#ifdef _DEBUG
		Microsoft::WRL::ComPtr<IDXGIFactory2> tmpFactory;
		hr = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(tmpFactory.ReleaseAndGetAddressOf()));
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));;
		
		hr = tmpFactory->QueryInterface(IID_PPV_ARGS(mFactory.ReleaseAndGetAddressOf()));
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));;
#else
			hr = CreateDXGIFactory1(IID_PPV_ARGS(mFactory.ReleaseAndGetAddressOf()));
			_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));;
#endif

		hr = CreateDevice(mFactory.Get(), mDevice.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

		//#ifdef _DEBUG
		//			ID3D12Debug* debugLayer = nullptr;
		//			hr = D3D12GetDebugInterface(IID_PPV_ARGS(&debugLayer));
		//			_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		//
		//			debugLayer->EnableDebugLayer();
		//			debugLayer->Release();
		//#endif
		//

#ifdef _DEBUG
		ID3D12DebugDevice2* dDevice;
		hr = mDevice->QueryInterface(IID_PPV_ARGS(&dDevice));
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		dDevice->ReportLiveDeviceObjects(D3D12_RLDO_DETAIL | D3D12_RLDO_IGNORE_INTERNAL);
		dDevice->Release();
#endif


		
		renderingQueue = std::make_unique<Dx12::ArCommandQueue>(mDevice.Get());
		resourceQueue = std::make_unique<Dx12::ArCommandQueue>(mDevice.Get());
		resourceCmdBundle = std::make_unique<Dx12::ArCommandBundle>(mDevice.Get());

		hr = CreateSwapChain(hWnd, mFactory.Get(), static_cast<UINT>(windowWidth), static_cast<UINT>(windowHeight), NumBackBuffers, renderingQueue->cmdQueue.Get(), mSwapChain.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		

		rtvHeap = std::make_unique<Descriptor::ArDescriptorHeap>(mDevice.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, NumBackBuffers);
		srvCbvHeap = std::make_unique<Descriptor::ArDescriptorHeap>(mDevice.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 10000);
		dsvHeap = std::make_unique<Descriptor::ArDescriptorHeap>(mDevice.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 100);
		imGuiHeap = std::make_unique<Descriptor::ArDescriptorHeap>(mDevice.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 10001);

		//imguiに画像を表示するために　一個目の空間はimgui用 それ以降はテクスチャ表示用
		Descriptor::ArDescriptor* tmp = imGuiHeap->PopDescriptor();

		frameResources.resize(NumBackBuffers);

		for(UINT i = 0; i < NumBackBuffers; ++i)
		{
			frameResources.at(i) = std::make_unique<Frame::FrameResource>(mDevice.Get(), mSwapChain.Get(),
			                                                              i, rtvHeap->PopDescriptor(), dsvHeap->PopDescriptor(), srvCbvHeap->PopDescriptor(), NumCmdLists);
		}

		//ビューポート　シザー矩形
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width = windowWidth;
		viewport.Height = windowHeight;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;

		//シザー矩形
		scissorRect.left =0;
		scissorRect.top = 0;
		scissorRect.right = static_cast<LONG>(windowWidth);
		scissorRect.bottom = static_cast<LONG>(windowHeight);

		for(auto& buffer : frameBuffer)
		{
			buffer = std::make_unique<FrameBuffer>(mDevice.Get(), frameResources.at(0)->GetBackBufferDesc(), 
			                                       clearColor, rtvHeap->GetDesc());
		}

		mDevice->SetName(L"Device");
		resourceCmdBundle->Begin();
	}

	void ArGraphics::Initialize()
	{
	}

	void ArGraphics::Terminate()
	{

	}

	void ArGraphics::Begin()
	{
		const UINT backBufferIndex = mSwapChain->GetCurrentBackBufferIndex();
		curFrameResource = frameResources.at(backBufferIndex).get();
		curFrameResource->Begin();
		if(nextCamera)
		{
			if(camera)
				camera->OffSceneCamera();
			camera = nextCamera;
			camera->OnSceneCamera();
			nextCamera = nullptr;
		}
		
		if(camera && light)
		{
			curFrameResource->UpdateSceneConstant(camera->GetViewMatrix(),
				camera->GetProjectionMatrix(), light->GetColor().GetColor(),
			                                      light->GetOwner()->GetTransform()->GetPosition(),
			                                      camera->GetOwner()->GetTransform()->GetPosition()
			);
		}
			
		
		const auto dsvHandle = curFrameResource->dsv->GetCPUHandle();
		curFrameResource->SetBarrier(D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

		
		//レンダーターゲット
		const auto rtvHandle = curFrameResource->rtv->GetCPUHandle();

		//深度
		curFrameResource->GetCmdList()->OMSetRenderTargets(1, &rtvHandle, true, &dsvHandle);
		curFrameResource->GetCmdList()->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
		curFrameResource->GetCmdList()->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
		
		curFrameResource->GetCmdList()->RSSetViewports(1, &viewport);
		curFrameResource->GetCmdList()->RSSetScissorRects(1, &scissorRect);
	}
		
	void ArGraphics::End()
	{
		curFrameResource->SetBarrier(D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

		curFrameResource->GetCmdList()->Close();
		ID3D12CommandList* cmdlists[] { curFrameResource->GetCmdList() };
		renderingQueue->cmdQueue->ExecuteCommandLists(1, cmdlists);
		renderingQueue->SetFence(1);
		
		mSwapChain->Present(1, 0);
	}

	HRESULT ArGraphics::CreateWhiteTexture(ID3D12Resource** resource)
	{
		HRESULT hr{ S_OK };
		
		
		D3D12_HEAP_PROPERTIES heapProp{};
		heapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
		heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
		heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
		heapProp.VisibleNodeMask = 0;
		
		D3D12_RESOURCE_DESC resDesc{};
		resDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		resDesc.Width = 4;
		resDesc.Height = 4;
		resDesc.DepthOrArraySize = 1;
		resDesc.SampleDesc.Count = 1;
		resDesc.SampleDesc.Quality = 0;
		resDesc.MipLevels = 1;
		resDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
		
		hr = mDevice->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, 
		                                      &resDesc, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, nullptr,
		                                      IID_PPV_ARGS(resource));
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

		std::vector<unsigned char> data(4 * 4 * 4);
		std::fill(data.begin(), data.end(), 0xff);
		hr = (*resource)->WriteToSubresource(0, nullptr, data.data(), 4 * 4, static_cast<UINT>(data.size()));
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		return hr;
	}
		
	HRESULT ArGraphics::CreateBlackTexture(ID3D12Resource** resource)
	{
		HRESULT hr{ S_OK };
		
		
		D3D12_HEAP_PROPERTIES heapProp{};
		heapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
		heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
		heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
		heapProp.VisibleNodeMask = 0;
		
		D3D12_RESOURCE_DESC resDesc{};
		resDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		resDesc.Width = 4;
		resDesc.Height = 4;
		resDesc.DepthOrArraySize = 1;
		resDesc.SampleDesc.Count = 1;
		resDesc.SampleDesc.Quality = 0;
		resDesc.MipLevels = 1;
		resDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
		
		hr = mDevice->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, 
		                                      &resDesc, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, nullptr,
		                                      IID_PPV_ARGS(resource));
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));;
		
		if(FAILED(hr))
		{
			*resource = nullptr;
			return hr;
		}
		
		std::vector<unsigned char> data(4 * 4 * 4);
		std::fill(data.begin(), data.end(), 0x00);
		hr = (*resource)->WriteToSubresource(0, nullptr, data.data(), 4 * 4, static_cast<UINT>(data.size()));
		return hr;
	}
		
	HRESULT ArGraphics::CreateGrayGradationTexture(ID3D12Resource** resource)
	{
		HRESULT hr{ S_OK };
		
		UINT width = 4;
		UINT height = 256;
		
		D3D12_HEAP_PROPERTIES heapProp{};
		heapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
		heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
		heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
		heapProp.VisibleNodeMask = 0;
		
		D3D12_RESOURCE_DESC resDesc{};
		resDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		resDesc.Width = width;
		resDesc.Height = height;
		resDesc.DepthOrArraySize = 1;
		resDesc.SampleDesc.Count = 1;
		resDesc.SampleDesc.Quality = 0;
		resDesc.MipLevels = 1;
		resDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
		
		hr = mDevice->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, 
		                                      &resDesc, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, nullptr,
		                                      IID_PPV_ARGS(resource));
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));;
		
		if(FAILED(hr))
		{
			*resource = nullptr;
			return hr;
		}
		
		std::vector<unsigned int> data(width * height);
		auto it = data.begin();
		unsigned int c = 0xff;
		for(; it != data.end(); it += 4)
		{
			auto col = (0xff << 24) | RGB(c, c, c);
			std::fill(it, it + 4, col);
			--c;
		}
		
		hr = (*resource)->WriteToSubresource(0, nullptr, data.data(),
		                                     width * sizeof(unsigned int),
		                                     static_cast<UINT>(sizeof(unsigned int) * data.size()));
		return hr;
	}

	HRESULT ArGraphics::CreateNoiseTexture(ID3D12Resource** resource)
	{
		HRESULT hr{ S_OK };


		D3D12_HEAP_PROPERTIES heapProp{};
		heapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
		heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
		heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
		heapProp.VisibleNodeMask = 0;

		D3D12_RESOURCE_DESC resDesc{};
		resDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		resDesc.Width = 256;
		resDesc.Height = 256;
		resDesc.DepthOrArraySize = 1;
		resDesc.SampleDesc.Count = 1;
		resDesc.SampleDesc.Quality = 0;
		resDesc.MipLevels = 1;
		resDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		hr = mDevice->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE,
			&resDesc, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, nullptr,
			IID_PPV_ARGS(resource));
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

		struct TexRGBA
		{
			unsigned char R, G, B, A;
		};

		std::vector<TexRGBA> textureBuffer(256 * 256);

		for(auto& buf : textureBuffer)
		{
			buf.R = static_cast<unsigned char>(Argent::Random::Generate(0, 255));
			buf.G = static_cast<unsigned char>(Argent::Random::Generate(0, 255));
			buf.B = static_cast<unsigned char>(Argent::Random::Generate(0, 255));
			buf.A = 255;
		}
		hr = (*resource)->WriteToSubresource(0, nullptr, textureBuffer.data(), sizeof(TexRGBA) * 256, sizeof(TexRGBA) * textureBuffer.size());
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		return hr;
	}

	void ArGraphics::SetSceneConstant(UINT rootParameterIndex)
	{
		curFrameResource->SetSceneConstant(rootParameterIndex);
	}

	HRESULT FindAdapter(IDXGIFactory6* factory, IDXGIAdapter1** adapter)
	{
		HRESULT hr{ S_OK };
		UINT adapterIndex = 0;
		while (true)
		{
			hr = factory->EnumAdapterByGpuPreference(adapterIndex, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(adapter));
			if (FAILED(hr)) break;

			DXGI_ADAPTER_DESC1 adapterDesc;
			(*adapter)->GetDesc1(&adapterDesc);
		
			// ソフトウェアアダプター
			if (adapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;
		
			// デバイスを生成できるかチェック
			hr = D3D12CreateDevice(*adapter, D3D_FEATURE_LEVEL_11_1, _uuidof(ID3D12Device),nullptr);
			if (SUCCEEDED(hr)) return hr;

			adapterIndex++;
		}

		// アダプタの取得
		if (*adapter == nullptr)
		{
			hr = factory->EnumAdapters1(0, adapter);
			if (FAILED(hr))
			{
		
				// 取得できない場合はWarpアダプタを取得
				return factory->EnumWarpAdapter(IID_PPV_ARGS(adapter));
			}
		}
		return E_FAIL;
	}

	HRESULT CreateDevice(IDXGIFactory6* factory, ID3D12Device** device)
	{
		HRESULT hr{ S_OK };
		Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;

		hr = FindAdapter(factory, adapter.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		
		DXGI_ADAPTER_DESC1 adapterDesc;
		adapter->GetDesc1(&adapterDesc);
		
		OutputDebugStringW(adapterDesc.Description);
		OutputDebugStringW(L"\n");
		
		// ドライバの機能レベル（12_0でDirectX12の基準を完全に満たしたもの）
		constexpr D3D_FEATURE_LEVEL d3dFeatureLevels[] =
		{
			D3D_FEATURE_LEVEL_12_1,
			D3D_FEATURE_LEVEL_12_0,
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0,
		};
		// フィーチャーレベルを高いレベルから徐々に下げながらデバイスを生成する
		for (const D3D_FEATURE_LEVEL& d3dFeatureLevel : d3dFeatureLevels)
		{
			hr = D3D12CreateDevice(adapter.Get(), d3dFeatureLevel,	IID_PPV_ARGS(device));
			if (SUCCEEDED(hr))
			{
				(*device)->SetName(L"Device");
				return hr;
			}
		}
		return E_FAIL;
	}

	HRESULT CreateSwapChain(HWND hWnd, IDXGIFactory2* factory, UINT windowWidth, UINT windowHeight, UINT NumBackBuffers, ID3D12CommandQueue* cmdQueue, IDXGISwapChain4** ppSwapChain)
	{
		HRESULT hr{ S_OK };
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
		Microsoft::WRL::ComPtr<IDXGISwapChain1> tempSwapChain;
		swapChainDesc.Width = windowWidth;
		swapChainDesc.Height =  windowHeight;
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.Stereo = FALSE;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;
		swapChainDesc.BufferCount = NumBackBuffers;
		swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		hr = factory->CreateSwapChainForHwnd(cmdQueue, hWnd, &swapChainDesc,
		                                     nullptr, nullptr, tempSwapChain.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));;
		hr = tempSwapChain->QueryInterface(IID_PPV_ARGS(ppSwapChain));
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));;

		return hr;
	}


	//HRESULT ArGraphics::CreateSwapChain(HWND hWnd)
	//{
	//	HRESULT hr{ S_OK };
	//	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	//	Microsoft::WRL::ComPtr<IDXGISwapChain1> tempSwapChain;
	//	swapChainDesc.Width = static_cast<UINT>(windowWidth);
	//	swapChainDesc.Height = static_cast<UINT>(windowHeight);
	//	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	//	swapChainDesc.Stereo = FALSE;
	//	swapChainDesc.SampleDesc.Count = 1;
	//	swapChainDesc.SampleDesc.Quality = 0;
	//	swapChainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;
	//	swapChainDesc.BufferCount = NumBackBuffers;
	//	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	//	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	//	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	//	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	//	hr = mFactory->CreateSwapChainForHwnd(renderingQueue->cmdQueue.Get(), hWnd, &swapChainDesc, 
	//		nullptr, nullptr, tempSwapChain.ReleaseAndGetAddressOf());
	//	_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));;
	//	hr = tempSwapChain->QueryInterface(IID_PPV_ARGS(mSwapChain.GetAddressOf()));
	//	_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));;
	//
	//	return hr;
	//}
}
