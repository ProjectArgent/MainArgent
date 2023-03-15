#include "NoneBoneMeshRenderer.h"
#include <functional>
#include <d3dx12.h>
#include <filesystem>
#include "../Core/ArTimer.h"
#include "../Other/Misc.h"
#include "../GameObject/GameObject.h"
#include "../Other/ArHelper.h"
#include "../Resource/ArResourceManager.h"

namespace Argent::Component::Renderer
{
	void NoneBoneMeshRenderer::Material::CreateTexture(const char* filePath, TextureType type)
	{
		textures[static_cast<int>(type)] = std::reinterpret_pointer_cast<Argent::Texture::ArTexture>(Argent::Resource::ArResourceManager::Instance().LoadTexture(filePath));
	}

	NoneBoneMeshRenderer::NoneBoneMeshRenderer(ID3D12Device* device, const char* filename,
		bool triangulate) :
		ArRenderer("name")
	{
		FbxManager* manager{ FbxManager::Create() };
		FbxScene* fbxScene{ FbxScene::Create(manager, "") };

		FbxImporter* importer{ FbxImporter::Create(manager, "") };
		bool importState{ FALSE };
		importState = importer->Initialize(filename);
		_ASSERT_EXPR_A(importState, importer->GetStatus().GetErrorString());

		importState = importer->Import(fbxScene);
		_ASSERT_EXPR_A(importState, importer->GetStatus().GetErrorString());

		FbxGeometryConverter converter(manager);
		if (triangulate)
		{
			converter.Triangulate(fbxScene, true, false);
			converter.RemoveBadPolygonsFromMeshes(fbxScene);
		}


		NoneBone::SkinnedScene sceneView{};

		std::function<void(FbxNode* fbxNode)> traverse
		{ [&](FbxNode* fbxNode)
			{
				NoneBone::SkinnedScene::Node& node{ sceneView.nodes.emplace_back() };
				node.attribute = fbxNode->GetNodeAttribute() ?
					fbxNode->GetNodeAttribute()->GetAttributeType() :
					FbxNodeAttribute::EType::eUnknown;
				node.name = fbxNode->GetName();
				node.id = fbxNode->GetUniqueID();
				node.parentIndex = sceneView.IndexOf(fbxNode->GetParent() ?
					fbxNode->GetParent()->GetUniqueID() : 0);
				for (int childIndex = 0; childIndex < fbxNode->GetChildCount(); ++childIndex)
				{
					traverse(fbxNode->GetChild(childIndex));
				}
			}
		};


		traverse(fbxScene->GetRootNode());

		FetchMesh(fbxScene, meshes, sceneView);
		FetchMaterial(fbxScene, materials, sceneView, filename);

		manager->Destroy();

		CreateComObject(device, filename);


		//ルートパラメータとパイプラインステート
		{
			D3D12_ROOT_SIGNATURE_DESC rootSigDesc{};
			D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc{};
			D3D12_DESCRIPTOR_RANGE range[5]{};

			range[0] = Helper::Dx12::DescriptorRange::Generate(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_CBV);
			range[1] = Helper::Dx12::DescriptorRange::Generate(1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_CBV);
			range[2] = Helper::Dx12::DescriptorRange::Generate(2, 1, D3D12_DESCRIPTOR_RANGE_TYPE_CBV);
			range[3] = Helper::Dx12::DescriptorRange::Generate(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);
			range[4] = Helper::Dx12::DescriptorRange::Generate(1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);

			D3D12_ROOT_PARAMETER rootParam[5]{};
			rootParam[static_cast<int>(RootParameterIndex::cbScene)] = Helper::Dx12::RootParameter::Generate(1, &range[0], D3D12_SHADER_VISIBILITY_ALL);
			rootParam[static_cast<int>(RootParameterIndex::cbObject)] = Helper::Dx12::RootParameter::Generate(1, &range[1], D3D12_SHADER_VISIBILITY_ALL);
			rootParam[static_cast<int>(RootParameterIndex::cbMaterial)] = Helper::Dx12::RootParameter::Generate(1, &range[2], D3D12_SHADER_VISIBILITY_ALL);
			rootParam[static_cast<int>(RootParameterIndex::txAlbedo)] = Helper::Dx12::RootParameter::Generate(1, &range[3], D3D12_SHADER_VISIBILITY_PIXEL);
			rootParam[static_cast<int>(RootParameterIndex::txNormal)] = Helper::Dx12::RootParameter::Generate(1, &range[4], D3D12_SHADER_VISIBILITY_PIXEL);

			D3D12_STATIC_SAMPLER_DESC samplerDesc = Helper::Dx12::Sampler::GenerateSamplerDesc(Helper::Dx12::Sampler::FilterMode::fPoint, Helper::Dx12::Sampler::WrapMode::wRepeat);


			rootSigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
			rootSigDesc.NumParameters = 5;
			rootSigDesc.pParameters = rootParam;
			rootSigDesc.NumStaticSamplers = 1;
			rootSigDesc.pStaticSamplers = &samplerDesc;


			//パイプライン

			D3D12_BLEND_DESC blendDesc{};
			blendDesc.AlphaToCoverageEnable = FALSE;
			blendDesc.IndependentBlendEnable = FALSE;
			blendDesc.RenderTarget[0] = Helper::Dx12::Blend::GenerateRenderTargetBlendDesc(Helper::Dx12::Blend::BlendMode::bAlpha);

			D3D12_INPUT_ELEMENT_DESC inputElementDesc[]
			{
				Helper::Dx12::InputElement::GenerateInputLayoutDesc("POSITION", DXGI_FORMAT_R32G32B32_FLOAT),
				Helper::Dx12::InputElement::GenerateInputLayoutDesc("NORMAL", DXGI_FORMAT_R32G32B32_FLOAT),
				Helper::Dx12::InputElement::GenerateInputLayoutDesc("TEXCOORD", DXGI_FORMAT_R32G32_FLOAT),
			};

			DXGI_SAMPLE_DESC sampleDesc{};
			sampleDesc.Count = 1;
			sampleDesc.Quality = 0;


			D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
			depthStencilDesc.DepthEnable = TRUE;
			depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
			depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;


			pipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

			pipelineStateDesc.RasterizerState = Helper::Dx12::Rasterizer::Generate();
			pipelineStateDesc.BlendState = blendDesc;
			pipelineStateDesc.InputLayout.NumElements = _countof(inputElementDesc);
			pipelineStateDesc.InputLayout.pInputElementDescs = inputElementDesc;
			pipelineStateDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
			pipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			pipelineStateDesc.NumRenderTargets = 1;
			pipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
			pipelineStateDesc.SampleDesc = sampleDesc;
			pipelineStateDesc.DepthStencilState = depthStencilDesc;
			pipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;


			renderingPipeline = std::make_shared<Argent::Graphics::RenderingPipeline::ArBaseRenderingPipeline>(
				"./Resources/Shader/StaticMeshVertex.cso",
				"./Resources/Shader/StaticMeshPixel.cso",
				&rootSigDesc,
				&pipelineStateDesc
				);
		}
	}

