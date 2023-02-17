#pragma once

#include <d3d12.h>
#include <wrl.h>
#include <vector>
#include <memory>
#include "ArDescriptor.h"


//ディスクリプタ管理用のクラス
//コンストラクタの引数で確保するヒープサイズを指定
//hack　ディスクリプタヒープ管理用のクラスもほしい？
namespace Argent::Descriptor
{

	class ArDescriptorHeap final
	{
	public:
		ArDescriptorHeap(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE type, UINT numDescriptors);
		~ArDescriptorHeap() = default;
		ArDescriptorHeap(const ArDescriptorHeap&) = delete;
		ArDescriptorHeap(const ArDescriptorHeap&&) = delete;
		ArDescriptorHeap operator=(const ArDescriptorHeap&) = delete;
		ArDescriptorHeap operator=(const ArDescriptorHeap&&) = delete;

		[[nodiscard]] UINT GetNumDescriptors() const { return MaxDescriptorCounts; }

		/**  // NOLINT(clang-diagnostic-invalid-utf8, clang-diagnostic-invalid-utf8)
			 * \brief 前から順番に確保したディスクリプターのポインタを割り当てていく 確保領域を超えた場合はアサート
			 * \return ディスクリプターのポインタを返却　
			 */
		[[nodiscard]] ArDescriptor* PopDescriptor();	
		[[nodiscard]] ID3D12DescriptorHeap* const* GetHeapDoublePointer() const { return descriptorHeap.GetAddressOf(); }
		[[nodiscard]] D3D12_DESCRIPTOR_HEAP_DESC GetDesc() const { return descriptorHeap->GetDesc(); }
		[[nodiscard]] ID3D12DescriptorHeap* GetHeapPointer() const { return descriptorHeap.Get(); }
	private:
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap;
		UINT MaxDescriptorCounts;
		UINT heapIncrementSize;
		UINT descriptorStackCounter;//現在のディスクリプター割当数　>= MaxDescriptorCounts のときに新たに割り当てようとするとアサート
		std::vector<std::unique_ptr<ArDescriptor>> descriptors;
	public:
	};
}
