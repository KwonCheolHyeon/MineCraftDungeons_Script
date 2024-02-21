#include "mdZombieAniScript.h"
#include "yaGameObject.h"
#include "yaInput.h"
#include "yaTime.h"
#include "yaMeshData.h"

#include "yaBoneAnimator.h"
#include "yaCollider3D.h"

#include "mdZombieScript.h"

namespace md
{
	ZombieAniScript::ZombieAniScript()
		: Script()
		, mAniClip(0)
	{
	}
	ZombieAniScript::~ZombieAniScript()
	{
	}
	void ZombieAniScript::Initalize()
	{
		ZombieScript* scr = GetOwner()->GetScript<ZombieScript>();

		BoneAnimator* boneAni = GetOwner()->GetComponent<BoneAnimator>();
		boneAni->AddAnimationClip(L"Idle", 0, 79);
		boneAni->AddAnimationClip(L"Walk", 81, 120);
		boneAni->AddAnimationClip(L"Stun", 122, 170);
		boneAni->AddAnimationClip(L"NoveltySleep", 171, 257);
		boneAni->AddAnimationClip(L"BasicAttack", 259, 300);
		boneAni->AddAnimationClip(L"GetHitFromTheFront", 302, 348);

		ChangeAnimation(4, true);
		boneAni->GetCompleteEventRef(L"GetHitFromTheFront") = std::bind(&ZombieAniScript::Complete, this);
		boneAni->GetEventRef(L"BasicAttack",15) = std::bind(&ZombieScript::Attack, scr);
	}
	void ZombieAniScript::Update()
	{

		eZombieState state = GetOwner()->GetScript<ZombieScript>()->GetZombieState();
		switch (state)
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
	void ZombieAniScript::Render()
	{
	}
	void ZombieAniScript::Start()
	{
	}
	void ZombieAniScript::Action()
	{
		
	}
	void ZombieAniScript::Complete()
	{
		if (GetOwner()->GetScript<ZombieScript>()->GetZombieState() == eZombieState::GetHitFromTheFront)
		{
			GetOwner()->GetScript<ZombieScript>()->SetZombieState(eZombieState::Walk);
		}
	}
	void ZombieAniScript::End()
	{

	}
	void ZombieAniScript::ChangeAnimation(UINT _aniClip, bool _loop)
	{
		// 한번 실행된 애니메이션은 다시 실행하지 않는다.
		if (mAniClip != _aniClip)
		{
			BoneAnimator* boneAni = GetOwner()->GetComponent<BoneAnimator>();
			boneAni->ChangeAnimation(_aniClip);
			boneAni->SetAnimationLoop(_aniClip, _loop);
			mAniClip = _aniClip;
		}
	}
	void ZombieAniScript::Idle()
	{
		ChangeAnimation(1, true);
	}
	void ZombieAniScript::Walk()
	{
		ChangeAnimation(2, true);
	}
	void ZombieAniScript::Stun()
	{
		ChangeAnimation(3, true);
	}
	void ZombieAniScript::NoveltySleep()
	{
		ChangeAnimation(4, true);
	}
	void ZombieAniScript::BasicAttack()
	{
		ChangeAnimation(5, false);
	}
	void ZombieAniScript::GetHitFromTheFront()
	{
		ChangeAnimation(6, false);
	}
}
