#pragma once

#include <d3d12.h>
#include <wrl.h>
#include <DirectXMath.h>
#include <vector>
#include <string>
#include <fbxsdk.h>
#include <memory>

#include <unordered_map>
#include "../GameObject/Component/Renderer/ArRenderer.h"
#include "../Graphic/Dx12/ArDescriptor.h"



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
	static constexpr int MaxBoneInfluences{ 4 };
	struct Vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 texcoord;
		float boneWeights[MaxBoneInfluences]{ 1, 0, 0, 0 };
		uint32_t boneIndices[MaxBoneInfluences]{};
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
			std::string materialName;

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

	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D12Resource> indexBuffer;
		Microsoft::WRL::ComPtr<ID3D12Resource> constantBuffer;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> constantHeap;

		Constant* constantMap;
		D3D12_VERTEX_BUFFER_VIEW vertexView{};
		D3D12_INDEX_BUFFER_VIEW indexView{};

		friend class ArSkinnedMeshRenderer;

		Skeleton bindPose;
	};

	struct Material
	{
		struct Constant
		{
			DirectX::XMFLOAT4 ka;
			DirectX::XMFLOAT4 kd;
			DirectX::XMFLOAT4 ks;
		};
		uint64_t uniqueId{};
		std::string name;

		DirectX::XMFLOAT4 ka{ 0.2f, 0.2f, 0.2f, 1.0f };
		DirectX::XMFLOAT4 kd{ 0.2f, 0.2f, 0.2f, 1.0f };
		DirectX::XMFLOAT4 ks{ 0.2f, 0.2f, 0.2f, 1.0f };

		static constexpr UINT MaxTextureNum = 4;
		std::string textureFilename[MaxTextureNum];
		std::vector<Argent::Descriptor::ArDescriptor*> srvDescriptor;
		Microsoft::WRL::ComPtr<ID3D12Resource> texture[MaxTextureNum];
		Argent::Descriptor::ArDescriptor* cbvDescriptor;

		Microsoft::WRL::ComPtr<ID3D12Resource> constantBuffer;
		//Material::Constant* constantMap{};
	};


public:
	ArSkinnedMeshRenderer(ID3D12Device* device, const char* filename, float samplingRate = 0, bool triangulate = false);
	virtual ~ArSkinnedMeshRenderer() = default;

	void Render(ID3D12GraphicsCommandList* cmdList,
		const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& color,
		const Animation::Keyframe* keyframe);

	void Render();

	void FetchMesh(FbxScene* fbxScene, std::vector<Mesh>& meshes);
	void FetchMaterial(FbxScene* fbxScene, std::unordered_map<uint64_t, Material>& materials);
	void FetchSkeleton(FbxMesh* fbxMesh, Skeleton& bindPose);
	void FetchAnimation(FbxScene* fbxScene, std::vector<Animation>& animationClips, 
		float samplingRate);
	void UpdateAnimation(Animation::Keyframe& keyframe);
	void Update();

#ifdef _DEBUG
	void DrawDebug() override;
#endif

	void CreateComObject(ID3D12Device* device, const char* filename);

private:
	Argent::Descriptor::ArDescriptor* constantDescriptor;
	Microsoft::WRL::ComPtr<ID3D12Resource> constantBuffer;
	D3D12_CONSTANT_BUFFER_VIEW_DESC constantView;
	Constants* constantMap{};


protected:
	SkinnedScene sceneView;
	std::vector<Mesh> meshes;
	std::unordered_map<uint64_t, Material> materials;

	int clipIndex{};
public:
	std::vector<Animation> animationClips;
};

void FetchBoneInfluences(const FbxMesh* fbxMesh, std::vector<boneInfluencesPerControlPoint>& boneInfluences);