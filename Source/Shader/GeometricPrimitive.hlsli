struct VsOut
{
	float4 position : SV_POSITION;
	float4 normal : NORMAL;
	float2 texcoord : TEXCOORD;
	float4 color : COLOR;
	float3 lightDirection : LIGHT_DIRECTION;
};


cbuffer SceneConstant : register(b0)
{
	row_major float4x4 viewProjection;
	float4 lightColor;
	float4 lightPosition;
	float4 cameraPosition;
}

cbuffer cbuff0 : register(b1)
{
	row_major float4x4 world;
	float4 materialColor;
}
