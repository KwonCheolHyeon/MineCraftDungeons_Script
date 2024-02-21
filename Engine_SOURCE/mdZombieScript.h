#pragma once
#include "yaScript.h"


namespace md
{
	enum class eZombieState
	{
		None,
		Idle,
		Walk,
		Stun,
		NoveltySleep,
		BasicAttack,
		GetHitFromTheFront,
	};

	class Transform;
	class RigidBody;
	class ZombieScript : public Script
	{
	public:


	public:
		ZombieScript();
		~ZombieScript();

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
		void Stun();
		void NoveltySleep();
		void BasicAttack();
		void GetHitFromTheFront();

		void Attack();

		// Target Object 방향으로 바라보도록 회전 시키는 함수
		void RotateTowardsTarget(math::Vector3 _pos, math::Vector3 _targetPos);
		//bool IsBInFrontOfA(const Game)

		eZombieState GetZombieState() { return mState; }
		void		 SetZombieState(eZombieState _eState) { mState = _eState; }

		void SetTarget(GameObject* _object) { mTarget = _object; }

		bool IsOnCollision() { return mdOnCollision; }

	private:
		eZombieState mState;
		GameObject* mTarget;
		Transform* mTr;
		RigidBody* mRb;
		float mMoveSpeed;

		bool mdOnCollision;
	};
}
