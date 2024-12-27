struct PS_Input
{
    float4 pos : SV_Position;
    float4 color : Color;
};

float4 main(PS_Input input) : SV_Target
{
    return input.color;
}
