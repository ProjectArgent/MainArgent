#pragma once
#include <unordered_map>
#include <d3d12.h>
#include <memory>
#include <wrl.h>
#include <string>
#include <filesystem>
#include "ArStaticMesh.h"

#include "ArResource.h"
#include "ArTexture.h"


namespace Argent::Resource
{
	class ArResourceManager
	{
		static constexpr uint64_t errorUniqueId = 0;
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
		[[nodiscard]] Argent::Mesh::StaticMesh::ArStaticMesh* GetMeshData(const std::string& meshName) const;
		//[[nodiscard]] const MeshData* AddMeshData(const std::string& meshName, std::unique_ptr<MeshData> mesh);

		void AddMesh(std::string name, std::unique_ptr<Mesh::StaticMesh::ArStaticMesh> mesh)
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

		/**
		 * \brief リソース管理用ユニークidを生み出す
		 * \return 
		 */
		[[nodiscard]]static UINT64 GenerateResourceUniqueId()
		{
			static UINT64 uniqueId = 1;
			const UINT64 ret = uniqueId;
			++uniqueId;
			return ret;
		}

		std::shared_ptr<Argent::Resource::ArResource> GetResource(uint64_t uniqueId) const  // NOLINT(modernize-use-nodiscard)
		{
			const auto it = resources.find(uniqueId);
			if(it == resources.end()) _ASSERT_EXPR(FALSE, "missing id");
			if(it->second.expired()) _ASSERT_EXPR(FALSE, "missing resource");

			return it->second.lock();
		}


		std::shared_ptr<Argent::Resource::ArResource> LoadTexture(const char* filePath);

		std::shared_ptr<Argent::Resource::ArResource> LoadFbx(const char* filePath);

	private:

		std::shared_ptr<Argent::Resource::ArResource> FindResourceFromFilePath(const char* filePath) const
		{
			for(auto& res : resources)
			{
				if(res.second.lock()->CompareName(filePath) && !res.second.expired())
				{
					return res.second.lock();
				}
			}

			return nullptr;
		}

		std::unordered_map<std::wstring, Microsoft::WRL::ComPtr<ID3D12Resource>> textures;
		std::unordered_map<std::string, std::unique_ptr<Argent::Mesh::StaticMesh::ArStaticMesh>> meshData;

		std::unordered_map<uint64_t, std::weak_ptr<Argent::Resource::ArResource>> resources;
		//std::unordered_map<uint64_t, std::weak_ptr<Argent::Texture::ArTexture>> textures;
		 
	public:
		static ArResourceManager& Instance()
		{
			static ArResourceManager instance;
			return instance;
		}

	};
}
