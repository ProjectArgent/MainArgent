#include "ArMaterial.h"
#include "ArResourceManager.h"


namespace Argent::Material
{
	ArMaterial::ArMaterial(const char* textureFilepath):
		color(DirectX::XMFLOAT4(1, 1, 1, 1))
	{
		if(textureFilepath[0] != NULL)
		{
			textures.emplace_back(std::make_unique<Texture::ArTexture>(textureFilepath));
		}
	}

	void ArMaterial::Render(ID3D12GraphicsCommandList* cmdList, UINT RootParameterIndex) const
	{
		textures.at(0)->Render(cmdList, RootParameterIndex);
	}

#ifdef _DEBUG
	void ArMaterial::DrawDebug()
	{
		if(ImGui::TreeNode("Material"))
		{
			ImGui::Text("%f", textures.at(0)->GetWidth());
			ImGui::Text("%f", textures.at(0)->GetHeight());
			ImGui::Image(reinterpret_cast<ImTextureID>(textures.at(0)->GetImDescriptor()->GetGPUHandle().ptr), ImVec2(128, 128));
			color.DrawDebug();
			ImGui::TreePop();
		}
	}
#endif
}
