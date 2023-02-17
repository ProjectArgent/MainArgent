#include "PMDModel.hlsli"
Texture2D tex : register(t0);
Texture2D sph : register(t1);
Texture2D spa : register(t2);
Texture2D toon : register(t3);
SamplerState smp : register(s0);
SamplerState smpToon : register(s1);


float4 main(VsOut pin) : SV_TARGET
{
	float diffuseB = saturate(dot(-lightDirection, pin.normal));
	float4 toonDif = toon.Sample(smpToon, float2(0, 1.0 - diffuseB));

	float3 refLight = normalize(reflect(-lightDirection, pin.normal.xyz));
	float specularB = pow(saturate(dot(refLight, -pin.ray)), specular.a);

	float2 sphereMapUV = pin.vNormal.xy;
	sphereMapUV = (sphereMapUV + float2(1, -1)) * float2(0.5, -0.5);

	float4 texColor = tex.Sample(smp, pin.texcoord);

	float brightness = dot(-lightDirection, pin.normal);
	float2 normalUV = (pin.normal.xy + float2(1, -1)) * float2(0.5, 0.5);

#if 1
	return max(saturate(
		toonDif 
		* diffuse
		* texColor
		* sph.Sample(smp, sphereMapUV))
		+ saturate(spa.Sample(smp, sphereMapUV) * texColor
		+ float4(specularB * specular.rgb, 1))
		, float4(texColor * ambient, 1));

#endif

	//return max(diffuseB
	//	* diffuse
	//	* texColor
	//	* sph.Sample(smp, sphereMapUV)
	//	+ spa.Sample(smp, sphereMapUV) * texColor
	//	+ float4(specularB * specular.rgb, 1)
	//	, float4(texColor * ambient, 1));



	//return float4(brightness, brightness, brightness, 1) * diffuse * tex.Sample(smp, pin.texcoord);
	//return float4(brightness, brightness, brightness, 1)
	//	* diffuse
	//	* color
	//	* sph.Sample(smp, sphereMapUV)
	//	+ spa.Sample(smp, sphereMapUV)
	//	+ float4(color * ambient, 1);
}