#pragma once
#include <DirectXMath.h>
#include "../../../imgui/imgui.h"

namespace Argent
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
}
