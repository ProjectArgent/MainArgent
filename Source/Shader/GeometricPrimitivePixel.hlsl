#include "GeometricPrimitive.hlsli"

SamplerState smp : register(s0);

Texture2D albedoTex : register(t0);

float4 main(VsOut pin) : SV_TARGET
{
	float4 color = albedoTex.Sample(smp, pin.texcoord);

	float brightness = dot(-pin.lightDirection, pin.normal);

	float r = (pin.color.r + lightColor.r) / 2 * brightness;
	float g = (pin.color.g + lightColor.g) / 2 * brightness;
	float b = (pin.color.b + lightColor.b) / 2 * brightness;

	return float4(float3(pin.color.xyz * lightColor.xyz * color.arg) * brightness, 1);
}