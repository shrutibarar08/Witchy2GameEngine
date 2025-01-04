cbuffer TransformBuffer
{
    matrix world;
    matrix projection;
}


float4 main( float3 pos : POSITION ) : SV_POSITION
{
    return mul(float4(pos, 1.0f), projection);
}
