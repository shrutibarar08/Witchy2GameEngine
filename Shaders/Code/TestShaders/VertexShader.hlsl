
cbuffer ConstantBuffer: register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;
};

struct VS_OUTPUT
{
    float4 Color : COLOR;
    float4 Position : SV_POSITION;
};

VS_OUTPUT main( float4 pos : POSITION, float4 color: COLOR)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.Position = mul(pos, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);
    output.Color = color;
    return output;
}
