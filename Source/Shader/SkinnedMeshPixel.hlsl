#include "SkinnedMesh.hlsli"
#include "../DemoShaderMacro.h"

SamplerState smpPoint : register(s0);
//SamplerState smpAniso : register(s1);

Texture2D tex : register(ALBEDO);
Texture2D normalTex : register(NORMAL);

float3 CalcLambertDiffuse(float3 normal, float3 lightVector, float3 lightColor, float3 kd)
{
	float d = max(0, dot(normal, -lightVector));

	return kd * lightColor * d;
}

float3 CalcPhongSpecular(float3 normal, float3 lightVector, float3 lightColor, 
	float3 eyeVector, float shininess, float3 ks)
{
	float3 R = reflect(lightVector, normal);

	float d = max(dot(-eyeVector, R), 0);
	d = pow(d, shininess);

	return d * lightColor * ks;
}

float4 main(VS_OUT pin) : SV_TARGET
{
	float4 color = tex.Sample(smpPoint, pin.texcoord);

	float3 normal = normalTex.Sample(smpPoint, pin.texcoord).rgb;

	float3 T = float3(1.0001, 0, 0);
	float3x3 CM = {normalize(T), normalize(cross(pin.worldNormal.xyz, T)), normalize(pin.worldNormal.xyz) };


	//float3 N = normalize(pin.worldNormal.xyz);
	float3 N = normalTex.Sample(smpPoint, pin.texcoord).rgb;
	N = normalize(mul(normal * 2.0f - 1.0f, CM));

	float3 L = normalize(float3(-lightPosition.xyz));
	float3 E = normalize(float3(cameraPosition.xyz - pin.worldPosition.xyz));


	float3 diffuse = CalcLambertDiffuse(N, L, lightColor, kd);
	float3 specular = CalcPhongSpecular(N, L, lightColor, E, shininess, ks);


	return float4(color.rgb * diffuse.rgb + specular.rgb, 1);
}