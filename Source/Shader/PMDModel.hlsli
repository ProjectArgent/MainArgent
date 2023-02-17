struct VsOut
{
	float4 svPos : SV_POSITION;
	float4 pos : POSITION;
	float4 normal : NORMAL0;
	float4 vNormal : NORMAL1;
	float2 texcoord : TEXCOORD;
	float ray : VECTOR;
};


cbuffer SceneConstant : register(b0)
{
	float4x4 view;
	float4x4 projection;
	float3 lightDirection;
	float3 cameraPosition;
}

cbuffer cbuff0 : register(b1)
{
	float4x4 world;
	float4x4 bone[256];
}

cbuffer Material : register(b2)
{
	float4 diffuse;
	float4 specular;
	float3 ambient;
}
