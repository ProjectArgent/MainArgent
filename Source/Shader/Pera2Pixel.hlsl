#include "Pera.hlsli"

float4 main(VSOUT pin) : SV_TARGET
{
	float4 color = tex.Sample(smp, pin.texcoord);

	//return color;

	float2 mTex = effectTex.Sample(smp, pin.texcoord).xy;
	mTex = mTex * 2.0f - 1.0f;


	return tex.Sample(smp, pin.texcoord + mTex * 0.1f);

	float w, h, leves;
	tex.GetDimensions(0, w, h, leves);

	float dy = 1.0f / h;
	float4 ret = float4(0, 0, 0, 0);

	ret += bkweights[0] * color;

	for(unsigned int i = 0; i < 8; ++i)
	{
		ret += bkweights[i >> 2][i % 4] * tex.Sample(smp, pin.texcoord + float2(0, dy * i));
		ret += bkweights[i >> 2][i % 4] * tex.Sample(smp, pin.texcoord + float2(0, -dy * i));
	}

	return float4(ret.rgb, color.a);

	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}