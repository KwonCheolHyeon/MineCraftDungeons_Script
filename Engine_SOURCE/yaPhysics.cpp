#include "yaPhysics.h"
#include "yaGameObject.h"
#include "yaRigidBody.h"
#include "yaTime.h"
#include "yaContactCallback.h"


namespace md
{
	Physics::Physics()
		: mFoundation(nullptr)
		, mPhysics(nullptr)
		, mTransfort(nullptr)
		, mPvd(nullptr)
		, mSceneClient(nullptr)
		, mScene(nullptr)
		, mControllerMgr(nullptr)
		, mCpuDispatcher(nullptr)
		, mCallback(nullptr)
		, mGravity(0.f, -25.81f, 0.f)
	{
	}
	Physics::~Physics()
	{
		mCpuDispatcher->release();

		SAFE_DELETE(mCallback);
		mScene->release();
		mPhysics->release();
		mPvd->release();
		mTransfort->release();
		mFoundation->release();
	}
	void Physics::Initialize()
	{
		mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, mAllocCallback, mErrorCallback);

		mTransfort = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
		mPvd = PxCreatePvd(*mFoundation);
		mPvd->connect(*mTransfort, physx::PxPvdInstrumentationFlag::eALL);

		mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, physx::PxTolerancesScale(), false, mPvd);

		mCpuDispatcher = physx::PxDefaultCpuDispatcherCreate(1);
		mCallback = new ContactCallback;

		physx::PxSceneDesc sceneDesc(mPhysics->getTolerancesScale());
		sceneDesc.gravity = mGravity;
		sceneDesc.cpuDispatcher = mCpuDispatcher;
		sceneDesc.filterShader = PlayerFilterShader;
		sceneDesc.simulationEventCallback = mCallback;

		mScene = mPhysics->createScene(sceneDesc);
		mSceneClient = mScene->getScenePvdClient();

		mSceneClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		mSceneClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		mSceneClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);


	}
	void Physics::Update()
	{
		mScene->simulate(Time::DeltaTime());
		mScene->fetchResults(true);
	}
	void Physics::Render()
	{
	}
	void Physics::AddActor(GameObject* _gameObject)
	{
		AssertEx(_gameObject, L"Physics::AddActor() - GameObject is nullptr");
		AssertEx(_gameObject->GetComponent<RigidBody>(), L"Physics::AddActor() - RigidBody is nullptr");
		AssertEx(_gameObject->GetComponent<RigidBody>()->IsAppliedPhysics(), L"Physics::AddActor() - Is not applied physics");
		mScene->addActor(*_gameObject->GetComponent<RigidBody>()->GetActor());
	}
	void Physics::RemoveActor(GameObject* _gameObject)
	{
		AssertEx(_gameObject, L"Physics::RemoveActor() - GameObject is nullptr");
		AssertEx(_gameObject->GetComponent<RigidBody>(), L"Physics::RemoveActor() - RigidBody is nullptr");
		AssertEx(_gameObject->GetComponent<RigidBody>()->IsAppliedPhysics(), L"Physics::RemoveActor() - Is not applied physics");
		mScene->removeActor(*_gameObject->GetComponent<RigidBody>()->GetActor());
	}
	physx::PxFilterFlags Physics::PlayerFilterShader(physx::PxFilterObjectAttributes _attributes0, physx::PxFilterData _filterData0, physx::PxFilterObjectAttributes _attributes1, physx::PxFilterData _filterData1, physx::PxPairFlags& _pairFlags, const void* _constantBlock, physx::PxU32 _constantBlockSize)
	{
		// 트리거와 트리거 또는 트리거와 충돌하는 물체를 구분하여 처리합니다.
		if (physx::PxFilterObjectIsTrigger(_attributes0) || physx::PxFilterObjectIsTrigger(_attributes1))
		{
			if ((_filterData1.word1 & _filterData0.word0) || (_filterData0.word1 & _filterData1.word0))
			{
				// 트리거와 충돌하는 물체 모두 OnTrigger 이벤트를 처리하도록 합니다.
				_pairFlags = physx::PxPairFlag::eTRIGGER_DEFAULT | physx::PxPairFlag::eNOTIFY_TOUCH_FOUND | physx::PxPairFlag::eNOTIFY_TOUCH_LOST;
				return physx::PxFilterFlag::eDEFAULT;
			}
		}

		// 충돌하는 물체의 경우 충돌 플래그만 생성합니다.
		_pairFlags = physx::PxPairFlag::eCONTACT_DEFAULT;

		// 두 필터가 서로 충돌플래그가 세워져 있을 경우
		if ((_filterData0.word0 & _filterData1.word1) || (_filterData1.word0 & _filterData0.word1))
		{
			_pairFlags |= physx::PxPairFlag::eNOTIFY_TOUCH_FOUND | physx::PxPairFlag::eNOTIFY_TOUCH_LOST;
			return physx::PxFilterFlag::eDEFAULT;
		}

		return physx::PxFilterFlag::eKILL;
	}
	void Physics::SetupFiltering(physx::PxShape* shape, uint32_t _layerIndex) const
	{
		std::bitset<32> layer{};

		layer[_layerIndex] = true;

		// Collision FilterData
		//physx::PxFilterData filterData{};
		//filterData.word0 = layer.to_ulong();					  // word0 = own ID
		//filterData.word1 = mCollisionMask[layerIndex].to_ulong(); // word1 = ID mask to filter pairs that trigger a contact callback
		//shape->setSimulationFilterData(filterData);

		// Raycast FilterData
		physx::PxFilterData queryFilterData{};
		queryFilterData.word0 = layer.to_ulong();					   // word0 = own ID
		queryFilterData.word1 = mRaycastMask[_layerIndex].to_ulong(); // word1 = ID mask to filter pairs that trigger a contact callback
		shape->setQueryFilterData(queryFilterData);
	}
	void Physics::EnableRaycast(uint32_t _leftLayerType, uint32_t _rightLayerType, bool _enable)
	{
		// Raycast 가능 여부 설정
		mRaycastMask[_leftLayerType][_rightLayerType] = _enable;
		mRaycastMask[_rightLayerType][_leftLayerType] = _enable;
	}
	bool Physics::Raycast(uint32_t _layerIndex, const math::Vector3& _origin, const math::Vector3& _direction, float _maxDistance, tRaycastHit* _outHit) const
	{
		std::bitset<32> layer{};
		layer[_layerIndex] = true;

		const physx::PxHitFlags  hitFlag = physx::PxHitFlag::eDEFAULT;
		
		physx::PxQueryFilterData filter{};
		filter.data.word0 = mRaycastMask[_layerIndex].to_ulong();
		filter.flags = physx::PxQueryFlag::eDYNAMIC | physx::PxQueryFlag::eSTATIC;

		// Raycast 수행, Object에 충돌 시 true 반환
		physx::PxRaycastBuffer hit{};
		const bool result = mScene->raycast(physx::PxVec3(_origin), physx::PxVec3(_direction), _maxDistance, hit, hitFlag, filter);
		hit.hasBlock;
		// 충돌 테스트용 코드
		if (result)
			int a = 0;
		
		// 충돌 된 Object 정보를 tRaycastHit 구조체에 저장
		_outHit->gameObject = (hit.hasBlock) ? static_cast<GameObject*>(hit.block.actor->userData) : nullptr;
		_outHit->hasBlocking = hit.hasBlock;
		_outHit->hitDistance = hit.block.distance;
		_outHit->hitPosition = physx::PxVec3(hit.block.position);
		_outHit->hitNormal = physx::PxVec3(hit.block.normal);
		return result;
		
	}
}
