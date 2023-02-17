#pragma once

#include <d3d12.h>



namespace ImguiCtrl
{
	void Initialize(HWND hWnd, ID3D12Device* device, ID3D12DescriptorHeap* heap);
	void Terminate();

	void Begin(const char* windowName = "ImGui Window");
	void End(ID3D12GraphicsCommandList* cmdList, ID3D12DescriptorHeap* heap);
	
}
