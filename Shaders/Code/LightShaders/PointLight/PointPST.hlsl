cbuffer LightCBuffer
{
    float3 lightPosition;
    float3 ambient;
    float3 diffuseColor;
    float diffuseIntensity;
    float attConst;
    float attLin;
    float attQuad;
};

cbuffer LightUpdates
{
    float3 materialColor;
    float specularIntensity;
    float specularPower;
}

Texture2D txColorMat : register(t0);
SamplerState txSampler : register(s0);

float4 main(float3 worldPos : POSITION, float3 n : NORMAL) : SV_Target
{
    // Fragment to light vector data
    float3 vToL = lightPosition - worldPos;
    float distToL = length(vToL);
    float3 dirToL = vToL / distToL;

    // Diffuse attenuation
    float att = 1.0f / (attConst + attLin * distToL + attQuad * (distToL * distToL));

    // Diffuse intensity
    float3 diffuse = diffuseColor * diffuseIntensity * att * max(0.0f, dot(dirToL, n));

    // Specular calculation
    float3 w = n * dot(vToL, n);
    float3 r = w * 2.0f - vToL;
    float3 specular = att * (diffuseColor * diffuseIntensity) * specularIntensity *
                      pow(max(0.0f, dot(normalize(-r), normalize(worldPos))), specularPower);
    // Combine lighting and texture
    float3 lighting = saturate(diffuse + ambient + specular);
    // Output final color

    return float4(lighting, 1.0f);
}
