#pragma once
#include "yaScript.h"


namespace md
{
	enum class eCreeperState
	{
		None,
		Idle,
		Walk,
		Explosion,
		Stun,
		Hit,
		Die,
	};

	class Transform;
	class RigidBody;
	class CreeperScript : public Script
	{

	public:
		CreeperScript();
		~CreeperScript();

		virtual void Initalize() override;
		virtual void Update() override;
		virtual void Render() override;

		virtual void OnCollisionEnter(Collider3D* collider) override;
		virtual void OnCollisionStay(Collider3D* collider) override;
		virtual void OnCollisionExit(Collider3D* collider) override;

		virtual void Start() override;
		virtual void Action() override;
		virtual void End() override;

	public:
		void Idle();
		void Walk();
		void Explosion();
		void Stun();
		void Hit();
		void Die();

		// Target Object 방향으로 바라보도록 회전 시키는 함수
		void RotateTowardsTarget(math::Vector3 _pos, math::Vector3 _targetPos);
		//bool IsBInFrontOfA(const Game)

		eCreeperState GetCreeperState() { return mCreeperState; }
		void		  SetCreeperState(eCreeperState _eState) { mCreeperState = _eState; }

		void SetTarget(GameObject* _object) { mTarget = _object; }

		bool IsOnCollision() { return mdOnCollision; }

	private:
		eCreeperState mCreeperState;
		GameObject* mTarget;
		Transform* mTr;
		RigidBody* mRb;
		float mMoveSpeed;

		bool mdOnCollision;
		bool mExplosionCheck;
		bool mIsComplete;

		int mIsFlash;
		float mFlashDuration;
		float mElpasedTime;

	
	};
}
