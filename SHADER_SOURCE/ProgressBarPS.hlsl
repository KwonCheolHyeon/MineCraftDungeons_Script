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
    float4 Color : COLOR;
    float2 UV : TEXCOORD;
};

float4 main(VSOut In) : SV_Target
{
    float4 color = (float) 0.0f;
    
    if (int_0 == 0) // heart color
    {
        if (In.UV.y >= float_0)
        {
            float4 tempColor = albedoTexture.Sample(anisotropicSampler, In.UV);
        
            if (0.f != tempColor.a)
                color += tempColor;
        }
         //0~1 사이로 제한하고 반환
        color = saturate(color);
    }
    else if (int_0 == 1 || int_0 == 3)// 1 rolling icon
    {
        if (In.UV.y >= float_1)
        {
            float4 tempColor = albedoTexture.Sample(anisotropicSampler, In.UV);
        
            if (0.f != tempColor.a)
                color += tempColor;
        }
         //0~1 사이로 제한하고 반환
        color = saturate(color);
    }
    else if (int_0 == 2)//2 heart frame alpha
    {
        float4 tempColor = albedoTexture.Sample(anisotropicSampler, In.UV);
       
        
        if (tempColor.a >= 0.1f)
        {
            tempColor.a = float_2;
        }
        color = tempColor;
    }
    
    
    return color;
}
