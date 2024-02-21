#include "yaRigidBody.h"
#include "yaApplication.h"
#include "yaPhysics.h"
#include "yaGameObject.h"
#include "yaCollisionManager.h"
#include "yaResources.h"
#include "yaMeshData.h"

//#include "../External/PhysX/Include/cooking/PxCooking.h"

namespace md
{
	RigidBody::RigidBody()
		: Component(enums::eComponentType::RigidBody)
		, mMaxVelocity(5.f)
		, mActor(nullptr)
		, mShape(nullptr)
		, mMaterial(nullptr)
		, mbAppliedPhysics(false)
		, mbAppliedGravity(false)
		, mbIsActorInScene(false)
		, mbTriangleActors(false)
	{
	}

	RigidBody::~RigidBody()
	{
		SAFE_DELETE(mPhysicsInfo.pGeometries);
	}

	void RigidBody::Initalize()
	{
		AssertEx(mbAppliedPhysics,  L"RigidBody::Initialize() - Physics가 적용되지 않도록 했습니다.");
		AssertEx(!mbIsActorInScene,  L"RigidBody::Initialize() - 이미 씬에 추가된 Actor입니다.");
		AssertEx(nullptr != mActor, L"RigidBody::Initialize() - mActor가 nullptr이었습니다.");

		// Physics관리자에 RigidBody를 등록
		if (true == mbAppliedPhysics && false == mbIsActorInScene && nullptr != mActor)
		{
			Application::GetPhysics()->AddActor(GetOwner());
		}
	}

	void RigidBody::FixedUpdate()
	{
		if (true == mbAppliedGravity && false == mbAppliedPhysics)
		{
			AddGravity();
		}

		if (true == mbAppliedPhysics && eActorType::Static == mPhysicsInfo.eActorType)
		{
			return;
		}
		else
		{
			GetOwner()->ReorganizePosition(eLayerType::Wall);

			GetOwner()->GetTransform()->Move(mVelocity);
		}
	}
	void RigidBody::Destroy()
	{
		if (true == mbAppliedPhysics)
		{
			physx::PxRigidActor* pActor = mActor->is<physx::PxRigidActor>();
			pActor->userData = nullptr;
		}
	}

	void RigidBody::SetPhysical(const tPhysicsInfo& _physicsInfo)
	{
		mPhysicsInfo = _physicsInfo;

		CreateMaterial();

		if (mbTriangleActors == true)
		{
			CreateActor();
			CreateTriangleActors();
		}
		else
		{
			CreateGeometry();
			CreateActor();
			CreateShape();
		}

		mbAppliedPhysics = true;

		InitializeActor();
		Initalize();
	}

	bool RigidBody::IsAppliedPhysics()
	{
		return mbAppliedPhysics;
	}
	
	void RigidBody::AddActorToScene()
	{
		AssertEx(mbAppliedPhysics, L"RigidBody::AddActorToScene() - 물리가 들어가지 않은 오브젝트에 대한 AddActorToScene 호출");
		AssertEx(mActor, L"RigidBody::AddActorToScene() - mActor가 생성되지 않음");

		Application::GetPhysics()->AddActor(GetOwner());
		mbIsActorInScene = true;
	}

	physx::PxTransform RigidBody::GetPhysicsTransform()
	{
		AssertEx(mbAppliedPhysics, L"RigidBody::GetPhysicsTransform() - 물리가 들어가지 않은 오브젝트에 대한 GetPhysicsTransform 호출");
		return GetActor<physx::PxRigidActor>()->getGlobalPose();
	}

	void RigidBody::SetPhysicsTransform(physx::PxTransform _transform)
	{
		//_transform.p.z = -_transform.p.z;
		AssertEx(mbAppliedPhysics, L"RigidBody::SetPhysicsTransform() - 물리가 들어가지 않은 오브젝트에 대한 SetPhysicsTransform 호출");
		GetActor<physx::PxRigidActor>()->setGlobalPose(_transform);
	}

