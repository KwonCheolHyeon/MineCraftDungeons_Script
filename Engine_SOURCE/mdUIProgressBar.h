#pragma once
#include "mdUIBase.h"
#include "UIEnums.h"

namespace md 
{
	class ProgressBar :
		public UIBase
	{
	public:
		ProgressBar();
		virtual ~ProgressBar();

		void Initalize() override;
		void Update() override;
		void FixedUpdate() override;
		void Render() override;

	public:
		void SetType(int _type) 
		{
			mType = _type;
		}
	public://HP 관련
		void SetPlayerHp(int _playerHp)
		{
			mPlayerHp = static_cast<float>(_playerHp);
		}
		int GetPlayerHp() const
		{
			return static_cast<int>(mPlayerHp);
		}

	public://쿨타임 (아이템 이나 구르기)
		void CoolTime();

		bool IsEnable() 
		{
			return bEnable;
		}
		void SetDisable() { bEnable = false; }
		//void SetEnable() { bEnable = true; }

		void SetCoolTimeNumber(float _coolTime) // 쿨타임 디폴트 1.5초 아이템인 경우 다를 수 있음
		{
			mCoolTime = _coolTime;
		}

	private://무슨 타입 인가? ex)hp인가 쿨타임 인가
		int mType;

	private://HP 관련
		float mPlayerHp;//현재 hp
		float mPlayerMaxHp; //최대 hp
		int mPrevPlayerHp; // hp의 변경을 확인 하기 위한 
		float mHeartFrameAlpha;//데미지 받았을 때 프레임 값

	private://쿨타임 관련
		float mCoolTime;//쿨타임
		float mNowTime;//현재 시간
		bool bEnable;

	private: // 최종 시간
		float mResult;


	private://fsm구조 
		eProgressBarState mProgressType;

	};
}


