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
    
    //float3 vNormal = In.ViewNormal;
    
    //if (usedNormal == 1)
    //{
    //    // 물체의 표면에 적용될 탄젠트 공간 기준 방향벡터를 가져옵니다.
    //    vNormal = normalTexture.Sample(anisotropicSampler, In.UV);
        
    //    // 0~1값을 -1~1의 값으로 변환합니다.
    //    vNormal = (vNormal * 2.0f) - 1.0f;
        
    //    float3x3 matTBN =
    //    {
    //        In.ViewTanget,
    //        In.ViewBiNormal,
    //        In.ViewNormal,
    //    };

    //    vNormal = normalize(mul(vNormal, matTBN));
    //}
    
    float4 SpecularCoeff = float4(1.0f, 1.0f, 1.0f, 1.0f);
    
    if (usedSpecular == 1)
    {
        SpecularCoeff = specularTexture.Sample(anisotropicSampler, In.UV);
    }
    
    
    if (int_0 == 1)
    {
       
        
        objColor = float4(1.0f, 0.f, 0.f, 1.f);

    }
   
    OutColor.Position = float4(In.ViewPos, 1.0f);
    OutColor.Normal = float4(In.ViewNormal, 1.0f);
    OutColor.Albedo = objColor;
    OutColor.Specular = SpecularCoeff;
  
    
    return OutColor;
}
