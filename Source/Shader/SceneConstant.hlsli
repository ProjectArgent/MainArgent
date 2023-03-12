cbuffer SceneConstant : register(b0)
{
	row_major float4x4 view;
	row_major float4x4 projection;
	float4 lightColor;
	float4 lightPosition;
	float4 cameraPosition;
}
