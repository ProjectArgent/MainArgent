#include "ArCommand.h"
#include<d3d12.h>
#include <dxgi1_6.h>

namespace Argent::Dx12
{
	ArCommandBundle::ArCommandBundle(ID3D12Device* device)
	{
		HRESULT hr{ S_OK };

		hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, 
		                                    IID_PPV_ARGS(cmdAlloc.ReleaseAndGetAddressOf()));
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
		                               cmdAlloc.Get(), nullptr, 
		                               IID_PPV_ARGS(cmdList.ReleaseAndGetAddressOf()));
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		executeCmdLists = cmdList.Get();

		cmdList->SetName(L"CmdLists");
		cmdAlloc->SetName(L"CmdAlloc");
		executeCmdLists->SetName(L"ExeCmdLists");

		cmdList->Close();
	}

	void ArCommandBundle::Begin() const
	{
		cmdAlloc.Get()->Reset();
		cmdList.Get()->Reset(cmdAlloc.Get(), nullptr	);
	}
}
