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
		//�f�[�^�̓ǂݍ��݂ƃA�j���[�V�����̃A�b�v�f�[�g�Ɏg���Ă�
		struct SkinnedScene
		{
			struct Node
			{
				uint64_t id{};
				std::string name;
				FbxNodeAttribute::EType attribute{};
				int64_t parentIndex{ -1 };
			};
			std::vector<Node> nodes;

			//todo ���̂�ł��傤�H
			int64_t IndexOf(uint64_t id) const
			{
				int64_t index{};
				for(const Node& node : nodes)
				{
					if(node.id == id) return index;
					++index;
				}
				return -1;
			}
		};
		void LoadFbx(const char* fileName, bool triangulate = false);


		void FetchMesh(FbxScene* fbxScene, std::vector<Argent::Data::ArMesh>& meshes);
	}
}
