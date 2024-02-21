#include "globals.hlsli"
struct VSIn
{
    float4 Pos : POSITION;
    float4 Color : COLOR;
    float2 UV : TEXCOORD;
};

struct VSOut
{
    float4 Pos : SV_Position;
    float4 WorldPos : COLOR2;
    float4 Color : COLOR;
    float2 UV : TEXCOORD;
};

//cbuffer time
//int 0

float4 main(VSOut In) : SV_Target
{
    float4 color = (float) 1.0f;
    
    //color.a = 1.0f;
    
    //float depth = In.WorldPos.z / 300.0f;
    //color.rgb = (depth);
    //color.r = 0.5f;
    color = albedoTexture.Sample(anisotropicSampler, In.UV);
    color.a = 1.0f;
    
    return color;
}
