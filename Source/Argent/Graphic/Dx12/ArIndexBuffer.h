#pragma once
#include <d3d12.h>
#include <wrl.h>
#include "../../Other/ArHelper.h"

namespace Argent::Dx12
{
	class ArIndexBuffer
	{
	public:
		ArIndexBuffer(ID3D12Device* device, std::vector<uint32_t> indices);
		~ArIndexBuffer() = default;

		ArIndexBuffer(const ArIndexBuffer&) = delete;
		ArIndexBuffer(const ArIndexBuffer&&) = delete;
		const ArIndexBuffer& operator=(const ArIndexBuffer&) = delete;
		const ArIndexBuffer& operator=(const ArIndexBuffer&&) = delete;

		void SetOnCommandList(ID3D12GraphicsCommandList* cmdList) const
		{
			cmdList->IASetIndexBuffer(&view);
		}

	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> buffer;
		D3D12_INDEX_BUFFER_VIEW view{};
	};

	inline ArIndexBuffer::ArIndexBuffer(ID3D12Device* device, std::vector<uint32_t> indices)
	{
		view = Argent::Helper::Dx12::Buffer::CreateIndex<uint32_t>(device, static_cast<UINT>(indices.size()), indices.begin(),
			indices.end(), buffer.ReleaseAndGetAddressOf());
	}
}
