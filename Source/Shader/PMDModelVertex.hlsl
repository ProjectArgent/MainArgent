#include "PMDModel.hlsli"

VsOut main( 
	float4 pos : POSITION,
	float4 normal : NORMAL,
	float2 texcoord : TEXCOORD,
	min16uint2 boneno : BONE_NO,
	min16uint weight : WEIGHT
	)
{
	float w = weight / 100.0f;

	float4x4 bm = bone[boneno[0]] * w + bone[boneno[1]] * (1 - w);

	VsOut vout;
	pos = mul(bm, pos);
	vout.svPos = mul(mul(mul(projection, view), world), pos);
	vout.pos = mul(world, pos);
	normal.w = 0;
	vout.normal = mul(world, normal);
	vout.vNormal = mul(view, vout.normal);
	vout.texcoord = texcoord;
	vout.ray = normalize(pos.xyz - cameraPosition);
	return vout;
}