cbuffer CBuffer: register(b0)
{
    matrix world;
    matrix projection;
};

struct VSIn
{
    float3 pos : POSITION;
    float2 tex : TEXTCORD;
    float3 normal : NORMAL;
};

struct VSOut
{
    float3 worldPosition : POSITION;
    float2 tex : TEXTCORD;
    float3 normal : NORMAL;
    float4 pos : SV_Position;
};

VSOut main(VSIn input)
{
    VSOut vs = (VSOut) 0;
    vs.worldPosition = (float3) mul(float4(input.pos, 1.0f), world);
    vs.pos = mul(float4(input.pos, 1.0f), projection);
    vs.normal = mul(input.normal, (float3x3) world);
    vs.tex = input.tex;
	return vs;
}
