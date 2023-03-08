#include "ArTexture.h"
#include "../../../../Graphic/ArGraphics.h"
#include "../../../../Graphic/Dx12/ArDescriptorHeap.h"
#include "../../../../Other/ArHelper.h"
#include "../../../../Other/ArResourceManager.h"

namespace Argent::Texture
{
	ArTexture::ArTexture(std::wstring filepath):
	//todo ファイルパスの最後尾からファイル名を取ってきてそれをnameに入れる
		Argent::Resource::ArResource(Argent::Resource::ArResourceManager::GenerateResourceUniqueId(), "name")
	,	filepath(std::move(filepath))
	{
		descriptor = Graphics::ArGraphics::Instance()->GetHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)->PopDescriptor();
		imDescriptor = Graphics::ArGraphics::Instance()->GetImGuiHeap()->PopDescriptor();
		Load();

		ID3D12Device* device = Graphics::ArGraphics::Instance()->GetDevice();

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		srvDesc.Format = shaderResource->GetDesc().Format;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		device->CreateShaderResourceView(shaderResource.Get(), &srvDesc,
			descriptor->GetCPUHandle());//ヒープを割り当てる場所

		device->CreateShaderResourceView(shaderResource.Get(), &srvDesc,
			imDescriptor->GetCPUHandle());//ヒープを割り当てる場所

		width = static_cast<float>(shaderResource->GetDesc().Width);
		height = static_cast<float>(shaderResource->GetDesc().Height);
	}

	HRESULT ArTexture::Load()
	{
		const HRESULT hr = Helper::Texture::LoadTexture(Argent::Graphics::ArGraphics::Instance()->GetDevice(), Argent::Graphics::ArGraphics::Instance()->GetResourceCmdBundle(), 
		                                                Argent::Graphics::ArGraphics::Instance()->GetResourceCmdQueue(), filepath.c_str(), shaderResource.ReleaseAndGetAddressOf());

		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		return S_OK;
	}


	void ArTexture::Render(ID3D12GraphicsCommandList* cmdList, UINT RootParameterIndex) const
	{
		cmdList->SetDescriptorHeaps(1, descriptor->GetDescriptorHeap()->GetHeapDoublePointer());
		cmdList->SetGraphicsRootDescriptorTable(RootParameterIndex, descriptor->GetGPUHandle());
	}
}
