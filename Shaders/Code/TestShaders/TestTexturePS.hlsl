struct VSOut
{
    float4 Position : SV_POSITION;
    float2 Tex : TEXCOORD0;
};

Texture2D txColorMat : register(t0);
SamplerState txSampler : register(s0);


float4 main(VSOut input) : SV_TARGET
{
	return txColorMat.Sample(txSampler, input.Tex);
}
