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

VSOut main(VSIn In)
{
    VSOut ouput = (VSOut) 0.0f;
    
    //float4 worldPosition = mul(float4(In.Pos, 1.0f), world);
    //float4 viewPosition = mul(worldPosition, view);
    //float4 ProjPosition = mul(viewPosition, projection);
    
    float4 viewPos = mul(float4(In.Pos, 0.0f), view);
    float4 projPos = mul(float4(viewPos.xyz, 1.0f), projection);
    projPos.z = projPos.w;
    
    ouput.Pos = projPos;
    ouput.UV = In.UV;
    ouput.CUBEUV = normalize(In.Pos);
    
    return ouput;
}
