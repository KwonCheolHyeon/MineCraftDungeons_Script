#include "mdPlayerHitScript.h"
#include "yaInput.h"
#include "yaGameObject.h"
#include "yaBaseRenderer.h"
#include "yaTime.h"

namespace md
{
	PlayerHitScript::PlayerHitScript()
		: Script()
		, mIsHit(0)
		, mHitDuration(0.3f)
		, mElpasedTime(0.0f)
	{
	}
	PlayerHitScript::~PlayerHitScript()
	{
	}
	void PlayerHitScript::Initalize()
	{
	}
	void PlayerHitScript::Update()
	{

		mElpasedTime += Time::DeltaTime();

		// 추후에 Hit 상태로 진입했을 때 mIsHit값 1로 전환해서 반짝이도록 변경해야함
		if (Input::GetKey(eKeyCode::U) && mIsHit == 0)
		{
			mIsHit = 1;
			mElpasedTime = 0.0f;
		}

		if (mIsHit == 1 && mElpasedTime > mHitDuration)
		{
			mIsHit = 0;
		}

	}
	void PlayerHitScript::Render()
	{
		BaseRenderer* hitEffectRenderer = GetOwner()->GetComponent<BaseRenderer>();
		if (hitEffectRenderer != nullptr)
		{
			std::shared_ptr<Material> hitEffectMaterial = hitEffectRenderer->GetMaterial(0);
			if (hitEffectMaterial != nullptr)
			{
				hitEffectMaterial->SetData(eGPUParam::Int_1, &mIsHit);
				hitEffectMaterial->SetData(eGPUParam::Float_2, &mHitDuration);
				hitEffectMaterial->SetData(eGPUParam::Float_3, &mElpasedTime);

			}
		}
	}
	void PlayerHitScript::OnCollisionEnter(Collider3D* collider)
	{
	}
	void PlayerHitScript::OnCollisionStay(Collider3D* collider)
	{
	}
	void PlayerHitScript::OnCollisionExit(Collider3D* collider)
	{
	}
	void PlayerHitScript::Start()
	{
	}
	void PlayerHitScript::Action()
	{
	}
	void PlayerHitScript::End()
	{
	}
}
