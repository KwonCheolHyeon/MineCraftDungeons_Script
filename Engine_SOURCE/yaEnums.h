#pragma once
#include <PxPhysicsAPI.h>
#include "yaMath.h"

namespace md::enums
{
	enum class eSceneType
	{
		TitleScene = 0,
		PlayScene,
		UITestScene,
		PhysXTestScene,
		AnimationFunctionTestScene,
		NavTestScene,
		ZombieMonsterTestScene,
		CreeperTestScene,
		RedStoneGolemTestScene,
		End,
	};

	enum class eLayerType
	{
		None = 0,
		Camera,
		Grid,
		Light,
		Monster,
		Player,
		Stage,
		Wall,
		Particle,
		PostProcess,
		Mouse,
		UI,

		End = 16,
	};
	enum
	{
		LAYER_TYPE_COUNT = static_cast<int>(eLayerType::End),
	};

	enum class eComponentType
	{
		None,
		Transform, // 위치 데이터 수정하는 컴포넌트
		Camera,
		Mesh,
		Collider,
		RigidBody,
		//Collider2,
		MeshRenderer,
		SpriteRenderer,
		Animator,
		BoneAnimator,
		ParticleSystem,
		AudioListener,
		AudioSource,
		Light,
		UI,
		//FadeOut,FadeIn
		Script,
		End,
	};

	enum class eResourceType
	{
		Mesh,
		Texture,
		Material,
		Sound,
		/*Font,*/
		Prefab,
		MeshData,
		GraphicShader,
		ComputeShader,
		AudioClip,
		Script,
		End,
	};

	enum class eColliderType
	{
		None,
		Rect,
		Circle,
		Box,
		Capsule,
		Sphere,
		Plane,
		Triangle,
		End,
	};

	enum class eAnimationType
	{
		None,
		SecondDimension,
		ThirdDimension,
		End,
	};

	enum class eLightType
	{
		Directional,
		Point,
		Spot,
		End,
	};

	enum class eAXIS
	{
		X,
		Y,
		Z,
		XY,
		XZ,
		YZ,
		XYZ,
		END,
	};

	extern const char* charComponentType[(int)eComponentType::End];
	extern const wchar_t* wcharComponentType[(int)eComponentType::End];
	extern const char* charResourceType[(int)eResourceType::End];
	extern const wchar_t* wcharResourceType[(int)eResourceType::End];

// ============
// PhysX enum
// ============
	enum class eActorType
	{
		Static,
		Dynamic,
		Kinematic,
		Character,
	};

	enum class eGeometryType
	{
		None,
		Sphere,
		Box,
		Capsule,
		Plane
	};

	enum class eModelType
	{
		StaticModel,
		DynamicModel,
	};
}
