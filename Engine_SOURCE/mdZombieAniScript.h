#pragma once
#include "yaScript.h"


namespace md
{
	class ZombieAniScript : public Script
	{
	public:
		ZombieAniScript();
		~ZombieAniScript();

		virtual void Initalize() override;
		virtual void Update() override;
		virtual void Render() override;

		virtual void OnCollisionEnter(Collider3D* collider) override {};
		virtual void OnCollisionStay(Collider3D* collider) override {};
		virtual void OnCollisionExit(Collider3D* collider) override {};

		virtual void Start() override;
		virtual void Action() override;
		virtual void Complete() override;
		virtual void End() override;

		void ChangeAnimation(UINT _aniClip, bool _loop);

	public:
		void Idle();
		void Walk();
		void Stun();
		void NoveltySleep();
		void BasicAttack();
		void GetHitFromTheFront();


	private:
		UINT mAniClip;
	};
}