	void NoneBoneMeshRenderer::Render(ID3D12GraphicsCommandList* cmdList,
		const DirectX::XMFLOAT4X4& world,
		const DirectX::XMFLOAT4& color) const
	{
		ArRenderer::Render(cmdList);

		Argent::Graphics::ArGraphics::Instance()->SetSceneConstant(static_cast<UINT>(RootParameterIndex::cbScene));


		cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		Constants constant{};
		constant.world = world;
		constant.color = color;
		demoConstBuffer->UpdateConstantBuffer(constant);
		for (const Mesh& mesh : meshes)
		{
			demoConstBuffer->SetOnCommandList(cmdList, static_cast<UINT>(RootParameterIndex::cbObject));

			cmdList->IASetVertexBuffers(0, 1, &mesh.vertexView);
			cmdList->IASetIndexBuffer(&mesh.indexView);

			for (const Mesh::Subset& subset : mesh.subsets)
			{
				const Material& material{ materials.at(subset.materialUniqueId) };
				material.constantBuffer->UpdateConstantBuffer(material.constant);
				material.SetOnCommand(cmdList);
				cmdList->DrawIndexedInstanced(subset.indexCount, 1, subset.startIndexLocation, 0, 0);
			}
		}
	}

	void NoneBoneMeshRenderer::Render() const 
	{
		Render(Argent::Graphics::ArGraphics::Instance()->GetCommandList(), GetOwner()->GetTransform()->GetWorld(),
			/*material->color.color*/DirectX::XMFLOAT4(1, 1, 1, 1));
	}

