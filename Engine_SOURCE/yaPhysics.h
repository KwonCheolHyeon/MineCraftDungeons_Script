#pragma once
#include "yaEngine.h"
#include "mdStruct.h"


namespace md
{
	class GameObject;
	class Physics
	{
	public:
		Physics();
		~Physics();

	public:
		void Initialize();
		void Update();
		void Render();

	public:
		void AddActor(GameObject* _pGameObject);
		void RemoveActor(GameObject* _pGameObject);

		const math::Vector3& GetGravity() { return mGravity; }
		physx::PxPhysics* GetPhysics() { return mPhysics; }

		static physx::PxFilterFlags PlayerFilterShader(
			physx::PxFilterObjectAttributes _attributes0, physx::PxFilterData _filterData0,
			physx::PxFilterObjectAttributes _attributes1, physx::PxFilterData _filterData1,
			physx::PxPairFlags& _pairFlags, const void* _pConstantBlock, physx::PxU32 _constantBlockSize);

		// Raycast 충돌 처리를 위한 QueryFilterData 세팅
		void SetupFiltering(physx::PxShape* shape, uint32_t _layerIndex) const;

		// LayerType에 따라 Raycast 활성화
		void EnableRaycast(uint32_t _leftLayerType, uint32_t _rightLayerType, bool _enable);
		
		// PhysX Raycast 실행
		//_origin: 레이의 시작점
		//_direction : 레이의 방향
		//_maxDistance : 레이가 얼마나 멀리까지 검사할 것인지 나타내는 최대 거리
		//_outHit : 레이가 어떤 물체와 충돌했을 때, 그 충돌 정보를 저장할 구조체
		bool Raycast(uint32_t _layerIndex, const math::Vector3& _origin, const math::Vector3& _direction, float _maxDistance, tRaycastHit* _outHit) const;

	private:
		physx::PxDefaultAllocator	   mAllocCallback;
		physx::PxDefaultErrorCallback  mErrorCallback;
		physx::PxFoundation*		   mFoundation;
		physx::PxPhysics*			   mPhysics;

		physx::PxPvdTransport*			mTransfort;
		physx::PxPvd*					mPvd;
		physx::PxPvdSceneClient*		mSceneClient;
		physx::PxScene*					mScene;
		physx::PxControllerManager*		mControllerMgr;

		physx::PxDefaultCpuDispatcher*	mCpuDispatcher;
		class ContactCallback*			mCallback;

		math::Vector3 mGravity;

		std::array<std::bitset<32>, 32> mRaycastMask;
	};
}
