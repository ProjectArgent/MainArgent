#pragma once
#include <unordered_map>
#include <d3d12.h>
#include <memory>
#include <wrl.h>
#include <string>
#include "../GameObject/Component/Renderer/Data/ArMesh.h"

#include "../GameObject/Component/Renderer/Data/ArResource.h"


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

		/**
		 * \brief リソース管理用ユニークidを生み出す
		 * \return 
		 */
		[[nodiscard]]static UINT64 GenerateResourceUniqueId()
		{
			static UINT64 uniqueId = 0;
			const UINT64 ret = uniqueId;
			++uniqueId;
			return ret;
		}


	private:
		std::unordered_map<std::wstring, Microsoft::WRL::ComPtr<ID3D12Resource>> textures;
		std::unordered_map<std::string, std::unique_ptr<Argent::Data::ArMesh>> meshData;
		//std::unordered_map<std::string, std::unique_ptr<MeshData>> meshData;


		std::unordered_map<uint64_t, std::weak_ptr<Argent::Resource::ArResource>> resources;
		 
	public:
		static ArResourceManager& Instance()
		{
			static ArResourceManager instance;
			return instance;
		}
	};
}
