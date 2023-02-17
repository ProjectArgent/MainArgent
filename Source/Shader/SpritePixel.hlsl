#include "Sprite.hlsli"

Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);


float4 main(VsOut pIn) : SV_TARGET
{
	return float4(tex.Sample(smp, pIn.texcoord)) * pIn.color;
	return float4(pIn.texcoord, 1, 1);
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}