#include "ArSkinnedMeshRenderer.h"
#include <functional>
#include <d3dx12.h>
#include <filesystem>
#include "../Core/ArTimer.h"
#include "../Other/Misc.h"
#include "../GameObject/GameObject.h"
#include "../Other/ArHelper.h"


ArSkinnedMeshRenderer::ArSkinnedMeshRenderer(ID3D12Device* device, const char* filename, 
                         float samplingRate, bool triangulate):
	ArRenderer("name")
{
	FbxManager* manager{ FbxManager::Create() };
	FbxScene* fbxScene{ FbxScene::Create(manager, "" ) };

	FbxImporter* importer{ FbxImporter::Create(manager, "") };
	bool importState{ FALSE };
	importState = importer->Initialize(filename);
	_ASSERT_EXPR_A(importState, importer->GetStatus().GetErrorString());

	
	importState = importer->Import(fbxScene);
	_ASSERT_EXPR_A(importState, importer->GetStatus().GetErrorString());

	FbxGeometryConverter converter(manager);
	if(triangulate)
	{
		converter.Triangulate(fbxScene, true, false);
		converter.RemoveBadPolygonsFromMeshes(fbxScene);
	}

	std::function<void(FbxNode* fbxNode)> traverse
	{ [&](FbxNode* fbxNode)
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
				traverse(fbxNode->GetChild(childIndex));
			}
		}
	};

	traverse(fbxScene->GetRootNode());

	FetchMesh(fbxScene, meshes);
	FetchMaterial(fbxScene, materials);
	FetchAnimation(fbxScene, animationClips, samplingRate);

	manager->Destroy();

	CreateComObject(device, filename);

	Argent::Dx12::ArRenderingPipeline::CreateGraphicsPipeline<Argent::Dx12::SkinnedMesh::ArDefaultGraphicsPipeline>(&renderingPipeline,
			"./Resource/Shader/SkinnedMeshVertex.cso",
			"./Resource/Shader/SkinnedMeshPixel.cso");
}


