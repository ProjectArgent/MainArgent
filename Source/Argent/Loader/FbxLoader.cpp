#include "FbxLoader.h"
#include <fbxsdk.h>
#include "../Resource/ArResourceManager.h"

namespace Argent::Loader::Fbx
{
	//データの読み込みとアニメーションのアップデートに使ってる
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

		//todo 何のやつでしょう？
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

	void Traverse(FbxNode* fbxNode, SkinnedScene& sceneView)
	{
		SkinnedScene::Node& node{ sceneView.nodes.emplace_back() };
		node.attribute = fbxNode->GetNodeAttribute() ?
			fbxNode->GetNodeAttribute()->GetAttributeType() :
			FbxNodeAttribute::EType::eUnknown;
		
		node.name = fbxNode->GetName();
		node.id = fbxNode->GetUniqueID();
		node.parentIndex = sceneView.IndexOf(fbxNode->GetParent() ?
			fbxNode->GetParent()->GetUniqueID() : 0);
		for(int childIndex = 0; childIndex < fbxNode->GetChildCount(); ++childIndex)
		{
			Traverse(fbxNode->GetChild(childIndex), sceneView);
		}
	}

	void CreateSkinnedMeshFromFBX()
	{

	}

	void LoadFbx(const char* fileName, bool triangulate)
	{
		//FbxManager* mgr{ FbxManager::Create() };
		//FbxScene* scene{ FbxScene::Create(mgr, "") };
		//FbxImporter* importer{ FbxImporter::Create(mgr, "") };
		//SkinnedScene sceneView{};
		//bool result{};
		//result = importer->Initialize(fileName);
		//_ASSERT_EXPR(result, L"import Initialize failed");

		//result = importer->Import(scene);
		//_ASSERT_EXPR(result, L"Import failed");

		////todo 三角化するかどうかをsdkの関数でしたい（関数あるかも知らない）
		//if(triangulate)
		//{
		//	FbxGeometryConverter converter(mgr);
		//	converter.Triangulate(scene, true);
		//	converter.RemoveBadPolygonsFromMeshes(scene);
		//}

		////再帰関数　シーン内に存在するすべてのノードを取り込む
		////todo nullノードとかあるらしいがそういうのって飛ばしちゃだめなのか？
		//Traverse(scene->GetRootNode(), sceneView);

		//mgr->Destroy();

		//std::vector<Argent::Mesh::ArStaticMesh::Vertex> vertices;
		//std::vector<uint32_t> indices;

		//Argent::Resource::ArResourceManager::Instance().AddMesh("DemoFBX", std::make_unique<Argent::Mesh::ArStaticMesh>(vertices, indices));
		////Argent::Resource::ArResourceManager::Instance().AddMesh(fileName, std::make_unique<Argent::Mesh::ArStaticMesh>(vertices, indices));

	}

}
