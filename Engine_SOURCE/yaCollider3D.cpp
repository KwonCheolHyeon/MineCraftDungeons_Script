#include "yaCollider3D.h"
#include "yaResources.h"
#include "yaMesh.h"
#include "yaMaterial.h"
#include "yaRigidBody.h"
#include "yaGameObject.h"

namespace md
{
	Collider3D::Collider3D()
		: Collider(enums::eColliderType::None)
		, mCollisionCount(0)
	{
		MH_ASSERT(TRUE);
		mMesh = Resources::Find<Mesh>(L"CubeMesh");
		mMaterial = Resources::Find<graphics::Material>(L"DebugMaterial");
	}

	Collider3D::~Collider3D()
	{
	}
	void Collider3D::Initalize()
	{
		AssertEx(IsPhysicsObject(), L"Collider::Initialize() - 충돌을 사용하기 위해서는 RigidBody->SetPhysical()가 선행되어야 함.");
	}
	void Collider3D::Update()
	{
	}
	void Collider3D::FixedUpdate()
	{
		physx::PxShape* shape = GetOwner()->GetComponent<RigidBody>()->GetShape();
		eGeometryType geometryType = GetOwner()->GetComponent<RigidBody>()->GetGeometryType();
		mType = eColliderType::None;
		physx::PxVec3 scale = {};

		switch (geometryType)
		{
		case enums::eGeometryType::Capsule:
		case enums::eGeometryType::Box:
			scale = GetOwner()->GetComponent<RigidBody>()->GetGeometrySize();
			mType = eColliderType::Box;
			break;

		case enums::eGeometryType::Sphere:
			scale = math::Vector3(GetOwner()->GetComponent<RigidBody>()->GetGeometrySphereRadius());
			mType = eColliderType::Sphere;
			break;

		case enums::eGeometryType::Plane:
			scale = GetOwner()->GetComponent<RigidBody>()->GetGeometrySize();
			mType = eColliderType::Plane;
			break;

		case enums::eGeometryType::None:
			scale = GetOwner()->GetComponent<RigidBody>()->GetGeometrySize();
			mType = eColliderType::Triangle;
			break;
		}

		scale += physx::PxVec3(0.00001f);
		Vector3 position = GetOwner()->GetPhysicalWorldPosition();

		DebugMesh meshAttribute = {};
		meshAttribute.position = Vector3(position.x, position.y, position.z);
		meshAttribute.scale = scale;
		meshAttribute.type = mType;

		renderer::debugMeshes.push_back(meshAttribute);
		
	}
	void Collider3D::OnCollisionEnter(Collider3D* _otherCollider)
	{
		mCollisionCount++;

		const std::vector<Script*>& scripts = GetOwner()->GetScripts();
		for (auto script : scripts)
		{
			script->OnCollisionEnter(_otherCollider);
		}
	}
	void Collider3D::OnCollisionStay(Collider3D* _otherCollider)
	{
		const std::vector<Script*>& scripts = GetOwner()->GetScripts();
		for (auto script : scripts)
		{
			script->OnCollisionStay(_otherCollider);
		}
	}
	void Collider3D::OnCollisionExit(Collider3D* _otherCollider)
	{
		mCollisionCount--;
		if (0 > mCollisionCount)
			mCollisionCount = 0;

		const std::vector<Script*>& scripts = GetOwner()->GetScripts();
		for (auto script : scripts)
		{
			script->OnCollisionExit(_otherCollider);
		}
	}
	void Collider3D::OnTriggerEnter(Collider3D* _otherCollider)
	{
		mCollisionCount++;
	}
	void Collider3D::OnTriggerStay(Collider3D* _otherCollider)
	{
	}
	void Collider3D::OnTriggerExit(Collider3D* _otherCollider)
	{
		mCollisionCount--;

		if (0 > mCollisionCount)
			mCollisionCount = 0;
	}
	math::Vector3 Collider3D::ComputePenetration(GameObject* _gameObject)
	{
		eGeometryType eGeometryType = GetOwner()->GetComponent<RigidBody>()->GetGeometryType();
		if (!_gameObject->GetComponent<RigidBody>())
			return Vector3::Zero;

		switch (eGeometryType)
		{
			case eGeometryType::Box:
			{
				physx::PxBoxGeometry boxGeom = GetOwner()->GetComponent<RigidBody>()->GetGeometry()->boxGeom;
				physx::PxTransform otherTransform = _gameObject->GetComponent<RigidBody>()->GetActor<physx::PxRigidActor>()->getGlobalPose();
				bool bIsPenet = false;
				switch (_gameObject->GetComponent<RigidBody>()->GetGeometryType())
				{
					case eGeometryType::Box:
					{
						physx::PxBoxGeometry otherGeom = _gameObject->GetComponent<RigidBody>()->GetGeometry()->boxGeom;
						bIsPenet = physx::PxGeometryQuery::computePenetration(mPenetDir, mPenetDepth, boxGeom, GetOwner()->GetComponent<RigidBody>()->GetActor<physx::PxRigidActor>()->getGlobalPose(), otherGeom, otherTransform);
						if (bIsPenet)
							return mPenetDir * mPenetDepth;
					}
					break;
					case eGeometryType::Capsule:
					{
						physx::PxCapsuleGeometry otherGeom = _gameObject->GetComponent<RigidBody>()->GetGeometry()->capsuleGeom;
						bIsPenet = physx::PxGeometryQuery::computePenetration(mPenetDir, mPenetDepth, boxGeom, GetOwner()->GetComponent<RigidBody>()->GetActor<physx::PxRigidActor>()->getGlobalPose(), otherGeom, otherTransform);
						if (bIsPenet)
							return mPenetDir * mPenetDepth;
					}
					break;
					case eGeometryType::Sphere:
					{
						physx::PxSphereGeometry otherGeom = _gameObject->GetComponent<RigidBody>()->GetGeometry()->sphereGeom;
						bIsPenet = physx::PxGeometryQuery::computePenetration(mPenetDir, mPenetDepth, boxGeom, GetOwner()->GetComponent<RigidBody>()->GetActor<physx::PxRigidActor>()->getGlobalPose(), otherGeom, otherTransform);
						if (bIsPenet)
							return mPenetDir * mPenetDepth;
					}
					break;
				}
			}
			break;
			////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			case eGeometryType::Capsule:
			{
				physx::PxCapsuleGeometry capsuleGeom = GetOwner()->GetComponent<RigidBody>()->GetGeometry()->capsuleGeom;
				physx::PxTransform otherTransform = _gameObject->GetComponent<RigidBody>()->GetActor<physx::PxRigidActor>()->getGlobalPose();

				bool bIsPenet = false;
				switch (_gameObject->GetComponent<RigidBody>()->GetGeometryType())
				{
					case eGeometryType::Box:
					{
						physx::PxBoxGeometry otherGeom = _gameObject->GetComponent<RigidBody>()->GetGeometry()->boxGeom;
						bIsPenet = physx::PxGeometryQuery::computePenetration(mPenetDir, mPenetDepth, capsuleGeom, GetOwner()->GetComponent<RigidBody>()->GetActor<physx::PxRigidActor>()->getGlobalPose(), otherGeom, otherTransform);
						if (_gameObject->GetLayerType() == eLayerType::Monster)
							int a = 0;
						
						if (bIsPenet)
							return mPenetDir * mPenetDepth;
					}
					break;
					case eGeometryType::Capsule:
					{
						physx::PxCapsuleGeometry otherGeom = _gameObject->GetComponent<RigidBody>()->GetGeometry()->capsuleGeom;
						bIsPenet = physx::PxGeometryQuery::computePenetration(mPenetDir, mPenetDepth, capsuleGeom, GetOwner()->GetComponent<RigidBody>()->GetActor<physx::PxRigidActor>()->getGlobalPose(), otherGeom, otherTransform);
						if (bIsPenet)
							return mPenetDir * mPenetDepth;
					}
					break;
					case eGeometryType::Sphere:
					{
						physx::PxSphereGeometry otherGeom = _gameObject->GetComponent<RigidBody>()->GetGeometry()->sphereGeom;
						bIsPenet = physx::PxGeometryQuery::computePenetration(mPenetDir, mPenetDepth, capsuleGeom, GetOwner()->GetComponent<RigidBody>()->GetActor<physx::PxRigidActor>()->getGlobalPose(), otherGeom, otherTransform);
						if (bIsPenet)
							return mPenetDir * mPenetDepth;
					}
					break;
				}
			}
			break;
			////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

			case eGeometryType::Sphere:
			{
				physx::PxTransform otherTransform = _gameObject->GetComponent<RigidBody>()->GetActor<physx::PxRigidActor>()->getGlobalPose();
				physx::PxSphereGeometry sphereGeom = GetOwner()->GetComponent<RigidBody>()->GetGeometry()->sphereGeom;
				bool bIsPenet = false;
				switch (_gameObject->GetComponent<RigidBody>()->GetGeometryType())
				{
					case eGeometryType::Box:
					{
						physx::PxBoxGeometry otherGeom = _gameObject->GetComponent<RigidBody>()->GetGeometry()->boxGeom;
						bIsPenet = physx::PxGeometryQuery::computePenetration(mPenetDir, mPenetDepth, sphereGeom, GetOwner()->GetComponent<RigidBody>()->GetActor<physx::PxRigidActor>()->getGlobalPose(), otherGeom, otherTransform);
						if (bIsPenet)
							return mPenetDir * mPenetDepth;
					}
					break;
					case eGeometryType::Capsule:
					{
						physx::PxCapsuleGeometry otherGeom = _gameObject->GetComponent<RigidBody>()->GetGeometry()->capsuleGeom;
						bIsPenet = physx::PxGeometryQuery::computePenetration(mPenetDir, mPenetDepth, sphereGeom, GetOwner()->GetComponent<RigidBody>()->GetActor<physx::PxRigidActor>()->getGlobalPose(), otherGeom, otherTransform);
						if (bIsPenet)
							return mPenetDir * mPenetDepth;
					}
					break;
					case eGeometryType::Sphere:
					{
						physx::PxSphereGeometry otherGeom = _gameObject->GetComponent<RigidBody>()->GetGeometry()->sphereGeom;
						bIsPenet = physx::PxGeometryQuery::computePenetration(mPenetDir, mPenetDepth, sphereGeom, GetOwner()->GetComponent<RigidBody>()->GetActor<physx::PxRigidActor>()->getGlobalPose(), otherGeom, otherTransform);
						if (bIsPenet)
							return mPenetDir * mPenetDepth;
					}
					break;
				}
			}
			break;
		}
		return math::Vector3::Zero;
	}
}