	void RigidBody::SetMassForDynamic(float _mass)
	{
		AssertEx(mbAppliedPhysics, L"RigidBody::SetMassForDynamic() - 물리가 들어가지 않은 오브젝트에 대한 SetMassForDynamic 호출");
		physx::PxActor* actor = mShape->getActor();
		physx::PxRigidDynamic* rigidActor = actor->is<physx::PxRigidDynamic>();
		rigidActor->setMass(_mass);
	}

	void RigidBody::SetVelocity(const math::Vector3& _velocity)
	{
		if (true == mbAppliedPhysics)
		{
			//AssertEx(eActorType::Dynamic == mPhysicsInfo.eActorType, L"RigidBody::SetVelocity() - Dynamic Actor가 아닌 물체에 대한 SetVelocity() 호출 시도");
			GetDynamicActor()->setLinearVelocity(_velocity);
			mVelocity = _velocity;
		}
		else
		{
			mVelocity = _velocity;
		}

		CheckMaxVelocity();
	}

	void RigidBody::SetVelocity(math::eAxis3D _eAxis, float _velocity)
	{
		if (true == mbAppliedPhysics)
		{
			//AssertEx(eActorType::Dynamic == mPhysicsInfo.eActorType, L"RigidBody::SetVelocity() - Dynamic Actor가 아닌 물체에 대한 SetVelocity() 호출 시도");
			math::Vector3 velocity = GetDynamicActor()->getLinearVelocity();
			switch (_eAxis)
			{
			case math::eAxis3D::X:
				velocity.x = _velocity;
				break;
			case math::eAxis3D::Y:
				velocity.y = _velocity;
				break;
			case math::eAxis3D::Z:
				velocity.z = _velocity;
				break;
			}
			GetDynamicActor()->setLinearVelocity(velocity);

			mVelocity = velocity;
		}
		else
		{
			switch (_eAxis)
			{
			case math::eAxis3D::X:
				mVelocity.x = _velocity;
				break;
			case math::eAxis3D::Y:
				mVelocity.y = _velocity;
				break;
			case math::eAxis3D::Z:
				mVelocity.z = _velocity;
				break;
			}
		}

		CheckMaxVelocity();
	}

	void RigidBody::SetVelocity(enums::eAXIS axis, const math::Vector3& velocity)
	{
		switch (axis)
		{
		case enums::eAXIS::X:
			mVelocity.x = velocity.x;
			break;
		case enums::eAXIS::Y:
			mVelocity.y = velocity.y;
			break;
		case enums::eAXIS::Z:
			mVelocity.z = velocity.z;
			break;
		case enums::eAXIS::XY:
			mVelocity.x = velocity.x;
			mVelocity.y = velocity.y;
			break;
		case enums::eAXIS::XZ:
			mVelocity.x = velocity.x;
			mVelocity.z = velocity.z;
			break;
		case enums::eAXIS::YZ:
			mVelocity.y = velocity.y;
			mVelocity.z = velocity.z;
			break;
		case enums::eAXIS::XYZ:
			mVelocity.x = velocity.x;
			mVelocity.y = velocity.y;
			mVelocity.z = velocity.z;
			break;
		case enums::eAXIS::END:
			break;
		default:
			break;
		}
	}

	void RigidBody::AddVelocity(const math::Vector3& _velocity)
	{
		if (true == mbAppliedPhysics)
		{
			AssertEx(eActorType::Dynamic == mPhysicsInfo.eActorType, L"RigidBody::AddVelocity() - Dynamic Actor가 아닌 물체에 대한 SetVelocity() 호출 시도");
			math::Vector3 velocity = GetDynamicActor()->getLinearVelocity();
			velocity += _velocity;
			GetDynamicActor()->setLinearVelocity(velocity);
		}
		else
		{
			mVelocity = _velocity;
		}

		CheckMaxVelocity();
	}

