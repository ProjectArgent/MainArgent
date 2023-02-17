#pragma once
#include <unordered_map>
#include <memory>
#include <string>
#include "../GameObject/Component/Renderer/Data/MeshData.h"

class ResourceManager
{
private:
	ResourceManager();
	~ResourceManager() = default;

public:
	static ResourceManager& Instance()
	{
		static ResourceManager instance;
		return instance;
	}

	void Initialize();
	void Clear();
	void Begin();

	[[nodiscard]] MeshData* GetMeshData(const std::string& meshName) const;
	[[nodiscard]] const MeshData* AddMeshData(const std::string& meshName, std::unique_ptr<MeshData> data);
protected:

	std::unordered_map<std::string, std::unique_ptr<MeshData>> meshData;
};

