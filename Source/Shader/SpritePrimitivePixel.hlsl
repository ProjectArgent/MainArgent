#include "SpritePrimitive.hlsli"

float4 main(VsOut pIn) : SV_TARGET
{
	return pIn.color;
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}