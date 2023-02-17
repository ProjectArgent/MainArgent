Texture2D<float4> tex : register(t0);
Texture2D<float4> effectTex : register(t1);
SamplerState smp : register(s0);



struct VSOUT
{
	float4 svPos : SV_POSITION;
	float2 texcoord : TEXCOORD;
};



cbuffer PostEffect : register(b0)
{
	float4 bkweights[2];
};