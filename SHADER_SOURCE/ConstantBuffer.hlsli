cbuffer Transform : register(b0)
{
    row_major matrix world;
    row_major matrix inverseWorld;
    row_major matrix view;
    row_major matrix inverseView;
    row_major matrix projection;
}

cbuffer MaterialData : register(b1)
{
    uint usedAlbedo;
    uint usedNormal;
    uint usedSpecular;
    uint usedAnimation;
    
    uint padd0;
    uint padd;
    uint padd1;
    uint padd2;
    
    int int_0;
    int int_1;
    int int_2;
    int int_3;
    
    float float_0;
    float float_1;
    float float_2;
    float float_3;
    
    float2 float2_0;
    float2 float2_1;
    float2 float2_2;
    float2 float2_3;

    float4 float4_0;
    float4 float4_1;
    float4 flaot4_2;
    float4 float4_3;

    matrix mat_0;
    matrix mat_1;
    matrix mat_2;
    matrix mat_3;
}

cbuffer Grid : register(b2)
{
    float4 cameraPosition;
    float2 cameraScale;
    float2 resolution;
}

cbuffer Animation : register(b3)
{
    float2 leftTop;
    float2 spriteSize;
    float2 offset;
    float2 atlasSize;

    uint animationType;
}

cbuffer NumberOfLight : register(b4)
{
    uint numberOfLight;
    uint indexOfLight;
}

cbuffer ParticleSystem : register(b5)
{
    float4 worldPosition;
    float4 startColor;
    float4 startSize;
    
    uint maxParticles;
    uint simulationSpace;
    float radius;
    float startSpeed;
    
    float startLifeTime;
    float deltaTime;
    float elapsedTime; //누적시간
    int padding;
}


cbuffer LightMatrix : register(b8)
{
    row_major Matrix lightView;
    row_major Matrix lightProjection;
};
