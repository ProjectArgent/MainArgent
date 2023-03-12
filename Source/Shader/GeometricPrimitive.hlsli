#include "SceneConstant.hlsli"

struct VsOut
{
	float4 position : SV_POSITION;
	float4 normal : NORMAL;
	float2 texcoord : TEXCOORD;
	float4 color : COLOR;
	float3 lightDirection : LIGHT_DIRECTION;
};



cbuffer cbuff0 : register(b1)
{
	row_major float4x4 world;
	float4 materialColor;
}
