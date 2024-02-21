#include "globals.hlsli"

// texture 11
struct VSIn
{
    float3 Pos : POSITION;
    float2 UV : TEXCOORD;
};

struct VSOut
{
    float4 Pos : SV_Position;
    float2 UV : TEXCOORD;
    float3 CUBEUV : TEXCOORD1;
};

float4 main(VSOut In) : SV_Target
{
    float4 output = (float4) 0.0f;

    //sphere
    //output = skyTexture.Sample(anisotropicSampler, In.UV);
    
    //Cube
    output = skyTexture.Sample(anisotropicSampler, In.CUBEUV);
    
    return output;
}
