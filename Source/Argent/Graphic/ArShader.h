#pragma once
#include <memory>

namespace Argent::Shader
{
	class ArShader
	{
	public:
		ArShader(const char* filepath);
		
	public:
		~ArShader() = default;
		bool Read(const char* filename);
		
	public:
		[[nodiscard]] void* GetData() const { return data.get(); }
		[[nodiscard]] uint64_t GetSize() const { return size; }
			
	private:
		const char* filepath;
		std::unique_ptr<uint8_t[]> data;
		uint64_t size;
	};
}
