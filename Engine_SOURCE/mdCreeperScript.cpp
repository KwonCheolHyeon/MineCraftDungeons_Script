#include "mdCreeperScript.h"
#include "yaInput.h"
#include "yaGameObject.h"
#include "yaMeshData.h"

#include "yaRigidBody.h"
#include "yaBoneAnimator.h"
#include "yaCollider3D.h"
#include "mdDamageScript.h"
#include "yaSceneManager.h"

#include "mdExplosionEffect.h"
#include "mdExplosionEfScript.h"
#include "yaObject.h"
#include "yaGameObject.h"
#include "yaMeshRenderer.h"
#include "yaRenderer.h"

#include "yaBaseRenderer.h"
#include "yaTime.h"

namespace md
{
	CreeperScript::CreeperScript()
		: Script()
		, mCreeperState(eCreeperState::Idle)
		, mTr(nullptr)
		, mRb(nullptr)
		, mTarget(nullptr)
		, mMoveSpeed(1.0f)
		, mdOnCollision(false)
		, mExplosionCheck(false)
		, mIsFlash(0)
		, mElpasedTime(0.0f)
		, mFlashDuration(2.5f)
	{
	}
	CreeperScript::~CreeperScript()
	{
	}
	void CreeperScript::Initalize()
	{
		MeshData* meshData = MeshData::LoadFromFbx(L"fbx\\Creeper.fbx");
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
		GetOwner()->SetPhysicalLocalPosition(Vector3(0.0f, 150.0f, 0.0f));
		GetOwner()->GetTransform()->SetPhysicalRotation(math::Vector3(0.0f, 0.0f, 90.0f));

		GetOwner()->SetScale(math::Vector3(0.0006f, 0.0006f, 0.0006f));

		GetOwner()->AddComponent<Collider3D>();
		GetOwner()->AddComponent<DamageScript>();
		mTr = GetOwner()->GetComponent<Transform>();
		mRb = GetOwner()->GetComponent<RigidBody>();

	}
	void CreeperScript::Update()
	{
		mElpasedTime += Time::DeltaTime();

		if (Input::GetKey(eKeyCode::H))
		{
			mTr->SetPositionY(0.0f);
		}

		switch (mCreeperState)
		{
		case md::eCreeperState::Idle:
			Idle();
			break;

		case md::eCreeperState::Walk:
			Walk();
			break;

		case md::eCreeperState::Explosion:
			Explosion();
			break;

		case md::eCreeperState::Stun:
			Stun();
			break;

		case md::eCreeperState::Hit:
			Hit();
			break;

		case md::eCreeperState::Die:
			Die();
			break;

		default:
			break;
		}

	}
	void CreeperScript::Render()
	{
		BaseRenderer* flashBaseRenderer = GetOwner()->GetComponent<BaseRenderer>();

		if (flashBaseRenderer != nullptr)
		{
			std::shared_ptr<Material> flashMaterial = flashBaseRenderer->GetMaterial(0);
			if (flashMaterial != nullptr)
			{
				flashMaterial->SetData(eGPUParam::Int_0, &mIsFlash);
				flashMaterial->SetData(eGPUParam::Float_0, &mFlashDuration);
				flashMaterial->SetData(eGPUParam::Float_1, &mElpasedTime);

			}
		}
	}
	void CreeperScript::OnCollisionEnter(Collider3D* collider)
	{
		if (collider->GetOwner()->GetName() == L"Player")
		{
			math::Vector3 stop = math::Vector3(0.f, mTr->GetPhysicalLocalPosition().y, 0.f);
			mRb->SetVelocity(stop);

			mdOnCollision = true;
			mCreeperState = eCreeperState::Explosion;
		}
	}
	void CreeperScript::OnCollisionStay(Collider3D* collider)
	{
	}
	void CreeperScript::OnCollisionExit(Collider3D* collider)
	{
		if (collider->GetOwner()->GetName() == L"Player")
		{
			mdOnCollision = false;
		}
	}
	void CreeperScript::Start()
	{
	}
	void CreeperScript::Action()
	{
	}
	void CreeperScript::End()
	{
	}
	void CreeperScript::Idle()
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
			mCreeperState = eCreeperState::Walk;
		}

		if (distance >= 20.f)
		{
			mCreeperState = eCreeperState::Idle;
		}

	}
	void CreeperScript::Walk()
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
		if (distance > 30.f)
		{
			mCreeperState = eCreeperState::Idle;
		}
	}

	void CreeperScript::Explosion()
	{
		if (mExplosionCheck == false)
		{
			mElpasedTime = 0.0f;
			mIsFlash = 1;
			mExplosionCheck = true;
		}

		// 일정시간이 지나면 폭발 Effect(이쪽에서 공격되도록 설정해야함)가 뜨고 Creeper 죽음처리 
		if (mElpasedTime >= mFlashDuration && mIsFlash == 1)
		{
			ExplosionEffect* explosionObj = object::Instantiate<ExplosionEffect>(eLayerType::Monster);
			explosionObj->SetPosition(mTr->GetPhysicalLocalPosition());
			explosionObj->AddComponent<ExplosionEfScript>();

			GetOwner()->Death();
			mIsFlash = 0;
		}
		
	}

	void CreeperScript::Stun()
	{
	}
	void CreeperScript::Hit()
	{
	}
	void CreeperScript::Die()
	{
	}
	void CreeperScript::RotateTowardsTarget(math::Vector3 _pos, math::Vector3 _targetPos)
	{
		float distance = math::Vector3::CalculateDistance(_pos, _targetPos);
		math::Vector3 direction = math::Vector3::CalculateDirection(_targetPos, _pos);
		float yawRadians = atan2(direction.x, direction.z);
		float yawDegrees = yawRadians * (180.0f / XM_PI);

		mTr->SetPhysicalRotation(math::Vector3(0.f, yawDegrees, 90.f));
	}
}
