#pragma once
#include "yaScript.h"

namespace md
{
	class CreeperAniScript : public Script
	{
	public:
		CreeperAniScript();
		~CreeperAniScript();

		virtual void Initalize() override;
		virtual void Update() override;
		virtual void Render() override;

		void Start();
		void Action();
		void Complete();
		void End();

		void Idle();
		void Walk();
		void Explosion();
		void Stun();
		void Hit();
		void Die();

		void ChangeAnimation(UINT _aniClip, bool _loop);

	private:
		UINT mAniClip;
	};
}