void ArSkinnedMeshRenderer::Render(ID3D12GraphicsCommandList* cmdList, 
	const DirectX::XMFLOAT4X4& world,
	const DirectX::XMFLOAT4& color,
	const Animation::Keyframe* keyframe)
{
	ArRenderer::Render(cmdList);

	Argent::Graphics::ArGraphics::Instance()->SetSceneConstant();


	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	constantMap->world = world;
	constantMap->color = color;
	for(const Mesh& mesh : meshes)
	{

#if 0
		DirectX::XMMATRIX B[3];
		B[0] = DirectX::XMLoadFloat4x4(&mesh.bindPose.bones.at(0).offsetTransform);
		B[1] = DirectX::XMLoadFloat4x4(&mesh.bindPose.bones.at(1).offsetTransform);
		B[2] = DirectX::XMLoadFloat4x4(&mesh.bindPose.bones.at(2).offsetTransform);

		DirectX::XMMATRIX A[3];
		A[0] = DirectX::XMMatrixRotationRollPitchYaw(DirectX::XMConvertToRadians(90), 0, 0);
		A[1] = DirectX::XMMatrixRotationRollPitchYaw(0, 0, DirectX::XMConvertToRadians(45)) * DirectX::XMMatrixTranslation(0, 2, 0);
		A[2] = DirectX::XMMatrixRotationRollPitchYaw(0, 0, DirectX::XMConvertToRadians(-45)) * DirectX::XMMatrixTranslation(0, 2, 0);

		DirectX::XMStoreFloat4x4(&constantMap->boneTransforms[0], B[0] * A[0]);
		DirectX::XMStoreFloat4x4(&constantMap->boneTransforms[1], B[1] * A[1] * A[0]);
		DirectX::XMStoreFloat4x4(&constantMap->boneTransforms[2], B[2] * A[2] * A[1] * A[0]);
#endif

		cmdList->SetDescriptorHeaps(1, constantDescriptor->GetDescriptorHeap()->GetHeapDoublePointer());
		cmdList->SetGraphicsRootDescriptorTable(1, constantDescriptor->GetGPUHandle());

		cmdList->IASetVertexBuffers(0, 1, &mesh.vertexView);
		cmdList->IASetIndexBuffer(&mesh.indexView);

		for(const Mesh::Subset& subset : mesh.subsets)
		{
			const size_t boneCount{ mesh.bindPose.bones.size() };
			for(int boneIndex = 0; boneIndex < boneCount; ++boneIndex)
			{
				const Skeleton::Bone& bone{ mesh.bindPose.bones.at(boneIndex) };
				const Animation::Keyframe::Node& boneNode{ keyframe->nodes.at(bone.nodeIndex) };
				DirectX::XMStoreFloat4x4(&mesh.constantMap->boneTransforms[boneIndex],
					DirectX::XMLoadFloat4x4(&bone.offsetTransform) * 
					DirectX::XMLoadFloat4x4(&boneNode.globalTransform) * 
					DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&mesh.defaultGlobalTransform))
				);
			}
			const Animation::Keyframe::Node meshNode{ keyframe->nodes.at(mesh.nodeIndex) };
			mesh.constantMap->globalTransform = meshNode.globalTransform;
			mesh.constantMap->defaultGlobalTransform = mesh.defaultGlobalTransform;


			const Material& material{ materials.at(subset.materialUniqueId) };
			

			cmdList->SetDescriptorHeaps(1, material.cbvDescriptor->GetDescriptorHeap()->GetHeapDoublePointer());
			cmdList->SetGraphicsRootDescriptorTable(4, material.cbvDescriptor->GetGPUHandle());

			cmdList->SetDescriptorHeaps(1, mesh.constantHeap.GetAddressOf());
			cmdList->SetGraphicsRootDescriptorTable(3, mesh.constantHeap->GetGPUDescriptorHandleForHeapStart());

			cmdList->SetDescriptorHeaps(1, material.srvDescriptor.at(0)->GetDescriptorHeap()->GetHeapDoublePointer());
			cmdList->SetGraphicsRootDescriptorTable(2, material.srvDescriptor.at(0)->GetGPUHandle());
			cmdList->DrawIndexedInstanced(subset.indexCount, 1, subset.startIndexLocation, 0, 0);
		}
	}
}

void ArSkinnedMeshRenderer::Render()
{
	//static int clipIndex{};
	int frameIndex{};
	static float animationTick{};

	Animation& animation{ this->animationClips.at(clipIndex) };
	frameIndex = static_cast<int>(animationTick* animation.samplingRate);
	if(frameIndex > animation.sequence.size() - 1)
	{
		frameIndex = 0;

		animationTick = 0;
	}
	else
	{
		animationTick += Argent::Timer::ArTimer::Instance().DeltaTime();
	}
	Animation::Keyframe& keyframe{ animation.sequence.at(frameIndex) };

	//todo マテリアルの適用
	Render(Argent::Graphics::ArGraphics::Instance()->GetCommandList(), GetOwner()->GetTransform()->GetWorld(),
		/*material->color.color*/DirectX::XMFLOAT4(1, 1, 1, 1), &keyframe);
}

