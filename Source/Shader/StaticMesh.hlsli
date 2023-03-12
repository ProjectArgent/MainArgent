#include "SceneConstant.hlsli"

struct VSIN
{
	float4 position : POSITION;
	float3 normal : NORMAL;
	float2 texcoord : TEXCOORD;
};

struct VS_OUT
{
	float4 position : SV_POSITION;
	float4 worldPosition : POSITION;
	float4 worldNormal : NORMAL;
	float2 texcoord : TEXCOORD;
	float4 color : COLOR;
};

cbuffer ObjectConstant : register(b1)
{
	row_major float4x4 world;
	float4 materialColor;
}

cbuffer MaterialConstant : register(b2)
{
	float4 ka;
	float4 kd;
	float4 ks;
	float shininess;
}