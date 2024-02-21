#pragma once
#include "CommonInclude.h"
#include "yaMath.h"
#include "yaGraphicDevice_DX11.h"

#include "yaMesh.h"
#include "yaShader.h"
#include "yaConstantBuffer.h"
#include "yaCamera.h"
#include "yaLight.h"
#include "yaStructedBuffer.h"

#include "yaMultiRenderTarget.h"

using namespace md::math;
using namespace md::graphics;

namespace md::renderer
{
	CBUFFER(TransformCB, CBSLOT_TRANSFORM)
	{
		Matrix world;
		Matrix inverseWorld;
		Matrix view;
		Matrix inverseView;
		Matrix projection;
	};

	CBUFFER(MaterialCB, CBSLOT_MATERIAL)
	{
		UINT usedAlbedo;
		UINT usedNormal;
		UINT usedSpecular;
		UINT usedAnimation;

		UINT usedBoneus;
		int pad;
		int pad1;
		int pad2;

		int int_0;
		int int_1;
		int int_2;
		int int_3;

		float float_0;
		float float_1;
		float float_2;
		float float_3;

		Vector2 float2_0;
		Vector2 float2_1;
		Vector2 float2_2;
		Vector2 float2_3;

		Vector3 float3_0;
		Vector3 float3_1;
		Vector3 float3_2;
		Vector3 float3_3;

		Vector4 float4_0;
		Vector4 float4_1;
		Vector4 float4_2;
		Vector4 float4_3;

		Matrix mat_0;
		Matrix mat_1;
		Matrix mat_2;
		Matrix mat_3;
	};

	CBUFFER(GridCB, CBSLOT_GRID)
	{
		Vector4 cameraPosition;
		Vector2 cameraScale;
		Vector2 resolution;
	};

	CBUFFER(AnimationCB, CBSLOT_ANIMATION)
	{
		Vector2 leftTop;
		Vector2 size;
		Vector2 offset;
		Vector2 atlasSize;

		UINT type;
	};

	//CBSLOT_NUMBEROFLIGHT
	CBUFFER(LightCB, CBSLOT_NUMBEROFLIGHT)
	{
		UINT numberOfLight;
		UINT indexOfLight;
	};

	CBUFFER(ParticleSystemCB, CBSLOT_PARTICLESYSTEM)
	{
		Vector4 worldPosition;
		Vector4 startColor;
		Vector4 startSize;

		UINT maxParticles;
		UINT simulationSpace;
		float radius;
		float startSpeed;

		float startLifeTime;
		float deltaTime;
		float elapsedTime; //누적시간
		int padding;
	};

	CBUFFER(NoiseCB, CBSLOT_NOISE)
	{
		Vector4 noiseSize;
		float noiseTime;
	};

	CBUFFER(BoneAnimationCB, CBSLOT_BONEANIMATION)
	{
		UINT boneCount;
		UINT frameIdx;
		UINT nextFrameIdx;
		float frameRatio;
	};

	CBUFFER(LightMatrixCB, CBSLOT_LIGHTMATRIX)
	{
		Matrix lightView;
		Matrix lightProjection;
	};

	extern Vertex vertexes[4];
	extern Camera* mainCamera;
	extern Camera* mainUICamera;
	extern ConstantBuffer* constantBuffers[];
	extern Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerStates[];
	extern Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerStates[];
	extern Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthstencilStates[];
	extern Microsoft::WRL::ComPtr<ID3D11BlendState> blendStates[];
	

	extern graphics::MultiRenderTarget* renderTargets[];

	extern std::vector<Camera*> cameras[];
	extern std::vector<DebugMesh> debugMeshes;
	extern std::vector<Light*> lights;
	extern std::vector<LightAttribute> lightsAttribute;
	extern StructedBuffer* lightsBuffer;

	extern md::GameObject* inspectorGameObject;

	void Initialize();
	void Render();
	void Release();

	//mrt
	void CreateRenderTargets();
	void ClearRenderTargets();

	//Renderer
	void PushLightAttribute(LightAttribute lightAttribute);
	void BindLights();
	void BindNoiseTexture();
	void CopyRenderTarget();
	void LoadUI();
}