void ArSkinnedMeshRenderer::FetchMesh(FbxScene* fbxScene, std::vector<Mesh>& meshes)
{
	for(const SkinnedScene::Node& node : sceneView.nodes)
	{
		if(node.attribute != FbxNodeAttribute::EType::eMesh) continue;

		FbxNode* fbxNode{ fbxScene->FindNodeByName(node.name.c_str()) };
		FbxMesh* fbxMesh{ fbxNode->GetMesh() };

		Mesh& mesh{ meshes.emplace_back() };
		mesh.uniqueId = fbxMesh->GetNode()->GetUniqueID();
		mesh.name = fbxMesh->GetNode()->GetName();
		mesh.nodeIndex = sceneView.IndexOf(mesh.uniqueId);
		mesh.defaultGlobalTransform = Argent::Helper::FBX::ToFloat4x4(fbxMesh->GetNode()->EvaluateGlobalTransform());

		std::vector<boneInfluencesPerControlPoint> boneInfluences;
		FetchBoneInfluences(fbxMesh, boneInfluences);
		FetchSkeleton(fbxMesh, mesh.bindPose);

		std::vector<Mesh::Subset>& subsets{ mesh.subsets };
		const int MaterialCount{ fbxMesh->GetNode()->GetMaterialCount() };
		subsets.resize(MaterialCount > 0 ? MaterialCount : 1);
		for(int materialIndex = 0; materialIndex < MaterialCount; ++materialIndex)
		{
			const FbxSurfaceMaterial* fbxMaterial{ fbxMesh->GetNode()->GetMaterial(materialIndex) };
			subsets.at(materialIndex).materialName = fbxMaterial->GetName();
			subsets.at(materialIndex).materialUniqueId = fbxMaterial->GetUniqueID();
		}
		if(MaterialCount > 0)
		{
			const int  polygonCount{ fbxMesh->GetPolygonCount() };
			for(int polygonIndex = 0; polygonIndex < polygonCount; ++polygonIndex)
			{
				const int materialIndex{fbxMesh->GetElementMaterial()->GetIndexArray().GetAt(polygonIndex) };
				subsets.at(materialIndex).indexCount += 3;
			}
			uint32_t offset{};
			for(Mesh::Subset& subset : subsets)
			{
				subset.startIndexLocation = offset;
				offset += subset.indexCount;
				subset.indexCount = 0;
			}
		}

		const int polygonCount{ fbxMesh->GetPolygonCount() };
		mesh.vertices.resize(polygonCount * 3LL);
		mesh.indices.resize(polygonCount * 3LL);

		FbxStringList uvNames;
		fbxMesh->GetUVSetNames(uvNames);
		const FbxVector4* controlPoints{ fbxMesh->GetControlPoints() };
		for(int polygonIndex = 0; polygonIndex < polygonCount; ++polygonIndex)
		{
			const int materialIndex{ MaterialCount > 0 ? 
				fbxMesh->GetElementMaterial()->GetIndexArray().GetAt(polygonIndex) : 0 };
			Mesh::Subset& subset{ subsets.at(materialIndex) };
			const uint32_t offset{ subset.startIndexLocation + subset.indexCount };

			for(int positionInPolygon = 0; positionInPolygon < 3; ++positionInPolygon)
			{
				const int vertexIndex{ polygonIndex * 3 + positionInPolygon };

				Vertex vertex;
				const int polygonVertex{ fbxMesh->GetPolygonVertex(polygonIndex, positionInPolygon) };
				vertex.position.x = static_cast<float>(controlPoints[polygonVertex][0]);
				vertex.position.y = static_cast<float>(controlPoints[polygonVertex][1]);
				vertex.position.z = static_cast<float>(controlPoints[polygonVertex][2]);


				//bone
				const boneInfluencesPerControlPoint& influencesPerControlPoint{ boneInfluences.at(polygonVertex) };
				for(size_t influenceIndex = 0; influenceIndex < influencesPerControlPoint.size(); ++influenceIndex)
				{
					if(influenceIndex < MaxBoneInfluences)
					{
						vertex.boneWeights[influenceIndex] = influencesPerControlPoint.at(influenceIndex).boneWeight;
						vertex.boneIndices[influenceIndex] = influencesPerControlPoint.at(influenceIndex).boneIndex;
					}
				}

				if(fbxMesh->GetElementNormalCount() > 0)
				{
					FbxVector4 normal;
					fbxMesh->GetPolygonVertexNormal(polygonIndex, positionInPolygon, normal);
					vertex.normal.x = static_cast<float>(normal[0]);
					vertex.normal.y = static_cast<float>(normal[1]);
					vertex.normal.z = static_cast<float>(normal[2]);
				}
				if(fbxMesh->GetElementUVCount() > 0)
				{
					FbxVector2 uv;
					bool unmappedUv;
					fbxMesh->GetPolygonVertexUV(polygonIndex	, positionInPolygon, 
						uvNames[0], uv, unmappedUv);
					vertex.texcoord.x = static_cast<float>(uv[0]);
					vertex.texcoord.y = 1.0f - static_cast<float>(uv[1]);
				}

				mesh.vertices.at(vertexIndex) = std::move(vertex);
				mesh.indices.at(static_cast<size_t>(offset) + positionInPolygon) = vertexIndex;
				++subset.indexCount;
			}
		}
	}
}

