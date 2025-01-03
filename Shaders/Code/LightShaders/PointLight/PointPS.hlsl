cbuffer LightCBuffer
{
    float3 lightPosition;
};

static const float3 materialColor = { 0.7f, 0.7f, 0.9f };
static const float3 ambient = { 0.15f, 0.15f, 0.15f };
static const float3 diffuseColor = { 1.0f, 1.0f, 1.0f };
static const float diffuseIntensity = 1.0f;
static const float attConst = 1.0f;
static const float attLin = 0.25f;
static const float attQuad = 0.75f;


float4 main(float3 worldPos : POSITION, float3 n : NORMAL) : SV_Target
{
	// fragment to light vector data
    const float3 vToL = lightPosition - worldPos;
    const float distToL = length(vToL);
    const float3 dirToL = vToL / distToL;
	// diffuse attenuation
    const float att = 1.0 / attConst + attLin * distToL + attQuad * (distToL * distToL);
	// diffuse intensity
    const float3 diffuse = diffuseColor * diffuseIntensity * att * max(0.0f, dot(dirToL, n));
	// final color
    return float4(saturate(diffuse + ambient), 1.0f);
}
