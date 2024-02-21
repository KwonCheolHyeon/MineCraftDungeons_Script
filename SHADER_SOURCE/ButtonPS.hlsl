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

#define ItemTexSlot positionTarget
#define FrameTexSlot normalTexture
#define OutlineTexSlot albedoTexture
#define DefaultTex albedoTexture

float4 main(VSOut In) : SV_Target
{
    float4 outputPixel = (float) 0.0f;
    
    if (int_0 == 1)
    {
        outputPixel = DefaultTex.Sample(pointSampler, In.UV);
    }
    else if (int_0 == 3)
    {     
        float4 frameImgPixelColor = FrameTexSlot.Sample(pointSampler, In.UV);
        if (0.f != frameImgPixelColor.a)
            outputPixel += frameImgPixelColor;
        
        float4 ItemPixelColor = ItemTexSlot.Sample(pointSampler, In.UV);
        if (0.f != ItemPixelColor.a)
            outputPixel += ItemPixelColor;
        
        float4 frameOutlineColor = OutlineTexSlot.Sample(pointSampler, In.UV);
        if (0.f != frameOutlineColor.a)
            outputPixel += frameOutlineColor;
    }
    else
    {
        outputPixel = (1.f, 0.f, 1.f, 1.f);
    }
    
    outputPixel = saturate(outputPixel);
    
    return outputPixel;
}
