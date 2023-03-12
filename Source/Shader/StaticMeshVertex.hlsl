#include "StaticMesh.hlsli"

VS_OUT main(VSIN vin)
{
	VS_OUT vout;
	row_major float4x4 viewProjection = mul(view, projection);
	vout.position = mul(vin.position, mul(world, viewProjection));
	vout.worldPosition = mul(vin.position, world);

	vout.worldNormal = normalize(mul(vin.normal, world));
	vout.texcoord = vin.texcoord;
	vout.color = materialColor;
	return vout;
}