	void RigidBody::AddVelocity(math::eAxis3D _eAxis, float _velocity)
	{
		if (true == mbAppliedPhysics)
		{
			AssertEx(eActorType::Dynamic == mPhysicsInfo.eActorType, L"RigidBody::AddVelocity() - Dynamic Actor가 아닌 물체에 대한 SetVelocity() 호출 시도");
			math::Vector3 velocity = GetDynamicActor()->getLinearVelocity();

			switch (_eAxis)
			{
			case math::eAxis3D::X:
				velocity.x += _velocity;
				break;
			case math::eAxis3D::Y:
				velocity.y += _velocity;
				break;
			case math::eAxis3D::Z:
				velocity.z += _velocity;
				break;
			}
			
			GetDynamicActor()->setLinearVelocity(velocity);
		}
		else
		{
			switch (_eAxis)
			{
			case math::eAxis3D::X:
				mVelocity.x += _velocity;
				break;
			case math::eAxis3D::Y:
				mVelocity.y += _velocity;
				break;
			case math::eAxis3D::Z:
				mVelocity.z += _velocity;
				break;
			}
		}

		CheckMaxVelocity();
	}

	void RigidBody::AddGravity()
	{
		mVelocity += GLOBAL_GRAVITY;
	}

	void RigidBody::SetMaxVelocity(float _maxVelocity)
	{
		if (true == mbAppliedPhysics)
		{
			AssertEx(eActorType::Dynamic == mPhysicsInfo.eActorType, L"RigidBody::SetMaxVelocity() - Dynamic Actor가 아닌 물체에 대한 SetMaxVelocity() 호출 시도");
			GetDynamicActor()->setMaxLinearVelocity(_maxVelocity);
		}

		mMaxVelocity = _maxVelocity;
	}

	void RigidBody::CheckMaxVelocity()
	{
		mVelocity.x = mVelocity.x < 0.f ? max(mVelocity.x, -mMaxVelocity) : min(mVelocity.x, mMaxVelocity);
		mVelocity.y = mVelocity.y < 0.f ? max(mVelocity.y, -mMaxVelocity) : min(mVelocity.y, mMaxVelocity);
		mVelocity.z = mVelocity.z < 0.f ? max(mVelocity.z, -mMaxVelocity) : min(mVelocity.z, mMaxVelocity);
	}

	void RigidBody::ApplyGravity()
	{
		if (true == mbAppliedPhysics)
		{
			AssertEx(eActorType::Dynamic == mPhysicsInfo.eActorType, L"RigidBody::ApplyGravity() - Dynamic Actor가 아닌 물체에 대한 ApplyGravity() 호출 시도");
			GetDynamicActor()->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, false);
		}

