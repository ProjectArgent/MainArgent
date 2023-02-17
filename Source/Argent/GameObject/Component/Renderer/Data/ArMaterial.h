#pragma once
#include <d3d12.h>
#include <memory>
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

		/**
		 * \brief todo ��قǎg�������@�e�N�X�`����ێ����Ă��Ȃ��ꍇ�̓_�~�[�e�N�X�`�����g���H�����ĂȂ��Ƃ����蓾�񂩂�Ƃ肠�������F�̃e�N�X�`���ł����H
		 * ���邢��bool�^�ł��V�F�[�_�[�ɓn����null�̂Ƃ��ɂ͎g��Ȃ��悤�ɂ���H
		 * \return true = �e�N�X�`�������Ă�
		 */
		bool HasTexture() const { return texture != nullptr; }

		std::unique_ptr<Image::ArImage> texture;
		Color color;
	};
}
