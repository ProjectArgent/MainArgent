#include "FbxLoader.h"
#include <fbxsdk.h>
#include <functional>

namespace Argent::Loader::Fbx
{
	void LoadFbx(const char* fileName, bool triangulate)
	{
		FbxManager* mgr{ FbxManager::Create() };
		FbxScene* scene{ FbxScene::Create(mgr, "") };
		FbxImporter* importer{ FbxImporter::Create(mgr, "") };
		SkinnedScene sceneView{};
		bool result{};
		result = importer->Initialize(fileName);
		_ASSERT_EXPR(result, L"import Initialize failed");

		result = importer->Import(scene);
		_ASSERT_EXPR(result, L"Import failed");

		//todo �O�p�����邩�ǂ�����sdk�̊֐��ł������i�֐����邩���m��Ȃ��j
		if(triangulate)
		{
			FbxGeometryConverter converter(mgr);
			converter.Triangulate(scene, true);
			converter.RemoveBadPolygonsFromMeshes(scene);
		}

		//�ċA�֐��@�V�[�����ɑ��݂��邷�ׂẴm�[�h����荞��
		//todo null�m�[�h�Ƃ�����炵�������������̂��Ĕ�΂����Ⴞ�߂Ȃ̂��H
		std::function<void(FbxNode*)> Traverse
		{
			[&](FbxNode* fbxNode)
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
					Traverse(fbxNode->GetChild(childIndex));
				}
			}
		};

		Traverse(scene->GetRootNode());


		mgr->Destroy();
	}
}
