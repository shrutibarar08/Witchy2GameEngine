
cbuffer ConstantBuffer : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;
};

float4 main( float4 pos : POSITION ) : SV_POSITION
{
    pos = mul(pos, World);
    pos = mul(pos, View);
    pos = mul(pos, Projection);
	return pos;
}
