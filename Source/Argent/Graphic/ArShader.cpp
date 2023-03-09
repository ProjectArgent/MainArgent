#include "ArShader.h"

#include <d3d12.h>
#include <cassert>
#include <fstream>
#include "../Other/ArResourceManager.h"
namespace Argent::Shader
{
	ArShader::ArShader(const char* filepath):
		ArResource(Resource::ArResourceManager::GenerateResourceUniqueId(), filepath, ResourceType::rShader)
	{
		data = nullptr;
		size = 0;
		const bool hr = Read(filepath);
		assert(hr);
	}
		
	bool ArShader::Read(const char* filename)
	{
		FILE* fp{ nullptr };

		errno_t errorNo{};
		errorNo = fopen_s(&fp, filename, "rb");
		if(!fp) _ASSERT_EXPR(FALSE, L"failed to open shader file");
		
		errorNo = fseek(fp, 0, SEEK_END);
		_ASSERT_EXPR(errorNo == 0, L"Something wrong");
		size = ftell(fp);
		errorNo = fseek(fp, 0, SEEK_SET);
		_ASSERT_EXPR(errorNo == 0, L"Something wrong");

		data.reset(new uint8_t[size]);
		errorNo = static_cast<errno_t>(fread(data.get(), size, 1, fp));
		_ASSERT_EXPR(errorNo != 0, L"Something wrong");
		errorNo = fclose(fp);
		_ASSERT_EXPR(errorNo == 0, L"failed to close shader file");
		return true;
	}
}
