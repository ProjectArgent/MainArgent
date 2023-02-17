#pragma once
#include <d3d12.h>

namespace Argent::Descriptor
{
	class ArDescriptorHeap;
	class ArDescriptor
	{
	public:
		ArDescriptor(D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle, UINT heapIndex, ArDescriptorHeap* heapPointer);
		~ArDescriptor() = default;
		ArDescriptor(const ArDescriptor&) = delete;
		ArDescriptor(const ArDescriptor&&) = delete;
		ArDescriptor operator=(const ArDescriptor&) = delete;
		ArDescriptor operator=(const ArDescriptor&&) = delete;


		[[nodiscard]] D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle() const { return gpuHandle; }
		[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle() const { return cpuHandle; }
		[[nodiscard]] ArDescriptorHeap* GetDescriptorHeap() const { return heapPointer; }

		UINT GetHeapIndex() const { return heapIndex; }

	private:
		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle;
		D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle;
		UINT heapIndex;
		ArDescriptorHeap* heapPointer;
	};
}
