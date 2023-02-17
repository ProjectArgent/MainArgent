#include "ArDescriptor.h"

#include "ArDescriptorHeap.h"
namespace Argent::Descriptor
{
	ArDescriptor::ArDescriptor(D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle,
	                           UINT heapIndex, ArDescriptorHeap* heapPointer):
		cpuHandle(cpuHandle)
		,	gpuHandle(gpuHandle)
		,	heapIndex(heapIndex)
		,	heapPointer(heapPointer)
	{}
}
