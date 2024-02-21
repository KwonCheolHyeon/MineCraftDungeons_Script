#include "yaMonster.h"
#include "yaMeshRenderer.h"
#include "yaTransform.h"
#include "yaBoneAnimator.h"


namespace md
{
	Monster::Monster()
		: mCurrentMonsterState (eMonsterState::Idle)
		, mPrevMonsterState (eMonsterState::Idle)
		, mHp (1)
	{
		CantTransState();
	}

	Monster::~Monster()
	{
	}

	void Monster::Initalize()
	{
		// Component 추가
		//AddComponent<BoneAnimator>();
		//AddComponent<MeshRenderer>();
	}

	void Monster::Update()
	{ 
		// 현재 상태의 로직 호출
		if (mStateLogic[mCurrentMonsterState])
			mStateLogic[mCurrentMonsterState]();

		GameObject::Update();
	}

	void Monster::FixedUpdate()
	{
		GameObject::FixedUpdate();
	}

	void Monster::Render()
	{
		GameObject::Render();
	}

	void Monster::ChangeMonsterState(eMonsterState _newState, bool _loop)
	{
		auto transitionKey = std::make_pair(mCurrentMonsterState, _newState);

		// 만약 해당 상태 전이가 mStateTransitionTable에 있고 false라면, 상태 변경을 허용하지 않음
		// 테이블에 없거나 true일 경우 상태 변경
		if (mStateTransitionTable.find(transitionKey) != mStateTransitionTable.end() && !mStateTransitionTable[transitionKey])
		{
			return;
		}
		
		//ChangeMonsterAnimation(_newState, _loop);
		mCurrentMonsterState = _newState;
	}

	void Monster::BindStateAndLogic()
	{
		// 상태와 상태관련된 로직함수를 bind해줌 
		//mStateLogic[eMonsterState::Idle] = std::bind(&Monster::Idle, this);
		//mStateLogic[eMonsterState::Walk] = std::bind(&Monster::Walk, this);
		//mStateLogic[eMonsterState::Attack] = std::bind(&Monster::Attack, this);
		//mStateLogic[eMonsterState::Sturn] = std::bind(&Monster::Sturn, this);
		//mStateLogic[eMonsterState::Hit] = std::bind(&Monster::Hit, this);
		//mStateLogic[eMonsterState::Die] = std::bind(&Monster::Die, this);
	}

	void Monster::CantTransState()
	{
		// 상태 전이 테이블 설정 - 특정 상태 False 지정시 다른 특정 상태로 전환 불가능
		mStateTransitionTable[{eMonsterState::Die, eMonsterState::Idle}] = false;
		mStateTransitionTable[{eMonsterState::Die, eMonsterState::Walk}] = false;
		mStateTransitionTable[{eMonsterState::Die, eMonsterState::Attack}] = false;
		mStateTransitionTable[{eMonsterState::Die, eMonsterState::Hit}] = false;
		mStateTransitionTable[{eMonsterState::Die, eMonsterState::Die}] = false;
	}
}
