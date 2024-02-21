#include "globals.hlsli"

struct VSIn
{
    float3 Position : POSITION;
    float4 Color : COLOR;
    float2 TexCoord : TEXCOORD;
};

struct VSOut
{
    float4 Position : SV_Position;
    float4 Color : COLOR;
    float2 TexCoord : TEXCOORD0;
};

VSOut main(VSIn In)
{
    VSOut OUT;

    float3 cameraPos = float4_0.xyz;

    // 카메라와 billboard 사이의 수평 방향 벡터를 계산
    float3 toCameraHorizontal = cameraPos - In.Position;
    toCameraHorizontal.y = 0.0f;
    toCameraHorizontal = normalize(toCameraHorizontal);

    // Z축과 toCameraHorizontal 벡터의 외적을 이용하여 right 방향을 계산
    float3 rightDirection = cross(float3(0.0f, 0.0f, 1.0f), toCameraHorizontal);
    
    // Z축은 up 방향으로 고정
    float3 upDirection = float3(0.0f, 0.0f, 1.0f);

    // 정점 위치를 조정
    float3 newPosition = In.Position + In.Position.x * rightDirection + In.Position.y * upDirection;

    // 변경된 위치를 월드, 뷰, 프로젝션 매트릭스로 변환
    float4 worldPosition = mul(float4(newPosition, 1.0f), world);
    float4 viewPosition = mul(worldPosition, view);
    OUT.Position = mul(viewPosition, projection);

    OUT.Color = In.Color;
    OUT.TexCoord = In.TexCoord;

    return OUT;
}
