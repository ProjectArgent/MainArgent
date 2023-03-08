#pragma once

namespace Argent::Loader
{
	namespace Fbx
	{
		struct SkinnedScene;

		void LoadFbx(const char* fileName, bool triangulate = false);

	}
}
