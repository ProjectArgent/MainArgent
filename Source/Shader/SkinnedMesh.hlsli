#include "SceneConstant.hlsli"

struct VSIN
{
	float4 position : POSITION;
	float3 normal : NORMAL;
	float2 texcoord : TEXCOORD;
	float4 boneWeights : WEIGHTS;
	uint4 boneIndices : BONES;
};

struct VS_OUT
{
	float4 position : SV_POSITION;
	float4 worldPosition : POSITION;
	float4 worldNormal : NORMAL;
	float2 texcoord : TEXCOORD;
	float4 color : COLOR;
};

static const int MaxBones = 256;
cbuffer ObjectConstant : register(b1)
{
	row_major float4x4 world;
	float4 materialColor;
}
cbuffer MeshConstant : register(b2)
{
	row_major float4x4 defaultGlobalTransform;
	row_major float4x4 globalTransform;
	row_major float4x4 boneTransforms[MaxBones];
}

cbuffer MaterialConstant : register(b3)

{
	float4 ka;
	float4 kd;
	float4 ks;
	float shininess;
}