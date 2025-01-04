cbuffer LightCBuffer
{
    float3 lightPosition;
};

static const float3 materialColor = { 0.7f, 0.7f, 0.9f };
static const float3 ambient = { 0.05f, 0.05f, 0.05f };
static const float3 diffuseColor = { 1.0f, 1.0f, 1.0f };
static const float diffuseIntensity = 1.0f;
static const float attConst = 1.0f;
static const float attLin = 0.045f;
static const float attQuad = 0.0075f;

Texture2D txColorMat : register(t0);
SamplerState txSampler : register(s0);

float4 main(float3 worldPos : POSITION, float2 tex : TEXTCORD, float3 n : NORMAL) : SV_Target
{
	// fragment to light vector data
    const float3 vToL = lightPosition - worldPos;
    const float distToL = length(vToL);
    const float3 dirToL = vToL / distToL;
	// diffuse attenuation
    const float att = 1.0f / (attConst + attLin * distToL + attQuad * (distToL * distToL));
	// diffuse intensity
    const float3 diffuse = diffuseColor * diffuseIntensity * att * max(0.0f, dot(dirToL, n));
	// final color
    float4 outColor = float4(saturate(diffuse + ambient), 1.0f);
    return outColor;
}