void ArSkinnedMeshRenderer::FetchMaterial(FbxScene* fbxScene, std::unordered_map<uint64_t, Material>& materials)
{
	const size_t nodeCount{ sceneView.nodes.size() };
	for(size_t nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
	{
		const SkinnedScene::Node& node{ sceneView.nodes.at(nodeIndex) };
		const FbxNode* fbxNode{ fbxScene->FindNodeByName(node.name.c_str()) };

		const int materialCount{ fbxNode->GetMaterialCount() };
		if(materialCount > 0)
		{
			for(int materialIndex = 0; materialIndex < materialCount; ++materialIndex)
			{
				const FbxSurfaceMaterial* fbxMaterial{ fbxNode->GetMaterial(materialIndex) };

				Material material;
				material.name = fbxMaterial->GetName();
				material.uniqueId = fbxMaterial->GetUniqueID();

				FbxProperty fbxProp;
				fbxProp= fbxMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);
				
				if(fbxProp.IsValid())
				{
					const FbxDouble3 color{ fbxProp.Get<FbxDouble3>() };
					material.kd.x = static_cast<float>(color[0]);
					material.kd.y = static_cast<float>(color[1]);
					material.kd.z = static_cast<float>(color[2]);
					material.kd.w = 1.0f;

					const FbxFileTexture* fbxTexture{ fbxProp.GetSrcObject<FbxFileTexture>() };
					material.textureFilename[0] = fbxTexture ? fbxTexture->GetRelativeFileName() : "";
				}

				fbxProp= fbxMaterial->FindProperty(FbxSurfaceMaterial::sSpecular);
				if(fbxProp.IsValid())
				{
					const FbxDouble3 color{ fbxProp.Get<FbxDouble3>() };
					material.ks.x = static_cast<float>(color[0]);
					material.ks.y = static_cast<float>(color[1]);
					material.ks.z = static_cast<float>(color[2]);
					material.ks.w = 1.0f;

					const FbxFileTexture* fbxTexture{ fbxProp.GetSrcObject<FbxFileTexture>() };
					material.textureFilename[1] = fbxTexture ? fbxTexture->GetRelativeFileName() : "";
				}

				fbxProp= fbxMaterial->FindProperty(FbxSurfaceMaterial::sAmbient);
				
				if(fbxProp.IsValid())
				{
					const FbxDouble3 color{ fbxProp.Get<FbxDouble3>() };
					material.ka.x = static_cast<float>(color[0]);
					material.ka.y = static_cast<float>(color[1]);
					material.ka.z = static_cast<float>(color[2]);
					material.ka.w = 1.0f;

					const FbxFileTexture* fbxTexture{ fbxProp.GetSrcObject<FbxFileTexture>() };
					material.textureFilename[3] = fbxTexture ? fbxTexture->GetRelativeFileName() : "";
				}
				materials.emplace(material.uniqueId, std::move(material));
			}
		}
		else
		{
			Material material;
			material.name = "Dummy";
			//material.uniqueId = 0;
			material.kd.x = 
			material.kd.y = 
			material.kd.z = 
			material.kd.w = 1.0f;
			material.textureFilename[0] = "";

			materials.emplace(material.uniqueId, std::move(material));

		}

		if(materialCount == 0)
		{
			Material material;
			material.name = "Dummy";
			material.uniqueId = 0;
			material.kd.x = 
			material.kd.y = 
			material.kd.z = 
			material.kd.w = 1.0f;
			material.textureFilename[0] = "";
			materials.emplace(material.uniqueId, std::move(material));
		}

	}
}

