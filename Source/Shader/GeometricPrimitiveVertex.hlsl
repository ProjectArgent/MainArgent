#include "GeometricPrimitive.hlsli"

VsOut main( float4 pos : POSITION, float4 normal : NORMAL, float2 texcoord : TEXCOORD)
{
	VsOut vout;
	vout.position = mul(pos, mul(world, viewProjection));
	normal.w = 0;
	vout.normal = mul(normal, world);
	vout.texcoord = texcoord;

	vout.color = materialColor;
	vout.lightDirection = normalize(pos.xyz - lightPosition);
	return vout;
}