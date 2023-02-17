#include "SpritePrimitive.hlsli"

VsOut main( float4 pos : POSITION, float4 color : COLOR )
{
	VsOut vOut;
	vOut.pos = pos;
	vOut.color = color;
	return vOut;
}