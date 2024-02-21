
struct SkinningInfo
{
    float3 Pos;
    float3 Tangent;
    float3 BiNormal;
    float3 Normal;
};

cbuffer BoneInfo : register(b7)
{
    uint boneCount;
    uint frameIdx;
    uint nextFrameIdx;
    float frameRatio;
}

StructuredBuffer<Matrix> BoneMatrix : register(t30);

matrix GetBoneMat(int _iBoneIdx, int _iRowIdx)
{
    return BoneMatrix[(boneCount * _iRowIdx) + _iBoneIdx];
}

void Skinning(inout float3 _vPos, inout float3 _vTangent, inout float3 _vBinormal, inout float3 _vNormal
    , inout float4 _vWeight, inout float4 _vIndices, int _iRowIdx)
{
    SkinningInfo info = (SkinningInfo) 0.f;

    if (_iRowIdx == -1)
        return;

    for (int i = 0; i < 4; ++i)
    {
        if (0.f == _vWeight[i])
            continue;

        matrix matBone = GetBoneMat((int) _vIndices[i], _iRowIdx);

        info.Pos += (mul(float4(_vPos, 1.f), matBone) * _vWeight[i]).xyz;
        info.Tangent += (mul(float4(_vTangent, 0.f), matBone) * _vWeight[i]).xyz;
        info.BiNormal += (mul(float4(_vBinormal, 0.f), matBone) * _vWeight[i]).xyz;
        info.Normal += (mul(float4(_vNormal, 0.f), matBone) * _vWeight[i]).xyz;
    }

    _vPos = info.Pos;
    _vTangent = normalize(info.Tangent);
    _vBinormal = normalize(info.BiNormal);
    _vNormal = normalize(info.Normal);
}



void Skinning(inout float3 _vPos, inout float4 _vWeight, inout float4 _vIndices, int _iRowIdx)
{
    SkinningInfo info = (SkinningInfo) 0.f;

    if (_iRowIdx == -1)
        return;

    for (int i = 0; i < 4; ++i)
    {
        if (0.f == _vWeight[i])
            continue;

        matrix matBone = GetBoneMat((int) _vIndices[i], _iRowIdx);

        info.Pos += (mul(float4(_vPos, 1.f), matBone) * _vWeight[i]).xyz;
    }

    _vPos = info.Pos;
}
