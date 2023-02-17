#include "ArResourceManager.h"

namespace Argent::Resource
{
	bool ArResourceManager::FindTexture(const wchar_t* filepath, ID3D12Resource** ppResource)
	{
		const std::wstring tmp = filepath;
		const auto it = textures.find(tmp);

		if (it != textures.end())
		{
			*ppResource = (*it).second.Get();
			(*it).second->AddRef();
			return true;
		}

		*ppResource = nullptr;
		return false;
	}
	
}
