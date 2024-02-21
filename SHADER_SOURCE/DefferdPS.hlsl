#include "globals.hlsli"

struct VSIn
{
    float4 Position : POSITION;
    float2 UV : TEXCOORD;
    float3 Tangent : TANGENT;
    float3 Normal : NORMAL;
    float3 BiNormal : BINORMAL;
};

struct VSOut
{
    float4 Position : SV_Position;
    float2 UV : TEXCOORD;
    
    float3 ViewPos : POSITION;
    
    float3 ViewNormal : NORMAL;
    float3 ViewTanget : TANGENT;
    float3 ViewBiNormal : BINORMAL;
};

struct PSOut
{
    float4 Position : SV_Target0;
    float4 Normal : SV_Target1;
    float4 Albedo : SV_Target2;
    float4 Specular : SV_Target3;
};


PSOut main(VSOut In) : SV_Target
{
    PSOut OutColor;
    
    float4 objColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
    
    if (usedAlbedo == 1)
    {
        objColor = albedoTexture.Sample(pointSampler, In.UV);
    }
    
    float3 vNormal = In.ViewNormal;
    
    if (usedNormal == 1)
    {
    // 물체의 표면에 적용될 탄젠트 공간 기준 방향벡터를 가져온다.
        vNormal = normalTexture.Sample(anisotropicSampler, In.UV);
        
    // 0~1값을 -1~1의 값으로 변환
        vNormal = (vNormal * 2.0f) - 1.0f;
        
        float3x3 matTBN =
        {
            In.ViewTanget,
            In.ViewBiNormal,
            In.ViewNormal,
        };

        vNormal = normalize(mul(vNormal, matTBN));
    }
    
    float4 SpecularCoeff = float4(1.0f, 1.0f, 1.0f, 1.0f);
    if (usedSpecular == 1)
    {
        SpecularCoeff = specularTexture.Sample(anisotropicSampler, In.UV);
    }
    
    //float4 EmissiveCoeff = float4(1.0f, 1.0f, 1.0f, 1.0f);
    //if (usedEmissive == 1)
    //{
    //    EmissiveCoeff = emissiveTexture.Sample(anisotropicSampler, In.UV);
    //}
    
    // Creeper Flash 관련 값 
    if (int_0 == 1)
    {
        // flashFrequency 값이 높을수록 빠르게 색상이 변경됨 
        float flashFrequency = 20.0f;
        float repeatedFlash = abs(sin(float_1 * flashFrequency) * 0.5f + 0.5f);

        float4 flashColor = lerp(float4(1.0f, 0.0f, 0.0f, 1.0f),
                             float4(1.0f, 1.0f, 0.0f, 1.0f),
                             repeatedFlash);
    
        objColor = lerp(objColor, flashColor, repeatedFlash);
    }
    
    // Hit Effect 관련 값
    if (int_1 == 1)
    {
        float mixRatio = float_3 / float_2;
        float4 flashColor = lerp(float4(1.0f, 0.0f, 0.0f, 1.0f),
                                     float4(1.0f, 1.0f, 0.0f, 1.0f),
                                     mixRatio);
        
        objColor = lerp(objColor, flashColor, mixRatio);
    }
    
    OutColor.Position = float4(In.ViewPos, 1.0f);
    OutColor.Normal = float4(vNormal, 1.0f);
    OutColor.Albedo = objColor;
    OutColor.Specular = SpecularCoeff;
    //OutColor.Emissive = EmissiveCoeff;
    
    return OutColor;
}
