#pragma once

#include <d3d12.h>
#include <wrl.h>
#include <DirectXMath.h>
#include <vector>
#include <string>
#include <fbxsdk.h>
#include <memory>

#include <unordered_map>
#include "ArRenderer.h"
#include "../Graphic/Dx12/ArConstantBuffer.h"
#include "../Resource/ArTexture.h"



//todo　ボーンあるいはアニメーションを持っていない場合はレンダリングできないため
//クラスを分けたい　
//ボーンを持っている== skinnedMesh 持っていない== mesh　という感じ
//なのでfbxローダを外側で作ってボーンの数を取得　そこからどっちにするか分ける
//リソースマネージャーにでもおいときゃいい感じになるでしょ
//コンストラクタの引数を変更するのはありじゃない
//ボーンとメッシュ情報にするのは全然あり　アニメーションも置いといていいかも
//unityみたいにしたいがアニメーションを分離して別の場所に置くと管理が大変そう

namespace Argent::Resource::FBX
{
	struct SkinnedScene
	{
		struct Node
		{
			uint64_t id{};
			std::string name;
			FbxNodeAttribute::EType attribute{ FbxNodeAttribute::EType::eUnknown };
			int64_t parentIndex{ -1 };
		};

		std::vector<Node> nodes;

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

	struct BoneInfluence
	{
		uint32_t boneIndex;
		float boneWeight;
	};
	using boneInfluencesPerControlPoint = std::vector<BoneInfluence>;

	struct Skeleton
	{
		struct Bone
		{
			uint64_t uniqueId{};
			std::string name;
			int64_t parentIndex{ -1 };
			int64_t nodeIndex{};

			DirectX::XMFLOAT4X4 offsetTransform
			{
				1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0, 1, 0,
				0, 0, 0, 1
			};
			bool isOrphan() const { return parentIndex < 0; }
		};
		std::vector<Bone> bones;
		int64_t indexOf(uint64_t uniqueId) const
		{
			int64_t index{};
			for(const Bone& bone : bones)
			{
				if(bone.uniqueId == uniqueId) return index;
				++index;
			}
			return -1;
		}
	};

	struct Animation
	{
		std::string name;
		float samplingRate{};

		struct Keyframe
		{
			struct Node
			{
				DirectX::XMFLOAT4X4 globalTransform
				{
					1, 0, 0, 0,
					0, 1, 0, 0,
					0, 0, 1, 0,
					0, 0, 0, 1
				};

				DirectX::XMFLOAT3 scaling{ 1, 1, 1 };
				DirectX::XMFLOAT4 rotation{ 0, 0, 0, 1 };
				DirectX::XMFLOAT3 translation{ 0, 0, 0 };
			};
			std::vector<Node> nodes;
		};
		std::vector<Keyframe> sequence;
	};

