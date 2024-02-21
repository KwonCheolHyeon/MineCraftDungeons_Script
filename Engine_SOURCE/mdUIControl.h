#pragma once
#include "mdUIBase.h"

namespace md 
{

	class UIControl : public UIBase
	{
	public:
		UIControl();
		virtual ~UIControl();

		void Initalize() override;
		void Update() override;
		void FixedUpdate() override;
		void Render() override;

	public:
		void SetInventory(class UIInventory* _inventory) { mInventory = _inventory; }
		void SetProgressBar(class ProgressBar* _progressBar) { mProgressVector.push_back(_progressBar); } // 0. 하트,  1. 쿨타임, 2. 구르기
		void SetDamegeFrame(class UIAlphaObject* _damageBar) { mDamageBar = _damageBar; } // 0 데미지 프레임

	private:
		void InventoryOnOff();
		void ProgressBarOn();
		void HpControl(float _hp);
		void SlowMoveHp(float _prevHp, float _nowHp);

	private:
		 UIInventory* mInventory;
		 std::vector<ProgressBar*> mProgressVector;
		 UIAlphaObject* mDamageBar;

	private:
		float mPrevHp;
		float mNowHp;
		float mHpTime;



	};
}


