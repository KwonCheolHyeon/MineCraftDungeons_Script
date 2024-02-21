#pragma once
#include "yaScript.h"

namespace md
{
	enum class ePlayerState
	{
		WalkKatanaState,//1
		WalkState, // 2
		SwordCombo1State, // 3
		SwordCombo2State,//4
		SwordCombo3State,//5
		StunState,//6
		RunKatanaState,//7
		RunState,//8
		IdleState,//9
		IdleKatanaState,//10
		GetHitFrontState,//11
		GenericEquipState,//12
		EatFastState,//13
		DrinkState,//14
		DodgeRollSideWayRightState,//15
		DodgeRollSideWayLeftState,//16
		DodgeRollFordwardState,//17
		DodgeRollBackWardState,//18
		DodgeRollState,//19
		PunchState,//20
	};

	class PlayerScript : public Script
	{
	public:
		PlayerScript();
		~PlayerScript();

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
		ePlayerState& GetPlayerState() { return mState; }  
		void		    SetPlayerState(ePlayerState _eState) { mState = _eState; } // enum형
		int				GetPlayerStateInt() { return mPlayerState; }  //int형
	
	private:
		void WalkKatana();//1
		void Walk();//2
		void SwordCombo1();//3
		void SwordCombo2();//4
		void SwordCombo3();//5
		void Stun();//6
		void RunKatana();//7
		void Run();//8
		void Idle();//9
		void IdleKatana();//10
		void GetHitFront();//11
		void GenericEquip();//12
		void EatFast();//13
		void Drink();//14
		void DodgeRollSideWayRight();//15
		void DodgeRollSideWayLeft();//16
		void DodgeRollFordward();//17
		void DodgeRollBackWard();//18
		void DodgeRoll();//19
		void Punch();//20

	private:
		ePlayerState mState;					//플레이어 상태
		int mPlayerState;						//플레이어 상태를 int로 보냄
		float mMoveSpeed; //플레이어 움직임 속도

	};
}
