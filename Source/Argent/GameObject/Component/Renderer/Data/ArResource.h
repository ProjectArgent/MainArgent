#pragma once
#include <string>


namespace Argent::Resource
{
	class ArResource
	{
	public:
		ArResource(uint64_t uniqueId, const char* name):
			uniqueId(uniqueId)
		,	name(name)
		{}

		uint64_t GetUniqueId() const { return uniqueId; }  // NOLINT(modernize-use-nodiscard)
		const std::string& GetName() const { return name; }// NOLINT(modernize-use-nodiscard)

	private:
		uint64_t uniqueId;
		std::string name;
	};
}