void ArSkinnedMeshRenderer::FetchSkeleton(FbxMesh* fbxMesh, Skeleton& bindPose)
{
	const int deformerCount = fbxMesh->GetDeformerCount(FbxDeformer::eSkin);
	for(int deformerIndex = 0; deformerIndex < deformerCount; ++deformerIndex)
	{
		FbxSkin* skin = static_cast<FbxSkin*>(fbxMesh->GetDeformer(deformerIndex, FbxDeformer::eSkin));
		const int clusterCount = skin->GetClusterCount();
		bindPose.bones.resize(clusterCount);
		for(int clusterIndex = 0; clusterIndex < clusterCount; ++clusterIndex)
		{
			FbxCluster* cluster = skin->GetCluster(clusterIndex);

			Skeleton::Bone& bone{ bindPose.bones.at(clusterIndex) };
			bone.name = cluster->GetLink()->GetName();
			bone.uniqueId = cluster->GetLink()->GetUniqueID();
			bone.parentIndex = bindPose.indexOf(cluster->GetLink()->GetParent()->GetUniqueID());
			bone.nodeIndex = sceneView.IndexOf(bone.uniqueId);

			FbxAMatrix referenceGlobalInitPosition;
			cluster->GetTransformMatrix(referenceGlobalInitPosition);

			FbxAMatrix clusterGlobalInitPosition;
			cluster->GetTransformLinkMatrix(clusterGlobalInitPosition);
			bone.offsetTransform = Argent::Helper::FBX::ToFloat4x4(clusterGlobalInitPosition.Inverse() * referenceGlobalInitPosition);
		}
	}
}

void ArSkinnedMeshRenderer::FetchAnimation(FbxScene* fbxScene, std::vector<Animation>& animationClips, float samplingRate)
{
	FbxArray<FbxString*> animationStackNames;
	fbxScene->FillAnimStackNameArray(animationStackNames);
	const int animationStackCount{ animationStackNames.GetCount() };
	for(int animationStackIndex = 0; animationStackIndex < animationStackCount; ++animationStackIndex)
	{
		Animation& animationClip{ animationClips.emplace_back() };
		animationClip.name = animationStackNames[animationStackIndex]->Buffer();

		FbxAnimStack* animationStack{ fbxScene->FindMember<FbxAnimStack>(animationClip.name.c_str()) };
		fbxScene->SetCurrentAnimationStack(animationStack);

		const FbxTime::EMode timeMode{ fbxScene->GetGlobalSettings().GetTimeMode() };
		FbxTime oneSecond{};
		oneSecond.SetTime(0, 0, 1, 0, 0, timeMode);
		animationClip.samplingRate = samplingRate > 0 ? samplingRate : static_cast<float>(oneSecond.GetFrameRate(timeMode));
		const FbxTime samplingInterval{ static_cast<FbxLongLong>(static_cast<float>(oneSecond.Get()) / animationClip.samplingRate) };
		const FbxTakeInfo* takeInfo{ fbxScene->GetTakeInfo(animationClip.name.c_str()) };
		const FbxTime startTime{ takeInfo->mLocalTimeSpan.GetStart() };
		const FbxTime stopTime{ takeInfo->mLocalTimeSpan.GetStop() };
		for(FbxTime time = startTime; time < stopTime; time += samplingInterval)
		{
			Animation::Keyframe& keyframe{ animationClip.sequence.emplace_back() };

			const size_t nodeCount{ sceneView.nodes.size() };
			keyframe.nodes.resize(nodeCount);
			for(size_t nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
			{
				FbxNode* fbxNode{ fbxScene->FindNodeByName(sceneView.nodes.at(nodeIndex).name.c_str()) };
				if(fbxNode)
				{
					Animation::Keyframe::Node& node{ keyframe.nodes.at(nodeIndex) };
					node.globalTransform = Argent::Helper::FBX::ToFloat4x4(fbxNode->EvaluateGlobalTransform(time));

					const FbxAMatrix& localTransform{ fbxNode->EvaluateLocalTransform(time) };
					node.scaling = Argent::Helper::FBX::Tofloat3(localTransform.GetS());
					node.rotation = Argent::Helper::FBX::ToFloat4(localTransform.GetQ());
					node.translation = Argent::Helper::FBX::Tofloat3(localTransform.GetT());

				}
			}
		}
	}
	for(int animationStackIndex = 0; animationStackIndex < animationStackCount; ++animationStackIndex)
	{
		delete animationStackNames[animationStackIndex];
	}
}

void ArSkinnedMeshRenderer::UpdateAnimation(Animation::Keyframe& keyframe)
{
	size_t nodeCount{ keyframe.nodes.size() };
	for(size_t nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
	{
		Animation::Keyframe::Node& node{ keyframe.nodes.at(nodeIndex) };
		DirectX::XMMATRIX S{ DirectX::XMMatrixScaling(node.scaling.x, node.scaling.y, node.scaling.z) };
		DirectX::XMMATRIX R { DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&node.rotation)) };
		DirectX::XMMATRIX T { DirectX::XMMatrixTranslation(node.translation.x, node.translation.y, node.translation.z) };

		int64_t parentIndex{ sceneView.nodes.at(nodeIndex).parentIndex };
		DirectX::XMMATRIX P{ parentIndex < 0 ? DirectX::XMMatrixIdentity() :
			DirectX::XMLoadFloat4x4(&keyframe.nodes.at(parentIndex).globalTransform) };

		DirectX::XMStoreFloat4x4(&node.globalTransform, S * R * T * P);
	}
}