		mbAppliedGravity = true;
	}

	void RigidBody::RemoveGravity()
	{
		if (true == mbAppliedPhysics)
		{
			AssertEx(eActorType::Dynamic == mPhysicsInfo.eActorType, L"RigidBody::RemoveGravity() - Dynamic Actor가 아닌 물체에 대한 RemoveGravity() 호출 시도");
			GetDynamicActor()->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, true);
		}

		mbAppliedGravity = false;
	}

	void RigidBody::SetFreezeRotation(FreezeRotationFlag flag, bool enable)
	{
		physx::PxActor* actor = mShape->getActor();
		AssertEx(actor, L"RigidBody::SetFreezeRotation - actor가 존재하지 않습니다.");

		physx::PxRigidDynamic* rigidActor = actor->is<physx::PxRigidDynamic>();
		if (rigidActor == nullptr)
		{
			return;
		}

		EnumFlags<FreezeRotationFlag, uint16_t> enumFlag{ flag };

		if (enable)
		{
			mFreezeRotationFlag |= enumFlag;
			if (enumFlag & FreezeRotationFlag::ROTATION_X)
				rigidActor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, true);
			if (enumFlag & FreezeRotationFlag::ROTATION_Y)
				rigidActor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, true);
			if (enumFlag & FreezeRotationFlag::ROTATION_Z)
				rigidActor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, true);
		}
		else
		{
			mFreezeRotationFlag &= ~enumFlag;
			if (enumFlag & FreezeRotationFlag::ROTATION_X)
				rigidActor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, false);
			if (enumFlag & FreezeRotationFlag::ROTATION_Y)
				rigidActor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, false);
			if (enumFlag & FreezeRotationFlag::ROTATION_Z)
				rigidActor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, false);
		}

		rigidActor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_X, true);
		rigidActor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Z, true);
	}

	void RigidBody::SetLinearDamping(float _damping)
	{
		AssertEx(eActorType::Dynamic == mPhysicsInfo.eActorType, L"RigidBody::SetLinearDamping() - Dynamic Actor가 아닌 물체에 대한 SetLinearDamping() 호출 시도");
		GetActor<physx::PxRigidDynamic>()->setLinearDamping(_damping);
	}

	void RigidBody::SetSimulationShapeFlag(bool _bFlag)
	{
		AssertEx(mbAppliedPhysics, L"RigidBody::SetSimulationShapeFlag() - 물리가 들어가지 않은 오브젝트에 대한 SetSimulationShapeFlag 호출");
		mShape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, _bFlag);
	}

	void RigidBody::SetTriggerShapeFlag(bool _bFlag)
	{
		AssertEx(mbAppliedPhysics, L"RigidBody::SetTriggerShapeFlag() - 물리가 들어가지 않은 오브젝트에 대한 SetTriggerShapeFlag 호출");
		mShape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, _bFlag);
	}

	void RigidBody::SetActorInSceneFlag(bool _bFlag)
	{
		mbIsActorInScene = _bFlag;
	}

	void RigidBody::AddForce(const math::Vector3& _force)
	{
		AssertEx(mbAppliedPhysics, L"RigidBody::AddForce() - 물리가 들어가지 않은 오브젝트에 대한 AddForce 호출");
		AssertEx(eActorType::Static != mPhysicsInfo.eActorType, L"RigidBody::AddForce() - Static 객체에 대해 힘 적용");

		physx::PxVec3 force = physx::PxVec3(_force.x, _force.y, _force.z);

		physx::PxRigidBodyExt::addForceAtPos(*GetDynamicActor(), force,
			GetOwner()->GetComponent<Transform>()->GetPhysicalLocalPosition(),
			physx::PxForceMode::eFORCE
		);
	}

	void RigidBody::CreateBoxGeometry()
	{
		mPhysicsInfo.pGeometries = new Geometries(mPhysicsInfo.eGeomType, mPhysicsInfo.size);
	}

	void RigidBody::CreateCapsuleGeometry()
	{
		mPhysicsInfo.pGeometries = new Geometries(mPhysicsInfo.eGeomType, mPhysicsInfo.size.x, mPhysicsInfo.size.y);
	}

	void RigidBody::CreatePlaneGeometry()
	{
		mPhysicsInfo.pGeometries = new Geometries(mPhysicsInfo.eGeomType);
	}

	void RigidBody::CreateSphereGeometry()
	{
		mPhysicsInfo.pGeometries = new Geometries(mPhysicsInfo.eGeomType, mPhysicsInfo.size.x);
	}

	void RigidBody::CreateGeometry()
	{
		mPhysicsInfo.size /= 2.f;

		switch (mPhysicsInfo.eGeomType)
		{
		case eGeometryType::Box:
			CreateBoxGeometry();
			break;

		case eGeometryType::Capsule:
			CreateCapsuleGeometry();
			break;

		case eGeometryType::Sphere:
			CreateSphereGeometry();
			break;

		case eGeometryType::Plane:
			CreatePlaneGeometry();
			break;
		}

		AssertEx(mPhysicsInfo.pGeometries, L"RigidBody::CreateGeometry() - Geometry 생성 실패");
	}

	void RigidBody::CreateShape()
	{
		switch (mPhysicsInfo.eGeomType)
		{
		case eGeometryType::Box:
			mShape = physx::PxRigidActorExt::createExclusiveShape(*mActor->is<physx::PxRigidActor>(), mPhysicsInfo.pGeometries->boxGeom, *mMaterial);
			Application::GetPhysics()->SetupFiltering(mShape, (UINT)GetOwner()->GetLayerType());
			break;
		case eGeometryType::Capsule:
			mShape = physx::PxRigidActorExt::createExclusiveShape(*mActor->is<physx::PxRigidActor>(), mPhysicsInfo.pGeometries->capsuleGeom, *mMaterial);
			Application::GetPhysics()->SetupFiltering(mShape, (UINT)GetOwner()->GetLayerType());
			break;
		case eGeometryType::Sphere:
			mShape = physx::PxRigidActorExt::createExclusiveShape(*mActor->is<physx::PxRigidActor>(), mPhysicsInfo.pGeometries->sphereGeom, *mMaterial);
			Application::GetPhysics()->SetupFiltering(mShape, (UINT)GetOwner()->GetLayerType());
			break;
		case eGeometryType::Plane:
			mShape = physx::PxRigidActorExt::createExclusiveShape(*mActor->is<physx::PxRigidActor>(), mPhysicsInfo.pGeometries->planeGeom, *mMaterial);
			Application::GetPhysics()->SetupFiltering(mShape, (UINT)GetOwner()->GetLayerType());
			break;
		}

		AssertEx(mShape, L"RigidBody::CreateShape() - Shape 생성 실패");
	}

	void RigidBody::CreateActor()
	{
		switch (mPhysicsInfo.eActorType)
		{
		case eActorType::Dynamic:
			mActor = PHYSICS->createRigidDynamic(physx::PxTransform(GetOwner()->GetPosition()));
			break;

		case eActorType::Static:
			mActor = PHYSICS->createRigidStatic(physx::PxTransform(GetOwner()->GetPosition()));
			break;

		case eActorType::Kinematic:
			mActor = PHYSICS->createRigidDynamic(physx::PxTransform(GetOwner()->GetPosition()));
			mActor->is<physx::PxRigidDynamic>()->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, true);
			break;
		}

		AssertEx(mActor, L"RigidBody::CreateActor() - Actor 생성 실패");
	}

	void RigidBody::CreateMaterial()
	{
		mMaterial = PHYSICS->createMaterial(mPhysicsInfo.massProperties.staticFriction,
											mPhysicsInfo.massProperties.dynamicFriction,
											mPhysicsInfo.massProperties.restitution);
	}

	void RigidBody::InitializeActor()
	{
		physx::PxRigidActor* pActor = mActor->is<physx::PxRigidActor>();
		pActor->userData = GetOwner();

		mPhysicsInfo.filterData.word0 = 1 << static_cast<int>(GetOwner()->GetLayerType());

		std::bitset<LAYER_TYPE_COUNT> collisionGroup = CollisionManager::GetCollisionGroup(GetOwner()->GetLayerType());

		for (int i = 0; i < LAYER_TYPE_COUNT; ++i)
		{
			if (true == collisionGroup[i])
				mPhysicsInfo.filterData.word1 |= 1 << i;
		}

		mShape->setSimulationFilterData(mPhysicsInfo.filterData);
		physx::PxVec3 myPos = GetOwner()->GetPosition();
		pActor->setGlobalPose(physx::PxTransform(myPos));

		switch (mPhysicsInfo.eActorType)
		{
		case eActorType::Static:
			break;
		case eActorType::Dynamic:
			ApplyGravity();
			break;
		case eActorType::Kinematic:
			SetSimulationShapeFlag(false);
			SetTriggerShapeFlag(true);
			break;
		}
	}

	void RigidBody::CreateTriangleActors()
	{
		// Index
		int RenderinfoCount = 611;

		std::vector<physx::PxVec3> combinedVertices;
		std::vector<physx::PxU32> combinedIndices;

		// mVertexVec, mIndexVec의 값들을 
		for (size_t i = 0; i < RenderinfoCount; i++) {
			physx::PxU32 startIndex = static_cast<physx::PxU32>(combinedVertices.size());
			combinedVertices.insert(combinedVertices.end(), mVertexVec[i].begin(), mVertexVec[i].end());

			for (physx::PxU32 index : mIndexVec[i]) {
				combinedIndices.push_back(startIndex + index);
			}
		}

		// 결합된 정점 및 인덱스로 meshDesc를 설정합니다.
		physx::PxTriangleMeshDesc meshDesc;
		meshDesc.points.count = static_cast<physx::PxU32>(combinedVertices.size());
		meshDesc.points.stride = sizeof(physx::PxVec3);
		meshDesc.points.data = &combinedVertices[0];
		meshDesc.triangles.count = static_cast<physx::PxU32>(combinedIndices.size() / 3);
		meshDesc.triangles.stride = 3 * sizeof(physx::PxU32);
		meshDesc.triangles.data = &combinedIndices[0];

		physx::PxTolerancesScale scale;
		physx::PxCookingParams params(scale);
		params.meshWeldTolerance = 0.001f;
		params.meshPreprocessParams = physx::PxMeshPreprocessingFlags(physx::PxMeshPreprocessingFlag::eENABLE_VERT_MAPPING);
		params.suppressTriangleMeshRemapTable = true;

		physx::PxDefaultMemoryOutputStream writeBuffer;
		physx::PxTriangleMeshCookingResult::Enum result;
		bool status = PxCookTriangleMesh(params, meshDesc, writeBuffer, &result);
		if (!status) {
			AssertEx(status, L"매쉬를 불러와 피직스X 충돌체를 만드는데 실패했습니다 TriMesh");
		}

		physx::PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
		physx::PxTriangleMesh* TriangleMesh = PHYSICS->createTriangleMesh(readBuffer);
		physx::PxTriangleMeshGeometry geom = TriangleMesh;

		physx::PxRigidActor* rigidActor = mActor->is<physx::PxRigidActor>();
		mShape = physx::PxRigidActorExt::createExclusiveShape(*rigidActor, geom, *mMaterial);
	}

	void RigidBody::CustomFBXLoad(const std::string& _meshName, float _ratio, bool _inverseIndex)
	{
		//매쉬를 찾는다
		//std::shared_ptr<Mesh> FindFBXMesh = MeshData::GetMesh();
		//if (FindFBXMesh == nullptr)
		//{
		//	MsgAssert("해당 매쉬는 존재하지 않습니다. " + _MeshName);
		//}

		//Mesh = FindFBXMesh;

		////랜더유닛카운트를 불러와 백터에 reserve를 한다
		//size_t RenderinfoCount = Mesh->GetRenderUnitCount();

		//VertexVec.reserve(RenderinfoCount + 1);
		//IndexVec.reserve(RenderinfoCount + 1);

		//for (size_t i = 0; i < RenderinfoCount; i++)
		//{
		//	//i 번째 GetRenderUnit에서 RenderUnitInfo를 Get한다
		//	FbxRenderUnitInfo* RenderUnitInfo = Mesh->GetRenderUnit(i);

		//	std::vector<GameEngineVertex> MeshVertexs = RenderUnitInfo->Vertexs;
		//	std::vector<unsigned int> Indexes = RenderUnitInfo->Indexs[0];

		//	size_t VertexSize = MeshVertexs.size();
		//	size_t IndexSize = Indexes.size();
		//	std::vector<physx::PxVec3> InstVertVec;
		//	std::vector<unsigned int> InstIndexVec;
		//	//Vertex와 Index 정보를 VertexVec, IndexVec에 저장한다
		//	for (size_t j = 0; j < VertexSize; j++)
		//	{
		//		InstVertVec.push_back(physx::PxVec3(MeshVertexs[j].POSITION.x, MeshVertexs[j].POSITION.y, MeshVertexs[j].POSITION.z) * _Ratio);
		//	}

		//	if (_InverseIndex == true)
		//	{
		//		for (size_t j = 0; j < IndexSize; j++)
		//		{
		//			InstIndexVec.push_back(physx::PxU32(Indexes[j]));
		//		}
		//	}
		//	if (_InverseIndex == false)
		//	{
		//		for (size_t j = IndexSize - 1; j >= 0; --j)
		//		{
		//			InstIndexVec.push_back(physx::PxU32(Indexes[j]));
		//		}
		//	}
		//	VertexVec.push_back(InstVertVec);
		//	IndexVec.push_back(InstIndexVec);
		//}
	}
}
