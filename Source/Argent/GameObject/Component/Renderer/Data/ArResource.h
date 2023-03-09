#pragma once
#include <filesystem>
#include <string>


namespace Argent::Resource
{
	class ArResource
	{
	public:

		enum class ResourceType
		{
			rShader,
			rTexture,
			rMesh,
			rSkinnedMesh,
		};
		ArResource(uint64_t uniqueId, const char* filePath, ResourceType type):
			uniqueId(uniqueId)
		,	filePath(filePath)
		,	type(type)
		{}
		

		uint64_t GetUniqueId() const { return uniqueId; }  // NOLINT(modernize-use-nodiscard)
		const char* GetFilePath() const { return filePath.c_str(); }// NOLINT(modernize-use-nodiscard)

	private:
		uint64_t uniqueId;
		std::string filePath;
		ResourceType type;
	};
}