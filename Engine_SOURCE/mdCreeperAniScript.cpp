#include "mdCreeperAniScript.h"
#include "mdCreeperScript.h"
#include "yaInput.h"
#include "yaGameObject.h"
#include "yaMeshData.h"
#include "mdCreeperScript.h"

#include "yaBoneAnimator.h"

namespace md
{
	CreeperAniScript::CreeperAniScript()
		: Script()
		, mAniClip(0)
	{
	
	}
	CreeperAniScript::~CreeperAniScript()
	{
	}
	void CreeperAniScript::Initalize()
	{
		BoneAnimator* boneAni = GetOwner()->GetComponent<BoneAnimator>();
		boneAni->AddAnimationClip(L"Stun", 0, 40);						// 1
		boneAni->AddAnimationClip(L"Walk", 41, 49);						// 2
		boneAni->AddAnimationClip(L"Novelty", 50, 149);					// 3
		boneAni->AddAnimationClip(L"Ignition", 150, 249);				// 4
		boneAni->AddAnimationClip(L"Idle", 250, 262);					// 5
		boneAni->AddAnimationClip(L"GetHitFromTheFront", 306, 316);		// 6
		boneAni->AddAnimationClip(L"BasicAttack", 317, 326);			// 7

		ChangeAnimation(5, true);
		boneAni->GetCompleteEventRef(L"GetHitFromTheFront") = std::bind(&CreeperAniScript::Complete, this);
	}
	void CreeperAniScript::Update()
	{
		eCreeperState creeperState = GetOwner()->GetScript<CreeperScript>()->GetCreeperState();
		switch (creeperState)
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
	void CreeperAniScript::Render()
	{
	}
	void CreeperAniScript::Start()
	{
	}
	void CreeperAniScript::Action()
	{

	}
	void CreeperAniScript::Complete()
	{
		if (GetOwner()->GetScript<CreeperScript>()->GetCreeperState() == eCreeperState::Hit)
		{
			GetOwner()->GetScript<CreeperScript>()->SetCreeperState(eCreeperState::Walk);
		}
	}
	void CreeperAniScript::End()
	{

	}
	void CreeperAniScript::ChangeAnimation(UINT _aniClip, bool _loop)
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
	void CreeperAniScript::Idle()
	{
		ChangeAnimation(5, true);
	}
	void CreeperAniScript::Walk()
	{
		ChangeAnimation(2, true);
	}
	void CreeperAniScript::Explosion()
	{
		ChangeAnimation(7, true);
	}
	void CreeperAniScript::Stun()
	{
		ChangeAnimation(1, true);
	}

	void CreeperAniScript::Hit()
	{
		ChangeAnimation(6, true);
	}

	void CreeperAniScript::Die()
	{
	}
	
}
