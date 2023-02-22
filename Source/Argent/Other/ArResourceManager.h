#pragma once
#include <unordered_map>
#include <d3d12.h>
#include <memory>
#include <wrl.h>
#include <string>
#include "../GameObject/Component/Renderer/Data/ArMesh.h"

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
		[[nodiscard]] Argent::Data::ArMesh* GetMeshData(const std::string& meshName) const;
		//[[nodiscard]] const MeshData* AddMeshData(const std::string& meshName, std::unique_ptr<MeshData> mesh);

		void AddMesh(std::string name, std::unique_ptr<Data::ArMesh> mesh)
		{
			for(const auto& data : meshData)
			{
				if(data.first == name)
				{
					return;
				}
			}

			meshData[name].swap(mesh);
		}

	private:
		std::unordered_map<std::wstring, Microsoft::WRL::ComPtr<ID3D12Resource>> textures;
		std::unordered_map<std::string, std::unique_ptr<Argent::Data::ArMesh>> meshData;
		//std::unordered_map<std::string, std::unique_ptr<MeshData>> meshData;

	public:
		static ArResourceManager& Instance()
		{
			static ArResourceManager instance;
			return instance;
		}
	};
}
