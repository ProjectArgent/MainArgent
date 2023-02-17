#include "Pera.hlsli"

VSOUT main( float4 pos : POSITION, float2 texcoord : TEXCOORD ) 
{
	VSOUT vout;
	vout.svPos = pos;
	vout.texcoord = texcoord;
	return vout;
}