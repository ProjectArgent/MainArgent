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
		struct SkinnedScene;

		void LoadFbx(const char* fileName, bool triangulate = false);


		void FetchMesh(FbxScene* fbxScene, std::vector<Data::ArMesh::Vertex>& vertices, std::vector<uint32_t>& indices, const SkinnedScene& sceneView);

	}
}
