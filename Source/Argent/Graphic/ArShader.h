#pragma once
#include <memory>
#include "../GameObject/Component/Renderer/Data/ArResource.h"


namespace Argent::Shader
{
	class ArShader:
		public Resource::ArResource
	{
	public:
		ArShader(const char* filepath);
		
	public:
		~ArShader() = default;
		bool Read(const char* filename);
		
		[[nodiscard]] void* GetData() const { return data.get(); }
		[[nodiscard]] uint64_t GetSize() const { return size; }
			
	private:
		std::unique_ptr<uint8_t[]> data;
		uint64_t size;
	};
}
