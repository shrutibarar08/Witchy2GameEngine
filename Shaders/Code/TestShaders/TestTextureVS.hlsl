cbuffer Transform: register(b0)
{
    matrix transform;
}

struct VSIn
{
    float3 Position : POSITION;
    float2 Tex : TEXTCORD;
};

struct VSOut
{
    float4 Position : SV_POSITION;
    float2 Tex : TEXCOORD0;
};


VSOut main( VSIn input)
{
    VSOut vso;
    vso.Position = mul(float4(input.Position, 1.0f), transform);
    vso.Tex = input.Tex;
	return vso;
}