	void NoneBoneMeshRenderer::Update()
	{
	}

#ifdef _DEBUG
	void NoneBoneMeshRenderer::DrawDebug()
	{
		if (ImGui::TreeNode("Skinned Mesh Renderer"))
		{
			if (ImGui::TreeNode("Material"))
			{
				for (auto& m : materials)
				{
					m.second.DrawDebug();
				}
				ImGui::TreePop();
			}

			ArRenderer::DrawDebug();
			ImGui::TreePop();
		}
	}
#endif

	void FetchMesh(FbxScene* fbxScene, std::vector<NoneBoneMeshRenderer::Mesh>& meshes, const NoneBone::SkinnedScene& sceneView)
	{
		for (const NoneBone::SkinnedScene::Node& node : sceneView.nodes)
		{
			if (node.attribute != FbxNodeAttribute::EType::eMesh) continue;

			FbxNode* fbxNode{ fbxScene->FindNodeByName(node.name.c_str()) };
			FbxMesh* fbxMesh{ fbxNode->GetMesh() };

			NoneBoneMeshRenderer::Mesh& mesh{ meshes.emplace_back() };
			mesh.uniqueId = fbxMesh->GetNode()->GetUniqueID();
			mesh.name = fbxMesh->GetNode()->GetName();
			mesh.nodeIndex = sceneView.IndexOf(mesh.uniqueId);

			std::vector<NoneBoneMeshRenderer::Mesh::Subset>& subsets{ mesh.subsets };
			const int MaterialCount{ fbxMesh->GetNode()->GetMaterialCount() };
			subsets.resize(MaterialCount > 0 ? MaterialCount : 1);
			for (int materialIndex = 0; materialIndex < MaterialCount; ++materialIndex)
			{
				const FbxSurfaceMaterial* fbxMaterial{ fbxMesh->GetNode()->GetMaterial(materialIndex) };
				subsets.at(materialIndex).materialUniqueId = fbxMaterial->GetUniqueID();
			}

			if (MaterialCount > 0)
			{
				const int  polygonCount{ fbxMesh->GetPolygonCount() };
				for (int polygonIndex = 0; polygonIndex < polygonCount; ++polygonIndex)
				{
					const int materialIndex{ fbxMesh->GetElementMaterial()->GetIndexArray().GetAt(polygonIndex) };
					subsets.at(materialIndex).indexCount += 3;
				}
				uint32_t offset{};
				for (NoneBoneMeshRenderer::Mesh::Subset& subset : subsets)
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
			for (int polygonIndex = 0; polygonIndex < polygonCount; ++polygonIndex)
			{
				const int materialIndex{ MaterialCount > 0 ?
					fbxMesh->GetElementMaterial()->GetIndexArray().GetAt(polygonIndex) : 0 };
				NoneBoneMeshRenderer::Mesh::Subset& subset{ subsets.at(materialIndex) };
				const uint32_t offset{ subset.startIndexLocation + subset.indexCount };

				for (int positionInPolygon = 0; positionInPolygon < 3; ++positionInPolygon)
				{
					const int vertexIndex{ polygonIndex * 3 + positionInPolygon };

					NoneBoneMeshRenderer::Vertex vertex;
					const int polygonVertex{ fbxMesh->GetPolygonVertex(polygonIndex, positionInPolygon) };
					vertex.position.x = static_cast<float>(controlPoints[polygonVertex][0]);
					vertex.position.y = static_cast<float>(controlPoints[polygonVertex][1]);
					vertex.position.z = static_cast<float>(controlPoints[polygonVertex][2]);


					//bone
					if (fbxMesh->GetElementNormalCount() > 0)
					{
						FbxVector4 normal;
						fbxMesh->GetPolygonVertexNormal(polygonIndex, positionInPolygon, normal);
						vertex.normal.x = static_cast<float>(normal[0]);
						vertex.normal.y = static_cast<float>(normal[1]);
						vertex.normal.z = static_cast<float>(normal[2]);
					}
					if (fbxMesh->GetElementUVCount() > 0)
					{
						FbxVector2 uv;
						bool unmappedUv;
						fbxMesh->GetPolygonVertexUV(polygonIndex, positionInPolygon,
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

	void FetchMaterial(FbxScene* fbxScene, std::unordered_map<uint64_t, NoneBoneMeshRenderer::Material>& materials, const NoneBone::SkinnedScene& sceneView, const char* fbxFilePath)
	{
		const size_t nodeCount{ sceneView.nodes.size() };
		for (size_t nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
		{
			const NoneBone::SkinnedScene::Node& node{ sceneView.nodes.at(nodeIndex) };
			const FbxNode* fbxNode{ fbxScene->FindNodeByName(node.name.c_str()) };

			const int materialCount{ fbxNode->GetMaterialCount() };
			if (materialCount > 0)
			{
				for (int materialIndex = 0; materialIndex < materialCount; ++materialIndex)
				{
					const FbxSurfaceMaterial* fbxMaterial{ fbxNode->GetMaterial(materialIndex) };

					NoneBoneMeshRenderer::Material material;
					material.name = fbxMaterial->GetName();
					UINT64 materialUniqueId = fbxMaterial->GetUniqueID();

					FbxProperty fbxProp;
					fbxProp = fbxMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);

					if (fbxProp.IsValid())
					{
						const FbxDouble3 color{ fbxProp.Get<FbxDouble3>() };


						material.constant.kd.x = static_cast<float>(color[0]);
						material.constant.kd.y = static_cast<float>(color[1]);
						material.constant.kd.z = static_cast<float>(color[2]);
						material.constant.kd.w = 1.0f;

						const FbxFileTexture* fbxTexture{ fbxProp.GetSrcObject<FbxFileTexture>() };

						std::string tmpFilePath = fbxTexture ? fbxTexture->GetRelativeFileName() : "";

						std::filesystem::path path(fbxFilePath);
						path.replace_filename(tmpFilePath);

						const std::string replacedFilePath = Helper::String::GetStringFromWideString(path.c_str());
						material.CreateTexture(replacedFilePath.c_str(), NoneBoneMeshRenderer::Material::TextureType::Albedo);
					}

					fbxProp = fbxMaterial->FindProperty(FbxSurfaceMaterial::sSpecular);
					if (fbxProp.IsValid())
					{
						const FbxDouble3 color{ fbxProp.Get<FbxDouble3>() };
						material.constant.ks.x = static_cast<float>(color[0]);
						material.constant.ks.y = static_cast<float>(color[1]);
						material.constant.ks.z = static_cast<float>(color[2]);
						material.constant.ks.w = 1.0f;

						const FbxFileTexture* fbxTexture{ fbxProp.GetSrcObject<FbxFileTexture>() };
						std::string tmpFilePath = fbxTexture ? fbxTexture->GetRelativeFileName() : "";
					}

					fbxProp = fbxMaterial->FindProperty(FbxSurfaceMaterial::sAmbient);

					if (fbxProp.IsValid())
					{
						const FbxDouble3 color{ fbxProp.Get<FbxDouble3>() };
						material.constant.ka.x = static_cast<float>(color[0]);
						material.constant.ka.y = static_cast<float>(color[1]);
						material.constant.ka.z = static_cast<float>(color[2]);
						material.constant.ka.w = 1.0f;

						const FbxFileTexture* fbxTexture{ fbxProp.GetSrcObject<FbxFileTexture>() };
						std::string tmpFilePath = fbxTexture ? fbxTexture->GetRelativeFileName() : "";
					}

					fbxProp = fbxMaterial->FindProperty(FbxSurfaceMaterial::sNormalMap);
					if (fbxProp.IsValid())
					{
						const FbxFileTexture* fbxTexture{ fbxProp.GetSrcObject<FbxFileTexture>() };
						std::string tmpFilePath = fbxTexture ? fbxTexture->GetRelativeFileName() : "";

						std::filesystem::path path(fbxFilePath);
						path.replace_filename(tmpFilePath);

						const std::string replacedFilePath = Helper::String::GetStringFromWideString(path.c_str());
						//material.CreateTexture(replacedFilePath.c_str(), NoneBoneMeshRenderer::Material::TextureType::Normal);
					}

					fbxProp = fbxMaterial->FindProperty(FbxSurfaceMaterial::sBump);
					if(fbxProp.IsValid())
					{
						const FbxFileTexture* fbxTexture{ fbxProp.GetSrcObject<FbxFileTexture>() };
						std::string tmpFilePath = fbxTexture ? fbxTexture->GetRelativeFileName() : "";

						std::filesystem::path path(fbxFilePath);
						path.replace_filename(tmpFilePath);

						const std::string replacedFilePath = Helper::String::GetStringFromWideString(path.c_str());
						material.CreateTexture(replacedFilePath.c_str(), NoneBoneMeshRenderer::Material::TextureType::Normal);
					}
					materials.emplace(materialUniqueId, std::move(material));
				}
			}

			if(materials.size() <= 0)
			{
				NoneBoneMeshRenderer::Material material;
				material.name = "Dummy";
				material.CreateTexture("", NoneBoneMeshRenderer::Material::TextureType::Albedo);
				material.CreateTexture("", NoneBoneMeshRenderer::Material::TextureType::Normal);
				materials.emplace(0, std::move(material));
			}
		}
	}

	void NoneBoneMeshRenderer::CreateComObject(ID3D12Device* device, const char* filename)
	{
		HRESULT hr{ S_OK };
		for (Mesh& mesh : meshes)
		{
			D3D12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
			D3D12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(Vertex) * mesh.vertices.size());
			hr = device->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &resDesc, D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr, IID_PPV_ARGS(mesh.vertexBuffer.ReleaseAndGetAddressOf()));
			_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));


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
			_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

			uint32_t* iMap{};
			hr = mesh.indexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&iMap));
			std::copy(mesh.indices.begin(), mesh.indices.end(), iMap);
			mesh.indexBuffer->Unmap(0, nullptr);

			mesh.indexView.Format = DXGI_FORMAT_R32_UINT;
			mesh.indexView.SizeInBytes = static_cast<UINT>(sizeof(uint32_t) * mesh.indices.size());
			mesh.indexView.BufferLocation = mesh.indexBuffer->GetGPUVirtualAddress();
		}


		D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
		heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		heapDesc.NodeMask = 0;
		heapDesc.NumDescriptors = static_cast<UINT>(materials.size());
		heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

		for (std::unordered_map<uint64_t, Material>::iterator it = materials.begin();
			it != materials.end(); ++it)
		{
			it->second.constantBuffer =
				std::make_unique<Argent::Dx12::ArConstantBuffer<Material::Constant>>(
					device,
					Graphics::ArGraphics::Instance()->GetHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)->PopDescriptor(),
					&it->second.constant);
		}
		demoConstBuffer = std::make_unique<Argent::Dx12::ArConstantBuffer<Constants>>(device, Graphics::ArGraphics::Instance()->GetHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)->PopDescriptor());
	}
}