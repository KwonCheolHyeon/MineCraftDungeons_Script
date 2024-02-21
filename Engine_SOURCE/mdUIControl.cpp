#include "mdUIControl.h"

#include "yaInput.h"
#include "mdUIInventory.h"
#include "mdUIProgressBar.h"
#include "mdUIAlphaObject.h"
#include "yaTime.h"
namespace md
{
	
	UIControl::UIControl()
		: mNowHp(100.f)
		, mPrevHp(100.f)
		, mHpTime(0.f)
		
	{
	}

	UIControl::~UIControl()
	{
	}

	void UIControl::Initalize()
	{

		UIBase::Initalize();
	}

	void UIControl::Update()
	{
		InventoryOnOff();
		ProgressBarOn();


		if (Input::GetKeyDown(eKeyCode::B)) 
		{
			HpControl(10.f);
		}
		if (Input::GetKeyDown(eKeyCode::A))
		{
			HpControl(-10.f);
		}


		UIBase::Update();
	}

	void UIControl::FixedUpdate()
	{

		UIBase::FixedUpdate();
	}

	void UIControl::Render()
	{

		UIBase::Render();
	}

	void UIControl::InventoryOnOff()
	{
		
		if (Input::GetKeyDown(eKeyCode::I)) 
		{
			if(mInventory->GetInventoryRender() == true)
			{
				mInventory->InventoryOnOff(false);//false == off
				mInventory->InventoryRender(false);
			}
			else 
			{
				mInventory->InventoryOnOff(true);//true == on
				mInventory->InventoryRender(true);
			}
		}


	}

	void UIControl::ProgressBarOn()
	{
		if (Input::GetKeyDown(eKeyCode::SPACE)) 
		{
			mProgressVector[2]->SetDisable();
		}

		if (Input::GetKeyDown(eKeyCode::E))
		{
			mProgressVector[1]->SetDisable();
		}

	}

	void UIControl::HpControl(float _hp)
	{
		mNowHp = mNowHp - _hp;
		if (mPrevHp > mNowHp) //hp회복
		{
			mProgressVector[0]->SetPlayerHp(static_cast<int> (mNowHp));
			

			mPrevHp = mNowHp;
		}
		else //hp감소
		{
			mProgressVector[0]->SetPlayerHp(static_cast<int> (mNowHp));
			mDamageBar->SetAlphaTimeZero();
			mDamageBar->SetHeartFrameAlpha(1.0f);
			mPrevHp = mNowHp;
		}


	}


}
