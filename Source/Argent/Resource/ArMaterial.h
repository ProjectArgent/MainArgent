#pragma once
#include <d3d12.h>
#include <memory>
#include <vector>
#include <DirectXMath.h>
#include "ArTexture.h"
#include "ArResource.h"
#include "../Component/ArColor.h"

namespace Argent::Material
{
	class ArBaseMaterial:
		public Resource::ArResource
	{
	public:
		ArBaseMaterial();
		~ArBaseMaterial() = default;

	protected:
		Color color;
		std::vector<std::shared_ptr<Texture::ArTexture>> textures;
	};

	class ArMaterial
	{
	public:
		ArMaterial(const char* textureFilepath);

		/**
		 * \brief 
		 * \param cmdList 
		 * \param RootParameterIndex 
		 */
		void Render(ID3D12GraphicsCommandList* cmdList, UINT RootParameterIndex) const;
#ifdef _DEBUG
		void DrawDebug();
#endif

		std::vector<std::shared_ptr<Texture::ArTexture>> textures;
		Color color;
		std::string name;
		DirectX::XMFLOAT4 ka{ 0.2f, 0.2f, 0.2f, 1.0f };
		DirectX::XMFLOAT4 kd{ 0.2f, 0.2f, 0.2f, 1.0f };
		DirectX::XMFLOAT4 ks{ 0.2f, 0.2f, 0.2f, 1.0f };
	};

	namespace SkinnedMesh
	{
		class ArStandardMaterial
		{
			
		};
	}
}