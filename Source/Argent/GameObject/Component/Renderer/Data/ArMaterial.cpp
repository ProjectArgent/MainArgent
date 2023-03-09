#include "ArMaterial.h"
#include "../imgui/imgui.h"
#include "../../../../Other/ArResourceManager.h"


namespace Argent::Material
{
	ArMaterial::ArMaterial(const char* textureFilepath):
		texture(nullptr)
	,	color(DirectX::XMFLOAT4(1, 1, 1, 1))
	{
		if(textureFilepath[0] != NULL)
		{
			texture = std::make_unique<Texture::ArTexture>(textureFilepath);
		}
	}

	void ArMaterial::Render(ID3D12GraphicsCommandList* cmdList, UINT RootParameterIndex) const
	{
		texture->Render(cmdList, RootParameterIndex);
	}

#ifdef _DEBUG
	void ArMaterial::DrawDebug()
	{
		if(ImGui::TreeNode("Material"))
		{
			ImGui::Text("%f", texture->GetWidth());
			ImGui::Text("%f", texture->GetHeight());
			ImGui::Image(reinterpret_cast<ImTextureID>(texture->GetImDescriptor()->GetGPUHandle().ptr), ImVec2(128, 128));
			color.DrawDebug();
			ImGui::TreePop();
		}
	}
#endif
}
