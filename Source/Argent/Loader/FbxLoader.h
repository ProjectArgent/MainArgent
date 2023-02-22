#pragma once
#include <cstdint>
#include <string>
#include <fbxsdk.h>
#include <vector>
#include "../GameObject/Component/Renderer/Data/ArMesh.h"


namespace Argent::Loader
{
	namespace Fbx
	{
		void LoadFbx(const char* fileName, bool triangulate = false);
	}
}
