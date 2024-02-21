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

VSOut main(VSIn In)
{
    VSOut Out = (VSOut) 0.f;
    
    //float4 wvp = mul(world, view);
    //wvp = mul(wvp, projection);
    
    float4 worldPosition = mul(In.Pos, world);
    //worldPosition.w = 1.0f;
    float4 viewPosition = mul(worldPosition, view);
    //viewPosition.w = 1.0f;
    float4 ProjPosition = mul(viewPosition, projection);
    
    Out.WorldPos = ProjPosition;
    //Out.WorldPos.w = 1.0f;
    
    Out.Pos = ProjPosition;
    Out.Color = In.Color;
    Out.UV = In.UV;
    
    return Out;
}