void ArSkinnedMeshRenderer::Update()
{
#if 0
	static int clipIndex{};
	int frameIndex{};
	static float animationTick{};

	Animation& animation{ mesh->animationClips.at(clipIndex) };
	frameIndex = static_cast<int>(animationTick* animation.samplingRate);
	if(frameIndex > animation.sequence.size() - 1)
	{
		++clipIndex;
		if(clipIndex > mesh->animationClips.size() - 1)
		{
			clipIndex = 0;
		}
		frameIndex = 0;

		animationTick = 0;
	}
	else
	{
		animationTick += HighResolutionTimer::Instance().DeltaTime();
	}
	Animation::Keyframe& keyframe{ animation.sequence.at(frameIndex) };

#if 1
	DirectX::XMStoreFloat4((&keyframe.nodes.at(0).rotation),
		DirectX::XMQuaternionRotationAxis(DirectX::XMVectorSet(1, 0, 0, 0), 1.5f));
	keyframe.nodes.at(0).translation.x = 0;
	mesh->UpdateAnimation(keyframe);
#endif

	mesh->Render(ArGraphics::Instance()->GetCommandList(),
		meshTransform.GetWorld(ArGraphics::Instance()->CoordinateSystemTransforms[coordinateSystem], 
			ArGraphics::Instance()->scaleFactor),
		 meshColor.color, &keyframe);

#endif

}

#ifdef _DEBUG
void ArSkinnedMeshRenderer::DrawDebug()
{
	if(ImGui::TreeNode("Skinned Mesh Renderer"))
	{
		ImGui::SliderInt("Animation Clip", &clipIndex, 0, animationClips.size() - 1);
		ImGui::Text(animationClips.at(clipIndex).name.c_str());
		ImGui::TreePop();
	}
	ArRenderer::DrawDebug();
}
#endif

