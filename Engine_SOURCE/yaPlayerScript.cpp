#include "yaPlayerScript.h"
#include "yaTransform.h"
#include "yaGameObject.h"
#include "yaInput.h"
#include "yaTime.h"
#include "yaAnimator.h"
#include "yaRigidBody.h"
#include "yaCollisionManager.h"
#include "yaRenderer.h"

#include "yaMeshData.h"
#include "mdDamageScript.h"
#include "yaBoneAnimator.h"

#include "yaFbxLoader.h"

namespace md
{
	PlayerScript::PlayerScript()
		: Script()
		, mMoveSpeed(10.0f)
		, mPlayerState(0)
	{
	}

	PlayerScript::~PlayerScript()
	{
	}

	void PlayerScript::Initalize()
	{
		//MeshData* meshData = MeshData::LoadFromFbx(L"fbx\\PlayerPrototype.fbx"); // 플레이어 파일

		//if (meshData)
		//{
		//	GetOwner()->SettingFromMeshData(meshData);
		//}

		// PhysX 초기 설정을 위한 구조체
		tPhysicsInfo info = {};
		info.eActorType = eActorType::Dynamic;
		info.eGeomType = eGeometryType::Capsule;
		info.size = math::Vector3(5.0f, 10.0f, 5.0f);

		RigidBody* rigid = GetOwner()->AddComponent<RigidBody>();
		// rigidBody를 추가후 SetPhysical을 해줘야 PhysX 적용
		rigid->SetPhysical(info);
		rigid->SetMassForDynamic(100.0f);
		rigid->SetFreezeRotation(FreezeRotationFlag::ROTATION_Y, true);
		rigid->SetFreezeRotation(FreezeRotationFlag::ROTATION_X, true);
		rigid->SetFreezeRotation(FreezeRotationFlag::ROTATION_Z, true);
		// SetPhysical() 이후 오브젝트의 위치를 설정해줘야한다(동기화때문)
		GetOwner()->SetPhysicalLocalPosition(Vector3(0.0f, 80.0f, 0.0f));
		GetOwner()->GetTransform()->SetPhysicalRotation(math::Vector3(0.0f, 0.0f, 90.0f));


		
		//ChangeAnimation(4, true);

		GetOwner()->AddComponent<Collider3D>();
		//GetOwner()->AddComponent<DamageScript>();
	}

	void PlayerScript::Update()
	{
		Transform* tr = GetOwner()->GetComponent<Transform>();
		RigidBody* rb = GetOwner()->GetComponent<RigidBody>();

		if (Input::GetKey(eKeyCode::H))
		{
			tr->SetPosition(math::Vector3(0.0f, 0.0f, 0.f));
		}

		if (Input::GetKey(eKeyCode::LBTN))
		{
			//CollisionManager::DrawRaycast(GetOwner()->GetPosition(), math::Vector3(0.0f, -1.0f, 0.0f), 200.0f);

			// Raycast 호출하여 다음 이동 방향을 구함		
			math::Vector3 mouseHitPos = CollisionManager::GetMouseRayHitPos(GetOwner()->GetLayerType(), tr->GetPosition());
			math::Vector3 localPos = tr->GetPhysicalLocalPosition();
			math::Vector3 moveDir = mouseHitPos - localPos;
			moveDir.Normalize();
			 
			math::Vector3 nextMoveTargetPos = moveDir * mMoveSpeed;
			rb->SetVelocity(nextMoveTargetPos); 
		}

		if (Input::GetKey(eKeyCode::SPACE))
		{
			rb->SetVelocity(math::Vector3::Zero);
		}


		if (Input::GetKeyDown(eKeyCode::P))
		{
			if (mPlayerState == 20)
			{
				mPlayerState = 0;
			}
	
			mState = static_cast<ePlayerState>(mPlayerState);
			
		}

		switch (mState)
		{
		case md::ePlayerState::WalkKatanaState:
			WalkKatana();
			mPlayerState = 1;
			break;
		case md::ePlayerState::WalkState:
			mPlayerState = 2;
			break;
		case md::ePlayerState::SwordCombo1State:
			mPlayerState = 3;
			break;
		case md::ePlayerState::SwordCombo2State:
			mPlayerState = 4;
			break;
		case md::ePlayerState::SwordCombo3State:
			mPlayerState = 5;
			break;
		case md::ePlayerState::StunState:
			mPlayerState = 6;
			break;
		case md::ePlayerState::RunKatanaState:
			mPlayerState = 7; 
			break;
		case md::ePlayerState::RunState:
			mPlayerState = 8;
			break;
		case md::ePlayerState::IdleState:
			mPlayerState = 9;
			break;
		case md::ePlayerState::IdleKatanaState:
			mPlayerState = 10;
			break;
		case md::ePlayerState::GetHitFrontState: 
			mPlayerState = 11;
			break;
		case md::ePlayerState::GenericEquipState:
			mPlayerState = 12;
			break;
		case md::ePlayerState::EatFastState:
			mPlayerState = 13;
			break;
		case md::ePlayerState::DrinkState:
			mPlayerState = 14;
			break;
		case md::ePlayerState::DodgeRollSideWayRightState:
			mPlayerState = 15;
			break;
		case md::ePlayerState::DodgeRollSideWayLeftState:
			mPlayerState = 16;
			break;
		case md::ePlayerState::DodgeRollFordwardState:
			mPlayerState = 17;
			break;
		case md::ePlayerState::DodgeRollBackWardState:
			mPlayerState = 18;
			break;
		case md::ePlayerState::DodgeRollState:
			mPlayerState = 19;
			break;
		case md::ePlayerState::PunchState:
			mPlayerState = 20;
			break;
		default:
			break;
		}




	}

	void PlayerScript::Render()
	{

	}

	void PlayerScript::OnCollisionEnter(Collider3D* collider)
	{

	}

	void PlayerScript::OnCollisionStay(Collider3D* collider)
	{
	}

	void PlayerScript::OnCollisionExit(Collider3D* collider)
	{
	}

	void PlayerScript::Start()
	{
	}

	void PlayerScript::Action()
	{
	}

	void PlayerScript::End()
	{
	}

	void PlayerScript::WalkKatana()
	{
	}

	void PlayerScript::Walk()
	{
	}

	void PlayerScript::SwordCombo1()
	{
	}

	void PlayerScript::SwordCombo2()
	{
	}

	void PlayerScript::SwordCombo3()
	{
	}

	void PlayerScript::Stun()
	{
	}

	void PlayerScript::RunKatana()
	{
	}

	void PlayerScript::Run()
	{
	}

	void PlayerScript::Idle()
	{
	}

	void PlayerScript::IdleKatana()
	{
	}

	void PlayerScript::GetHitFront()
	{
	}

	void PlayerScript::GenericEquip()
	{
	}

	void PlayerScript::EatFast()
	{
	}

	void PlayerScript::Drink()
	{
	}

	void PlayerScript::DodgeRollSideWayRight()
	{
	}

	void PlayerScript::DodgeRollSideWayLeft()
	{
	}

	void PlayerScript::DodgeRollFordward()
	{
	}

	void PlayerScript::DodgeRollBackWard()
	{
	}

	void PlayerScript::DodgeRoll()
	{
	}

	void PlayerScript::Punch()
	{
	}

}
