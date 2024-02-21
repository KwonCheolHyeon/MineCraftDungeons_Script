#include "mdZombieScript.h"
#include "yaInput.h"
#include "yaGameObject.h"
#include "yaMeshData.h"
#include "yaBaseRenderer.h"
#include "yaTime.h"

#include "yaBoneAnimator.h"
#include "yaRigidBody.h"
#include "yaCollider3D.h"

#include "mdDamageScript.h"
#include "yaSceneManager.h"

namespace md
{
	ZombieScript::ZombieScript()
		: Script()
		, mState(eZombieState::Idle)
		, mTr(nullptr)
		, mRb(nullptr)
		, mTarget(nullptr)
		, mMoveSpeed(3.0f)
		, mdOnCollision(false)
	{

	}
	ZombieScript::~ZombieScript()
	{
	}
	void ZombieScript::Initalize()
	{
		MeshData* meshData = MeshData::LoadFromFbx(L"fbx\\Zombie.fbx");
		if (meshData)
		{
			GetOwner()->SettingFromMeshData(meshData);
		}

		// PhysX 초기 설정을 위한 구조체
		tPhysicsInfo info = {};
		info.eActorType = eActorType::Dynamic;
		info.eGeomType = eGeometryType::Capsule;
		info.size = math::Vector3(5.0f, 10.0f, 5.0f);

		// rigidBody를 추가후 SetPhysical을 해줘야 PhysX 적용
		RigidBody* rigid = GetOwner()->AddComponent<RigidBody>();
		rigid->SetPhysical(info);
		//rigid->SetMassForDynamic(100.0f);
		rigid->SetFreezeRotation(FreezeRotationFlag::ROTATION_Y, true);
		rigid->SetFreezeRotation(FreezeRotationFlag::ROTATION_X, true);
		rigid->SetFreezeRotation(FreezeRotationFlag::ROTATION_Z, true);
		// SetPhysical() 이후 오브젝트의 위치를 설정해줘야한다(동기화때문)
		GetOwner()->SetPhysicalLocalPosition(Vector3(15.0f, 150.0f, 15.0f));
		GetOwner()->GetTransform()->SetPhysicalRotation(math::Vector3(0.0f, 0.0f, 90.0f));

		GetOwner()->AddComponent<Collider3D>();
		GetOwner()->AddComponent<DamageScript>();
		mTr = GetOwner()->GetComponent<Transform>();
		mRb = GetOwner()->GetComponent<RigidBody>();
	}
	void ZombieScript::Update()
	{
		if (Input::GetKey(eKeyCode::H))
		{
			mTr->SetPositionY(0.0f);
		}

		switch (mState)
		{
		case eZombieState::Idle:
			Idle();
			break;

		case eZombieState::Walk:
			Walk();
			break;

		case eZombieState::Stun:
			Stun();
			break;

		case eZombieState::NoveltySleep:
			NoveltySleep();
			break;

		case eZombieState::BasicAttack:
			BasicAttack();
			break;

		case eZombieState::GetHitFromTheFront:
			GetHitFromTheFront();
			break;

		default:
			break;
		}
	}
	void ZombieScript::Render()
	{

	}
	void ZombieScript::OnCollisionEnter(Collider3D* collider)
	{
		if (collider->GetOwner()->GetName() == L"Player")
		{
			math::Vector3 stop = math::Vector3(0.f, mTr->GetPhysicalLocalPosition().y, 0.f);
			mRb->SetVelocity(stop);

			mdOnCollision = true;
			mState = eZombieState::BasicAttack;
		}
	}
	void ZombieScript::OnCollisionStay(Collider3D* collider)
	{
	}
	void ZombieScript::OnCollisionExit(Collider3D* collider)
	{
		if (collider->GetOwner()->GetName() == L"Player")
		{
			mdOnCollision = false;
		}
	}
	void ZombieScript::Start()
	{
	}
	void ZombieScript::Action()
	{
	}
	void ZombieScript::End()
	{
	}
	void ZombieScript::Idle()
	{
		if (!mTarget)
		{
			mTarget = SceneManager::FindPlayerObject();
			if (!mTarget)
			{
				return;
			}
		}

		// 이동 정지
		math::Vector3 stop = math::Vector3(0.f, mTr->GetPhysicalLocalPosition().y, 0.f);
		mRb->SetVelocity(stop);
		
		// Target Object와 가까워지면 Walk로 상태 변경
		float distance = math::Vector3::CalculateDistance(mTr->GetPhysicalLocalPosition(), mTarget->GetPhysicalLocalPosition());
		if (distance <= 10.f)
		{
			mState = eZombieState::Walk;
		}

		if (distance >= 20.f)
		{
			mState = eZombieState::NoveltySleep;
		}
	}
	void ZombieScript::Walk()
	{
		if (!mTarget)
		{
			mTarget = SceneManager::FindPlayerObject();
			if (!mTarget)
			{
				return;
			}
		}

		RotateTowardsTarget(mTr->GetPhysicalLocalPosition(), mTarget->GetPhysicalLocalPosition());

		math::Vector3 targetPos = mTarget->GetPhysicalLocalPosition();
		math::Vector3 localPos = mTr->GetPhysicalLocalPosition();
		math::Vector3 moveDir = targetPos - localPos;
		moveDir.Normalize();

		math::Vector3 nextMoveTargetPos = moveDir * mMoveSpeed;
		mRb->SetVelocity(nextMoveTargetPos);

		// Target Object와 멀어지면 Idle로 상태 변경
		float distance = math::Vector3::CalculateDistance(mTr->GetPhysicalLocalPosition(), mTarget->GetPhysicalLocalPosition());
		if (distance > 10.f)
		{
			mState = eZombieState::Idle;
		}
	}
	
	void ZombieScript::Stun()
	{
	}
	
	void ZombieScript::NoveltySleep()
	{
		// Target Object와 가까워지면 Idleh로 상태 변경
		float distance = math::Vector3::CalculateDistance(mTr->GetPhysicalLocalPosition(), mTarget->GetPhysicalLocalPosition());
		if (distance < 20.f)
		{
			mState = eZombieState::Idle;
		}
	}
	
	void ZombieScript::BasicAttack()
	{
		if(!mdOnCollision)
			mState = eZombieState::Walk;
	}
	void ZombieScript::GetHitFromTheFront()
	{

	}

	void ZombieScript::Attack()
	{
		int a = 0;
	}

	void ZombieScript::RotateTowardsTarget(math::Vector3 _pos, math::Vector3 _targetPos)
	{
		float distance = math::Vector3::CalculateDistance(_pos, _targetPos);
		math::Vector3 direction = math::Vector3::CalculateDirection(_targetPos, _pos);
		float yawRadians = atan2(direction.x, direction.z);
		float yawDegrees = yawRadians * (180.0f / XM_PI);

		mTr->SetPhysicalRotation(math::Vector3(0.f, yawDegrees, 90.f));
	}
}