void ArSkinnedMeshRenderer::CreateComObject(ID3D12Device* device, const char* filename)
{
	HRESULT hr{ S_OK };
	for(Mesh& mesh : meshes)
	{
		D3D12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		D3D12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(Vertex) * mesh.vertices.size());
		hr = device->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &resDesc, D3D12_RESOURCE_STATE_GENERIC_READ, 
			nullptr, IID_PPV_ARGS(mesh.vertexBuffer.ReleaseAndGetAddressOf()));
		assert(SUCCEEDED(hr));


		Vertex* vMap{};
		hr = mesh.vertexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&vMap));
		std::copy(mesh.vertices.begin(), mesh.vertices.end(), vMap);
		mesh.vertexBuffer->Unmap(0, nullptr);

		mesh.vertexView.SizeInBytes = static_cast<UINT>(mesh.vertices.size() * sizeof(Vertex));
		mesh.vertexView.StrideInBytes = sizeof(Vertex);
		mesh.vertexView.BufferLocation = mesh.vertexBuffer->GetGPUVirtualAddress();


		resDesc.Width = sizeof(uint32_t) * mesh.indices.size();
		hr = device->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &resDesc, D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr, IID_PPV_ARGS(mesh.indexBuffer.ReleaseAndGetAddressOf()));
		assert(SUCCEEDED(hr));

		uint32_t* iMap{};
		hr = mesh.indexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&iMap));
		std::copy(mesh.indices.begin(), mesh.indices.end(), iMap);
		mesh.indexBuffer->Unmap(0, nullptr);

		mesh.indexView.Format = DXGI_FORMAT_R32_UINT;
		mesh.indexView.SizeInBytes = static_cast<UINT>(sizeof(uint32_t) * mesh.indices.size());
		mesh.indexView.BufferLocation = mesh.indexBuffer->GetGPUVirtualAddress();

		D3D12_DESCRIPTOR_HEAP_DESC heapDesc;
		heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		heapDesc.NodeMask = 0;
		heapDesc.NumDescriptors = 1;
		heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		hr = device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(mesh.constantHeap.ReleaseAndGetAddressOf()));
		assert(SUCCEEDED(hr));

		resDesc = CD3DX12_RESOURCE_DESC::Buffer(Argent::Helper::Math::CalcAlignmentSize(sizeof(Mesh::Constant)));

		hr = device->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &resDesc, D3D12_RESOURCE_STATE_GENERIC_READ, 
			nullptr, IID_PPV_ARGS(mesh.constantBuffer.ReleaseAndGetAddressOf()));
		assert(SUCCEEDED(hr));

		hr = mesh.constantBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mesh.constantMap));
		assert(SUCCEEDED(hr));
		mesh.constantMap->defaultGlobalTransform = mesh.defaultGlobalTransform;

		D3D12_CONSTANT_BUFFER_VIEW_DESC cbv{};
		cbv.SizeInBytes = static_cast<UINT>(mesh.constantBuffer->GetDesc().Width);
		cbv.BufferLocation = mesh.constantBuffer->GetGPUVirtualAddress();
		device->CreateConstantBufferView(&cbv, mesh.constantHeap->GetCPUDescriptorHandleForHeapStart());
	}


	D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapDesc.NodeMask = 0;
	heapDesc.NumDescriptors = static_cast<UINT>(materials.size());
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	for(std::unordered_map<uint64_t, Material>::iterator it = materials.begin(); 
		it != materials.end(); ++it)
	{
		it->second.srvDescriptor.resize(Material::MaxTextureNum + 1);
		for(auto& descriptor : it->second.srvDescriptor)
		{
			descriptor = Argent::Graphics::ArGraphics::Instance()->GetHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)->PopDescriptor();
		}

		if(it->second.textureFilename[0].size() > 0)
		{
			std::filesystem::path path(filename);
			path.replace_filename(it->second.textureFilename[0]);
			Argent::Graphics::ArGraphics::Instance()->LoadTexture(path.c_str(), it->second.texture[0].ReleaseAndGetAddressOf());

		}
		else
		{
			Argent::Graphics::ArGraphics::Instance()->CreateWhiteTexture(it->second.texture[0].ReleaseAndGetAddressOf());
		}

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		srvDesc.Format = it->second.texture[0]->GetDesc().Format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Texture2D.MipLevels = 1;
		device->CreateShaderResourceView(it->second.texture[0].Get(), &srvDesc, it->second.srvDescriptor.at(0)->GetCPUHandle());

		D3D12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		D3D12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Buffer(Argent::Helper::Math::CalcAlignmentSize(sizeof(Material::Constant)));
		hr = device->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &resDesc, 
			D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(it->second.constantBuffer.ReleaseAndGetAddressOf()));
		assert(SUCCEEDED(hr));

		ArSkinnedMeshRenderer::Material::Constant* map{};
		hr = it->second.constantBuffer->Map(0, nullptr, reinterpret_cast<void**>(&map));
		assert(SUCCEEDED(hr));

		map->ka = it->second.ka;
		map->ks = it->second.ks;
		map->kd = it->second.kd;

		it->second.constantBuffer->Unmap(0, nullptr);

		it->second.cbvDescriptor = Argent::Graphics::ArGraphics::Instance()->GetHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)->PopDescriptor();

		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
		cbvDesc.SizeInBytes = static_cast<UINT>(it->second.constantBuffer->GetDesc().Width);
		cbvDesc.BufferLocation = it->second.constantBuffer->GetGPUVirtualAddress();
		device->CreateConstantBufferView(&cbvDesc, it->second.cbvDescriptor->GetCPUHandle());
	}


	//コンスタント
	{
		D3D12_HEAP_PROPERTIES constHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		CD3DX12_RESOURCE_DESC constResDesc = CD3DX12_RESOURCE_DESC::Buffer(Argent::Helper::Math::CalcAlignmentSize(sizeof(Constants)));

		hr = device->CreateCommittedResource(&constHeapProp, D3D12_HEAP_FLAG_NONE, &constResDesc, D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr, IID_PPV_ARGS(constantBuffer.ReleaseAndGetAddressOf()));
		assert(SUCCEEDED(hr));

		
		hr = constantBuffer->Map(0, nullptr, reinterpret_cast<void**>(&constantMap));
		assert(SUCCEEDED(hr));
		DirectX::XMStoreFloat4x4(&constantMap->world, DirectX::XMMatrixIdentity());
	}

	constantDescriptor = Argent::Graphics::ArGraphics::Instance()->GetHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)->PopDescriptor();

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
	cbvDesc.SizeInBytes = static_cast<UINT>(constantBuffer->GetDesc().Width);
	cbvDesc.BufferLocation = constantBuffer->GetGPUVirtualAddress();

	device->CreateConstantBufferView(&cbvDesc, constantDescriptor->GetCPUHandle());
}

