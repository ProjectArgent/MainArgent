#include "Sprite.hlsli"

VsOut main( float4 pos : POSITION, float2 texcoord : TEXCOORD, float4 color : COLOR )
{
	VsOut vOut;
	//vOut.position = mul(mat, pos);
	vOut.position = pos;
	vOut.texcoord = texcoord;
	vOut.color = color;
	return vOut;
}