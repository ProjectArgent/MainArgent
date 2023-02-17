#pragma once
#include <unordered_map>
#include <d3d12.h>
#include <wrl.h>
#include <string>

namespace Argent::Resource
{
	class ArResourceManager
	{
		ArResourceManager() = default;
	public:
		~ArResourceManager() = default;
		ArResourceManager(const ArResourceManager&) = delete;
		ArResourceManager(const ArResourceManager&&) = delete;
		ArResourceManager operator=(const ArResourceManager&) = delete;
		ArResourceManager operator=(const ArResourceManager&&) = delete;


		bool FindTexture(const wchar_t* filepath, ID3D12Resource** ppResource);

	private:
		std::unordered_map<std::wstring, Microsoft::WRL::ComPtr<ID3D12Resource>> textures;


	public:
		static ArResourceManager& Instance()
		{
			static ArResourceManager instance;
			return instance;
		}
	};
}
