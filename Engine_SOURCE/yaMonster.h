#pragma once
#include "yaGameObject.h"

namespace md
{
	class Transform;
	class BoneAnimator;

	class Monster : public GameObject
	{
	public:
		enum class eMonsterState
		{
			None,
			Idle,
			Walk,
			Attack,
			Sturn,
			Hit,
			Die,
		};

	public:
		Monster();
		virtual ~Monster();

		virtual void Initalize();
		virtual void Update();
		virtual void FixedUpdate();
		virtual void Render();

		//State
		virtual void Idle() = 0;
		virtual void Walk() = 0;
		virtual void Attack() = 0;
		virtual void Sturn() = 0;
		virtual void Hit() = 0;
		virtual void Die() = 0;

	public:
		virtual void ChangeMonsterState(eMonsterState _newState, bool _loop);
		virtual eMonsterState GetMonsterState() { return mCurrentMonsterState; }

		virtual void ChangeMonsterAnimation(eMonsterState _newState, bool _loop) = 0;

		virtual void BindStateAndLogic();
		virtual void CantTransState();

	private:
		eMonsterState mPrevMonsterState;
		eMonsterState mCurrentMonsterState;

	protected:
		std::map<eMonsterState, std::function<void()>> mStateLogic;

	private:
		std::map<std::pair<eMonsterState, eMonsterState>, bool> mStateTransitionTable;
		int mHp;

	};
}
