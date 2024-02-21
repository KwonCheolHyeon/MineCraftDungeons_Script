#pragma once
#include "yaEngine.h"
#include "yaComponent.h"
#include "mdStruct.h"

namespace md
{
	enum class FreezeRotationFlag : uint8_t
	{
		ROTATION_X = (1 << 1),
		ROTATION_Y = (1 << 2),
		ROTATION_Z = (1 << 4),

		END
	};

	class RigidBody :
		public Component
	{
	public:
		RigidBody();
		virtual ~RigidBody();

		virtual void Initalize() override;
		virtual void Update() override {};
		virtual void FixedUpdate()override;
		virtual void Render() override {};
		virtual void PrevRender() override {};
		virtual void Destroy();

		// PhysX Obejct 생성
		void SetPhysical(const tPhysicsInfo& _physicsInfo = tPhysicsInfo());
		
		void SeAppliedtPhysics(bool _appliedPhysics) { mbAppliedPhysics = _appliedPhysics; }
		bool IsAppliedPhysics();

		physx::PxActor* GetActor() { return mActor; }

		template<typename T>
		inline T* GetActor() const
		{
			T* pActor = mActor->is<T>();
			assert(pActor);
			return pActor;
		}

		// 현재 씬에 Actor 추가
		void AddActorToScene();

		physx::PxRigidDynamic* GetDynamicActor() { return GetActor<physx::PxRigidDynamic>(); }
		physx::PxRigidStatic* GetStaticActor() { return GetActor<physx::PxRigidStatic>(); }
		physx::PxShape* GetShape() { return mShape; }
		physx::PxTransform GetPhysicsTransform();
		void			   SetPhysicsTransform(physx::PxTransform _transform);

		eActorType GetActorType() { return mPhysicsInfo.eActorType; }

		eGeometryType GetGeometryType() { return mPhysicsInfo.eGeomType; }
		math::Vector3 GetGeometrySize() { return mPhysicsInfo.size * 2.f; }
		float GetGeometrySphereRadius() { return mPhysicsInfo.size.x * 2.f; }
		Geometries* GetGeometry() { return mPhysicsInfo.pGeometries; }
		const physx::PxFilterData& GetFilterData() { return mPhysicsInfo.filterData; }

		void SetOtherLayerInFilterData(enums::eLayerType _eOtherLayer) { mPhysicsInfo.filterData.word1 |= 1 << static_cast<int>(_eOtherLayer); }

		void SetMassForDynamic(float _mass);
		// Actor의 선형 속도 설정
		void SetVelocity(const math::Vector3& _velocity);
		// Actor의 이동
		void SetVelocity(math::eAxis3D _eAxis, float _velocity);
		void SetVelocity(enums::eAXIS axis, const math::Vector3& velocity);
		void AddVelocity(const math::Vector3& _velocity);
		void AddVelocity(math::eAxis3D _eAxis, float _velocity);
		void AddGravity();

		void SetMaxVelocity(float _maxVelocity);
		void CheckMaxVelocity();

		void ApplyGravity();
		void RemoveGravity();

		// object 축 잠금
		void SetFreezeRotation(FreezeRotationFlag flag, bool enable);

		// object 선형 감쇠
		void SetLinearDamping(float _damping);
		// PhysX 시뮬레이션에 포함 여부 설정
		void SetSimulationShapeFlag(bool _bFlag);
		// PhysX 충돌 이벤트를 트리거로 처리 할지 설정
		// 트리거 : 물리적 충돌에 사용X, 보통 겹쳐지는 지를 확인
		void SetTriggerShapeFlag(bool _bFlag);
		
		void SetActorInSceneFlag(bool _bFlag);
		bool IsActorInScene() { return mbIsActorInScene; }

		// Actor에 힘을 가하는 함수
		void AddForce(const math::Vector3& _force);

		void CustomFBXLoad(const std::string& _meshName, float _ratio, bool _inverseIndex);

		void IsCreateTriangleActors(bool triangleActors) { mbTriangleActors = triangleActors; }
		void SetTriangleVertexVec(std::vector<std::vector<physx::PxVec3>> _vertexVec){ mVertexVec = _vertexVec;}
		void SetTriangleIndexVec(std::vector<std::vector<physx::PxU32>>  _indexVec) { mIndexVec = _indexVec; }

	private:
		void CreateBoxGeometry();
		void CreateCapsuleGeometry();
		void CreatePlaneGeometry();
		void CreateSphereGeometry();

	private:
		void InitializeActor();
		void CreateActor();
		void CreateShape();
		void CreateMaterial();
		void CreateGeometry();

		//
		void CreateTriangleActors();
	private:
		EnumFlags<FreezeRotationFlag, uint16_t> mFreezeRotationFlag;
		// PhysX Object설정을 위한 구조체
		tPhysicsInfo mPhysicsInfo;

		physx::PxActor* mActor;
		physx::PxShape* mShape;
		physx::PxMaterial* mMaterial;

		// Fbx 관련
		// 물리적인 정점
		std::vector<std::vector<physx::PxVec3>> mVertexVec;
		// 삼각형 인덱스
		std::vector<std::vector<physx::PxU32>> mIndexVec;

		math::Vector3 mVelocity;
		float mMaxVelocity;

		bool mbAppliedPhysics;
		bool mbAppliedGravity;
		bool mbIsActorInScene;

		bool mbTriangleActors;
	};
}
