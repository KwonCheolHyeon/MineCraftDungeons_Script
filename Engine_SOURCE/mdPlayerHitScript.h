#pragma once
#include "yaScript.h"

namespace md
{
	class PlayerHitScript : public Script
	{
	public:
		PlayerHitScript();
		~PlayerHitScript();

		virtual void Initalize() override;
		virtual void Update() override;
		virtual void Render() override;

		virtual void OnCollisionEnter(Collider3D* collider) override;
		virtual void OnCollisionStay(Collider3D* collider) override;
		virtual void OnCollisionExit(Collider3D* collider) override;

		void Start();
		void Action();
		void End();

	private:
		int mIsHit;
		float mHitDuration;
		float mElpasedTime;
	};
}