void FetchBoneInfluences(const FbxMesh* fbxMesh, 
	std::vector<boneInfluencesPerControlPoint>& boneInfluences)
{
	const int controlPointsCount{ fbxMesh->GetControlPointsCount() };
	boneInfluences.resize(controlPointsCount);

	const int skinCount{ fbxMesh->GetDeformerCount(FbxDeformer::eSkin) };
	for(int skinIndex = 0; skinIndex < skinCount; ++skinIndex)
	{
		const FbxSkin* fbxSkin{static_cast<FbxSkin*>(fbxMesh->GetDeformer(skinIndex, FbxDeformer::eSkin)) };
		const int clusterCount{ fbxSkin->GetClusterCount() };
		for(int clusterIndex = 0; clusterIndex < clusterCount; ++clusterIndex)
		{
			const FbxCluster* fbxCluster{ fbxSkin->GetCluster(clusterIndex) };
			const int controlPointIndicesCount{ fbxCluster->GetControlPointIndicesCount() };
			for(int controlPointIndicesIndex = 0; controlPointIndicesIndex < controlPointIndicesCount;
				++controlPointIndicesIndex)
			{
				const int controlPointIndex{ fbxCluster->GetControlPointIndices()[controlPointIndicesIndex] };
				const double controlPointWeight{ fbxCluster->GetControlPointWeights()[controlPointIndicesIndex] };
				BoneInfluence& boneInfluence{ boneInfluences.at(controlPointIndex).emplace_back() };
				boneInfluence.boneIndex = static_cast<uint32_t>(clusterIndex);
				boneInfluence.boneWeight = static_cast<float>(controlPointWeight);
			}
		}
	}
}
