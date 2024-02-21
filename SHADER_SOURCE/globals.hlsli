#include "Light.hlsli"
#include "Particle.hlsli"
#include "Resource.hlsli"
#include "Sampler.hlsli"
#include "Blur.hlsli"
#include "BoneAnimation.hlsli"


float4 decode(float _value)
{
    uint rgba = asint(_value);

    float r = float((rgba & 0xff000000) >> 24) / 255.f;
    float g = float((rgba & 0x00ff0000) >> 16) / 255.f;
    float b = float((rgba & 0x0000ff00) >> 8) / 255.f;
    float a = float((rgba & 0x000000ff) >> 0) / 255.f;

    return float4(r, g, b, a);
}