	class ArSkinnedMeshRenderer:
		public Argent::Component::Renderer::ArRenderer
	{
	public:
		enum class RootParameterIndex
		{
			cbScene,
			cbObject,
			cbMesh,
			cbMaterial,
			txAlbedo,
			txNormal,
		};
		static constexpr int MaxBoneInfluences{ 4 };
		
		struct Vertex			
		{
			DirectX::XMFLOAT3 position;
			DirectX::XMFLOAT3 normal;
			DirectX::XMFLOAT2 texcoord;
			float boneWeights[MaxBoneInfluences]{1, 0, 0, 0};
			uint32_t boneIndices[MaxBoneInfluences];
		};

		static const int MaxBones{ 256 };

		struct Constants
		{
			DirectX::XMFLOAT4X4 world;
			DirectX::XMFLOAT4 color;
		};

		struct Mesh
		{
			struct Subset
			{
				uint64_t materialUniqueId{};

				uint32_t startIndexLocation{};
				uint32_t indexCount{};
			};
			std::vector<Subset> subsets;

			struct Constant
			{
				DirectX::XMFLOAT4X4 defaultGlobalTransform;
				DirectX::XMFLOAT4X4 globalTransform;
				DirectX::XMFLOAT4X4 boneTransforms[MaxBones]
				{
					{1, 0, 0, 0,
						0, 1, 0, 0,
						0, 0, 1, 0,
						0, 0, 0, 1
					}
				};
			};

			uint64_t uniqueId{};
			std::string name;
			int64_t nodeIndex{};
			DirectX::XMFLOAT4X4 defaultGlobalTransform
			{
				1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0, 1, 0,
				0, 0, 0, 1,
			};
		
			std::vector<Vertex> vertices;
			std::vector<uint32_t> indices;

		//private:
			Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer;
			Microsoft::WRL::ComPtr<ID3D12Resource> indexBuffer;

			std::unique_ptr<Argent::Dx12::ArConstantBuffer<Constant>> constantBuffer;
			D3D12_VERTEX_BUFFER_VIEW vertexView{};
			D3D12_INDEX_BUFFER_VIEW indexView{};

			friend class FbxResource;
			Skeleton bindPose;
		};

		struct Material
		{
			enum class TextureType
			{
				Albedo,
				Normal,
			};
			struct Constant
			{
				DirectX::XMFLOAT4 ka{ 0.2f, 0.2f, 0.2f, 1.0f };
				DirectX::XMFLOAT4 kd{ 0.2f, 0.2f, 0.2f, 1.0f };
				DirectX::XMFLOAT4 ks{ 0.2f, 0.2f, 0.2f, 1.0f };
				float shininess = 128;
			};
			std::string name;

			static constexpr int NumTextures = 2;
			std::shared_ptr<Argent::Texture::ArTexture> textures[NumTextures]; 
			std::unique_ptr<Argent::Dx12::ArConstantBuffer<Constant>> constantBuffer;
			Constant constant{};
			void CreateTexture(const char* filePath, TextureType type);
			

			void SetOnCommand(ID3D12GraphicsCommandList* cmdList) const
			{
				//todo なんとか白
				constantBuffer->SetOnCommandList(cmdList, static_cast<UINT>(RootParameterIndex::cbMaterial));
				textures[static_cast<int>(TextureType::Albedo)]->Render(cmdList, static_cast<UINT>(RootParameterIndex::txAlbedo));
				textures[static_cast<int>(TextureType::Normal)]->Render(cmdList, static_cast<UINT>(RootParameterIndex::txNormal));
			}

	#ifdef _DEBUG
			void DrawDebug()
			{
				if(ImGui::TreeNode(name.c_str()))
				{
					ImGui::DragFloat3("Ka", &constant.ka.x, 0.001f, 0, 1.0f);
					ImGui::DragFloat3("Kd", &constant.kd.x, 0.001f, 0, 1.0f);
					ImGui::DragFloat3("Ks", &constant.ks.x, 0.001f, 0, 1.0f);
					ImGui::DragFloat("Shininess", &constant.shininess, 1.0f, 0, FLT_MAX);
					ImGui::TreePop();
				}
			}
	#endif

		};


	public:
		
		ArSkinnedMeshRenderer(ID3D12Device* device, const char* filename, float samplingRate = 0, bool triangulate = false);
		virtual ~ArSkinnedMeshRenderer() = default;

		void Render(ID3D12GraphicsCommandList* cmdList,
			const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& color,
			const Animation::Keyframe* keyframe) const;

		void Render() const override;

		void Update() override;

	#ifdef _DEBUG
		void DrawDebug() override;
	#endif

		void CreateComObject(ID3D12Device* device, const char* filename);

	private:
		std::unique_ptr<Argent::Dx12::ArConstantBuffer<Constants>> demoConstBuffer;

	protected:
		SkinnedScene sceneView;
		std::vector<Mesh> meshes;
		std::unordered_map<uint64_t, Material> materials;

		int clipIndex{};
		float frameIndex{};
	public:
		std::vector<Animation> animationClips;
	};

	namespace SkinnedMesh
	{
		
		void FetchMesh(FbxScene* fbxScene, std::vector<ArSkinnedMeshRenderer::Mesh>& meshes, const SkinnedScene& sceneView);
		void FetchMaterial(FbxScene* fbxScene, std::unordered_map<uint64_t, ArSkinnedMeshRenderer::Material>& materials, const SkinnedScene& sceneView, const char* fbxFilePath);
		void FetchSkeleton(FbxMesh* fbxMesh, Skeleton& bindPose, const SkinnedScene& sceneView);
		void FetchAnimation(FbxScene* fbxScene, std::vector<Animation>& animationClips, 
			float samplingRate, const SkinnedScene& sceneView);
		void UpdateAnimation(Animation::Keyframe& keyframe, const SkinnedScene& sceneView);


		void FetchBoneInfluences(const FbxMesh* fbxMesh, std::vector<boneInfluencesPerControlPoint>& boneInfluences);
	}
	
}