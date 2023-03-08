#pragma once
#include <d3d12.h>
#include <string>
#include <wrl.h>
#include "../../../../Graphic/Dx12/ArDescriptor.h"
#include "ArResource.h"

namespace Argent::Texture
{
	class ArTexture:
		public Argent::Resource::ArResource
	{
	public:
		ArTexture(std::wstring filepath);

		void Render(ID3D12GraphicsCommandList* cmdList, UINT RootParameterIndex) const;
		ID3D12Resource* GetTexture() const { return shaderResource.Get(); }
		float GetWidth() const { return width; }
		float GetHeight() const { return height; }
		Descriptor::ArDescriptor* GetDescriptor() const { return descriptor; }
		Descriptor::ArDescriptor* GetImDescriptor() const { return imDescriptor; }
	private:
		HRESULT Load();
	private:
		std::wstring filepath;
		Descriptor::ArDescriptor* descriptor;
		Descriptor::ArDescriptor* imDescriptor;
		Microsoft::WRL::ComPtr<ID3D12Resource> shaderResource;
		float width;
		float height;
	};
}
