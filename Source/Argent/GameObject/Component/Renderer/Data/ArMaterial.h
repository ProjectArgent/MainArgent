#pragma once
#include <d3d12.h>
#include <memory>
#include <vector>
#include <DirectXMath.h>
#include "../imgui/imgui.h"
#include "ArImage.h"

namespace Argent::Material
{
	struct Color
	{
		Color(const DirectX::XMFLOAT4& color = DirectX::XMFLOAT4(1, 1, 1, 1)):
			color(color)
		{}

#ifdef _DEBUG
		void DrawDebug()
		{
			ImGui::ColorEdit4("Color", &color.x,
				ImGuiColorEditFlags_PickerHueWheel | 
				ImGuiColorEditFlags_AlphaBar);
		}
#endif
		DirectX::XMFLOAT4 color;
	};

	struct ArMaterial
	{
	public:
		ArMaterial(const wchar_t* textureFilepath);

		/**
		 * \brief 
		 * \param cmdList 
		 * \param RootParameterIndex 
		 */
		void Render(ID3D12GraphicsCommandList* cmdList, UINT RootParameterIndex) const;
#ifdef _DEBUG
		void DrawDebug();
#endif

		std::unique_ptr<Image::ArImage> texture;
		Color color;
		std::string name;
		DirectX::XMFLOAT4 ka{ 0.2f, 0.2f, 0.2f, 1.0f };
		DirectX::XMFLOAT4 kd{ 0.2f, 0.2f, 0.2f, 1.0f };
		DirectX::XMFLOAT4 ks{ 0.2f, 0.2f, 0.2f, 1.0f };
	};
}
