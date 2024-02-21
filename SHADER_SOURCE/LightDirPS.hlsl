#include "globals.hlsli"

struct VSIn
{
    float4 Position : POSITION;
};

struct VSOut
{
    float4 Position : SV_Position;
};


struct PS_OUT
{
    float4 vDiffuse : SV_Target;
    float4 vSpecular : SV_Target1;
    //float4 vEmssive : SV_Target2;
};


		//albedo = Resources::Find<Texture>(L"PositionTarget");
		//lightMaterial->SetTexture(eTextureSlot::PositionTarget, albedo);
		//albedo = Resources::Find<Texture>(L"NormalTarget");
		//lightMaterial->SetTexture(eTextureSlot::NormalTarget, albedo);
		//albedo = Resources::Find<Texture>(L"SpecularTarget");
		//lightMaterial->SetTexture(eTextureSlot::SpecularTarget, albedo);

PS_OUT main(VSOut _in)
{
    PS_OUT output = (PS_OUT) 0.f;
    
    float2 vUV = _in.Position.xy / float2(1600.0f, 900.0f);
    float4 vViewPos = positionTarget.Sample(anisotropicSampler, vUV);
    
    if (0.f == vViewPos.a)
        discard;
      
    float4 vViewNormal = normalTarget.Sample(anisotropicSampler, vUV);
        
    LightColor lightcolor = (LightColor) 0.f;
    CalculateLight3D(vViewPos.xyz, vViewNormal.xyz, 0, lightcolor);
    
    // 그림자 판정
    // ViewPos -> WorldPos
    float3 vWorldPos = mul(float4(vViewPos.xyz, 1.f), inverseView).xyz;

    // WorldPos -> Light 투영
    float4 vLightProj = mul(float4(vWorldPos, 1.f), lightView);
    vLightProj = mul(float4(vWorldPos, 1.f), lightProjection);

    // w 로 나눠서 실제 xy 투영좌표를 구함
    vLightProj.xy /= vLightProj.w;
    vLightProj.z /= vLightProj.w;

    // 샘플링을 하기 위해서 투영좌표계를 UV 좌표계로 변환
    float2 vDepthMapUV = float2((vLightProj.x * 0.5) + 0.5f, -(vLightProj.y * 0.5) + 0.5f);
    float fDepth = ShadowMapTarget.Sample(anisotropicSampler, vDepthMapUV).r;
    float fShadowPow = 0.f;

    //// 광원에 기록된 깊이보다, 물체의 깊이가 더 멀 때, 그림자 판정
    if (0.f != fDepth
        && 0.f <= vDepthMapUV.x && vDepthMapUV.x <= 1.f
        && 0.f <= vDepthMapUV.y && vDepthMapUV.y <= 1.f
        && vLightProj.z >= fDepth + 0.0001f)
    {
        fShadowPow = 0.9f;
    }
        // 광원에 기록된 깊이보다, 물체의 깊이가 더 멀 때, 그림자 판정
    //if (vLightProj.z >= fDepth + 0.0001f)
    //{
    //    fShadowPow = 0.9f;
    //}
        
    output.vDiffuse = lightcolor.diffuse * (1.f - fShadowPow) + lightcolor.ambient;
    
    float SpecCoef = specularTarget.Sample(anisotropicSampler, vUV).x;
    float4 vSpec = decode(SpecCoef);

    //output.vDiffuse = lightcolor.diffuse + lightcolor.ambient /*+ vEmssive*/;
    output.vSpecular.xyz = lightcolor.specular.xyz; // * vSpec.xyz;
       
    output.vDiffuse.a = 1.f;
    output.vSpecular.a = 1.f;
    
    return output;
}
