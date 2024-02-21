#pragma once
#include <wrl.h>
#include "yaMath.h"

#include <d3d11.h>
#include <d3dcompiler.h>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")


#define CB_GETBINDSLOT(name) __CBUFFERBINDSLOT__##name##__
#define CBUFFER(name, slot) static const int CB_GETBINDSLOT(name) = slot; struct alignas(16) name

#define CBSLOT_TRANSFORM		0
#define CBSLOT_MATERIAL			1
#define CBSLOT_GRID				2
#define CBSLOT_ANIMATION		3
#define CBSLOT_NUMBEROFLIGHT	4
#define CBSLOT_PARTICLESYSTEM	5
#define CBSLOT_NOISE			6
#define CBSLOT_BONEANIMATION    7
#define CBSLOT_LIGHTMATRIX	    8

using namespace md::math;
namespace md::graphics
{
	enum class ValidationMode
	{
		Disabled,
		Enabled,
		GPU,
	};

	enum class eShaderStage
	{
		VS,
		HS,
		DS,
		GS,
		PS,
		CS,
		ALL,
		Count,
	};

	enum class eSamplerType
	{
		Point,
		Linear,
		Anisotropic,
		End,
	};

	enum class eRSType
	{
		SolidBack,
		SolidFront,
		SolidNone,
		WireframeNone,
		End,
	};

	enum class eDSType
	{
		Less,
		Greater,
		NoWrite,
		None,
		End,
	};

	enum class eBSType
	{
		Default,
		AlphaBlend,
		OneOne,
		End,
	};

	enum class eRTType
	{
		Swapchain,
		Deffered,
		Light,
		Shadow,
		End,
	};

	enum class eRenderingMode
	{
		DefferdOpaque,
		DefferdMask, 

		//광원처리
		Light,

		Opaque,
		CutOut,
		Transparent,
		PostProcess,
		None,
		End,
	};


	struct Vertex
	{
		Vector4 pos;
		Vector4 color;
		Vector2 uv;
		Vector3 tangent;
		Vector3 biNormal;
		Vector3 normal;

		Vector4 weight;
		Vector4 Indices;
	};

	struct GpuBuffer
	{
		enum class eType
		{
			Buffer,
			Texture,
			UnknownType,
		} type = eType::UnknownType;

		D3D11_BUFFER_DESC desc = {};
		Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;

		GpuBuffer() = default;
		virtual ~GpuBuffer() = default;
	};

	enum class eCBType
	{
		Transform,
		Material,
		Grid,
		Animation,
		Light,
		ParticleSystem,
		Noise,
		Bone,
		LightMatrix,
		End,
	};

	enum class eGPUParam
	{
		Int_0,
		Int_1,
		Int_2,
		Int_3,
		Float_0,
		Float_1,
		Float_2,
		Float_3,
		Vector2_0,
		Vector2_1,
		Vector2_2,
		Vector2_3,
		Vector3_0,
		Vector3_1,
		Vector3_2,
		Vector3_3,
		Vector4_0,
		Vector4_1,
		Vector4_2,
		Vector4_3,
		Matrix_0,
		Matrix_1,
		Matrix_2,
		Matrix_3,
	};

	enum class eSRVType
	{
		SRV,//Shader Resource View
		UAV,
		End,
	};

	//Texture2D positionTarget : register(t2);
	//Texture2D normalTarget : register(t3);
	//Texture2D albedoTarget : register(t4);
	//Texture2D specularTarget : register(t5);

	enum class eTextureSlot
	{
		Albedo,
		Normal,

		PositionTarget,
		NormalTarget,
		AlbedoTarget,
		SpecularTarget,
		DiffuseLightTarget,
		SpecularLightTarget,

		Specular,
		Emissive,
		ShadowMap,
		//CubeT8,
		//CubeT9,
		//SkyBox = 11,
		//Array2DT10,
		//Array2DT11,

		End,
	};

	struct MaterialColr
	{
		Vector4 diffuseColor;
		Vector4 specularColor;
		Vector4 AmbientColor;
		Vector4 EmessiveColor;
	};

	struct DebugMesh
	{
		enums::eColliderType type;
		math::Vector3 position;
		math::Vector3 rotatation;
		math::Vector3 scale;
		
		float radius;
		float duration;
		float time;
	};

	struct LightAttribute
	{
		Vector4 diffuse;
		Vector4 specular;
		Vector4 ambient;

		Vector4 position;
		Vector4 direction;

		enums::eLightType type;
		float radius;
		float angle;
		int padding;
	};

	struct Particle
	{
		Vector4 position;
		Vector4 direction;

		float lifeTime;
		float time;
		float speed;
		UINT active;
	};

	struct ParticleShared
	{
		UINT activeCount;
	};

	///Animation 3D
	struct BoneFrameTransform
	{
		Vector4 translate;
		Vector4 scale;
		Vector4 rotation;
	};

	struct BoneKeyFrame
	{
		Matrix bone;

		double time;
		int frame;
		Vector3 translate;
		Vector3 scale;
		Vector4 rotation;
	};

	struct BoneMatrix
	{
		std::wstring name;
		int depth;
		int parentIdx;
		Matrix offset;
		Matrix bone;
		std::vector<BoneKeyFrame> keyFrames;
	};

	struct BoneAnimationClip
	{
		std::wstring name;
		int originStartFrame;
		int originEndFrame;
		int originFrameLength;
			
		double originStartTime;
		double originEndTime;
		double originTimeLength;
		float updateTime;
		//fbxsdk::FbxTime::EMode mode;
		UINT mode;

		int limitedStartFrame;
		int limitedEndFrame;
		int limitedFrameLength;

		bool isLoop;
	};

	
}
