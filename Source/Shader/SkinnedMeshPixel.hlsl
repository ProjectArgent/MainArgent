#include "SkinnedMesh.hlsli"
SamplerState smp : register(s0);
Texture2D tex : register(t0);

float4 main(VS_OUT pin) : SV_TARGET
{
	float4 color = tex.Sample(smp, pin.texcoord);
	//return float4(color.rgb, 1);


	float3 N = normalize(pin.worldNormal.xyz);
	float3 L = pin.lightDirection;
	float3 diffuse = color.rgb * max(0, dot(N, L));
	return float4(diffuse, color.a) * pin.color;
}