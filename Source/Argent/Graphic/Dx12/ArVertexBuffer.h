#pragma once
#include <d3d12.h>
#include <wrl.h>
#include "../../Other/ArHelper.h"

namespace Argent::Dx12
{
	template<typename T>
	class ArVertexBuffer
	{
	public:
		ArVertexBuffer(ID3D12Device* device, std::vector<T> vertices, T** map = nullptr);
		~ArVertexBuffer() = default;

		ArVertexBuffer(const ArVertexBuffer&) = delete;
		ArVertexBuffer(const ArVertexBuffer&&) = delete;
		const ArVertexBuffer& operator=(const ArVertexBuffer&) = delete;
		const ArVertexBuffer& operator=(const ArVertexBuffer&&) = delete;

		void SetOnCommandList(ID3D12GraphicsCommandList* cmdList, UINT startSlot, UINT numViews) const
		{
			cmdList->IASetVertexBuffers(startSlot, numViews, &view);
		}

	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> buffer;
		D3D12_VERTEX_BUFFER_VIEW view{};
	};

	template <typename T>
	::Argent::Dx12::ArVertexBuffer<T>::ArVertexBuffer(ID3D12Device* device, std::vector<T> vertices, T** map)
	{
		if(map == nullptr)
		{
			T* tmpMap{};
			view = Argent::Helper::Dx12::Buffer::CreateVertex<T>(device, static_cast<UINT>(vertices.size()), vertices.begin(), 
				vertices.end(), &tmpMap, buffer.ReleaseAndGetAddressOf());
		}
		else
		{
			view = Argent::Helper::Dx12::Buffer::CreateVertex<T>(device, static_cast<UINT>(vertices.size()), vertices.begin(), 
				vertices.end(), map, buffer.ReleaseAndGetAddressOf(), false);
		}
	}

}