#include "mdUIProgressBar.h"
#include "yaRenderer.h"
#include "yaBaseRenderer.h"
#include "yaMaterial.h"
#include "yaInput.h"
#include "yaTime.h"
#include "yaFontWrapper.h"

namespace md 
{
	ProgressBar::ProgressBar()
		: mType(0)
		, mPlayerHp(0)
		, mPlayerMaxHp(100)
		, mPrevPlayerHp(0)
		, mHeartFrameAlpha(0.0f)
		, mCoolTime(5.5f)
		, mNowTime(0.0f)
		, bEnable(true)
		, mResult(0.0f)
		, mProgressType(eProgressBarState::Default)
	{


	}

	ProgressBar::~ProgressBar()
	{
	}

	void ProgressBar::Initalize()
	{
		

		UIBase::Initalize();
	}

	void ProgressBar::Update()
	{

		switch (mProgressType)
		{
		case eProgressBarState::Hp:
			mResult = mPlayerHp / mPlayerMaxHp;
			mProgressType = eProgressBarState::Default;
			break; 
		case eProgressBarState::CoolTime:
			CoolTime();
			break;
		case eProgressBarState::Default:

			if (mType == 0) 
			{
				if (mPrevPlayerHp != mPlayerHp)
				{
					mPrevPlayerHp = static_cast<int>(mPlayerHp);
					mProgressType = eProgressBarState::Hp;
				}
			}
			else
			{
				if (bEnable == false)
				{
					mProgressType = eProgressBarState::CoolTime;

				}
				else 
				{
					if (mType == 1) 
					{
						mResult = 0.f;
					}
					else if (mType == 3)
					{
						mResult = 1.f;
					}
				}
			}
			break;
		default:
			break;
		}

		UIBase::Update();
	}

	void ProgressBar::FixedUpdate()
	{

		UIBase::FixedUpdate();
	}

	void ProgressBar::Render()
	{
		

		BaseRenderer* progressBarBaseRender = GetComponent<BaseRenderer>();
		if (progressBarBaseRender != nullptr)
		{
			std::shared_ptr<Material> progressBarMaterial = progressBarBaseRender->GetMaterial(0);
			if (progressBarMaterial != nullptr)
			{
				if (mType == 0) 
				{
					progressBarMaterial->SetData(eGPUParam::Int_0, &mType);
					progressBarMaterial->SetData(eGPUParam::Float_0, &mResult);
				}
				else 
				{
					progressBarMaterial->SetData(eGPUParam::Int_0, &mType);
					progressBarMaterial->SetData(eGPUParam::Float_1, &mResult);
				}
				
			}
		}

		UIBase::Render();

		Color testColor;
		testColor.R(255.f);
		testColor.G(255.f);
		testColor.B(255.f);
		testColor.A(1.f);

		const wchar_t* result = L"Arial, 에어리얼, 0";
		const wchar_t* result2 = L"Agency, 에이젠시, 1";
		const wchar_t* result3 = L"Godo B, 고도 B, 2";
		const wchar_t* result4 = L"DungGeunMo, 둥근모꼴, 3";
		FontWrapper::DrawFont(result, 900.f, 230.f, 30.f, testColor.RGBA().v, 0);
		FontWrapper::DrawFont(result2, 900.f, 330.f, 30.f, testColor.RGBA().v, 1);
		FontWrapper::DrawFont(result3, 900.f, 430.f, 30.f, testColor.RGBA().v, 2);
		FontWrapper::DrawFont(result4, 900.f, 530.f, 30.f, testColor.RGBA().v, 3);
	}



	void ProgressBar::CoolTime()
	{
		mNowTime += Time::DeltaTime(); 

		mResult = mNowTime / mCoolTime;

		if (mNowTime >= mCoolTime)
		{
			mProgressType = eProgressBarState::Default;
			mNowTime = 0.0f;
			bEnable = true;

			
			mResult = 0.f;
		}

		
	}




}
