#pragma once
#include <unordered_map>
#include <d3d12.h>
#include <memory>
#include <wrl.h>
#include <string>
#include "../GameObject/Component/Renderer/Data/ArMeshData.h"

namespace Argent::Resource
{
	class ArResourceManager
	{
		ArResourceManager()
		{
			Clear();
		}
	public:
		~ArResourceManager() = default;
		ArResourceManager(const ArResourceManager&) = delete;
		ArResourceManager(const ArResourceManager&&) = delete;
		ArResourceManager operator=(const ArResourceManager&) = delete;
		ArResourceManager operator=(const ArResourceManager&&) = delete;

		void Initialize();
		void Clear();
		void Begin();

		bool FindTexture(const wchar_t* filepath, ID3D12Resource** ppResource);
		[[nodiscard]] Argent::Data::ArMeshData* GetMeshData(const std::string& meshName) const;
		//[[nodiscard]] const MeshData* AddMeshData(const std::string& meshName, std::unique_ptr<MeshData> data);

	private:
		std::unordered_map<std::wstring, Microsoft::WRL::ComPtr<ID3D12Resource>> textures;
		std::unordered_map<std::string, std::unique_ptr<Argent::Data::ArMeshData>> meshData;
		//std::unordered_map<std::string, std::unique_ptr<MeshData>> meshData;

	public:
		static ArResourceManager& Instance()
		{
			static ArResourceManager instance;
			return instance;
		}
	};
}
