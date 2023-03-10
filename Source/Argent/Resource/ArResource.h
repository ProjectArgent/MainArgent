#pragma once
#include <filesystem>
#include <string>
#include "../Other/ArHelper.h"

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
			rMaterial,
		};
		ArResource(uint64_t uniqueId, const char* name, ResourceType type):
			uniqueId(uniqueId)
		,	name(name)
		,	type(type)
		{}

		virtual bool CompareName(const char* n)
		{
			return name.compare(n);
		}

		uint64_t GetUniqueId() const { return uniqueId; }  // NOLINT(modernize-use-nodiscard)
		const char* GetName() const { return name.c_str(); }// NOLINT(modernize-use-nodiscard)

	private:
		uint64_t uniqueId;
		std::string name;
		ResourceType type;
	};

	class ArImportedResource:
		public ArResource
	{
	public:
		ArImportedResource(uint64_t uniqueId, const char* filePath, ResourceType type):
			ArResource(uniqueId, Argent::Helper::String::ExtractFileName(filePath, false).c_str(), type)
		,	filePath(filePath)
			{}

		const char* GetFilePath() const { return filePath.c_str(); }

		bool CompareName(const char* n) override
		{
			return ArResource::CompareName(n) || filePath.compare(n);
		}

	protected:
		std::string filePath;
	};
}