#include "mdUIAlphaObject.h"
#include "yaRenderer.h"
#include "yaBaseRenderer.h"
#include "yaMaterial.h"
#include "yaInput.h"
#include "yaTime.h"

namespace md 
{

	UIAlphaObject::UIAlphaObject()
		: mType(2)
		, bEnable(true)
		, mObjectAlpha(0.0f)
		, mTime(0.0f)
		, mAOState(eUIAlphaObjectState::AODefault)
		, mHeartAlphaTimer(2.0f)
		, mHeartAlpha(0.f)
	{
	}

	UIAlphaObject::~UIAlphaObject()
	{
	}

	void UIAlphaObject::Initalize()
	{



		UIBase::Initalize();
	}

	void UIAlphaObject::Update()
	{


		

		switch (mAOState)
		{
		case eUIAlphaObjectState::AOEnable:

			break;
		case eUIAlphaObjectState::AODisable:

			break;
		case eUIAlphaObjectState::AODefault:
			if (mType == 2)
			{
				if (mHeartAlpha > 0.f)
				{
					CloseAlpha();
				}
			}
			break;
		default:
			break;
		}


		

		


		UIBase::Update();
	}

	void UIAlphaObject::FixedUpdate()
	{



		UIBase::FixedUpdate();
	}

	void UIAlphaObject::Render()
	{

		BaseRenderer* progressBarBaseRender = GetComponent<BaseRenderer>();
		if (progressBarBaseRender != nullptr)
		{
			std::shared_ptr<Material> progressBarMaterial = progressBarBaseRender->GetMaterial(0);
			if (progressBarMaterial != nullptr)
			{
				progressBarMaterial->SetData(eGPUParam::Int_0, &mType);
				progressBarMaterial->SetData(eGPUParam::Float_2, &mObjectAlpha);
			}
		}


		UIBase::Render();
	}

	void UIAlphaObject::CloseAlpha()
	{
		mTime += Time::DeltaTime();

		float test = mTime / 1.0f;

		float alpha = mHeartAlpha - test;

		mObjectAlpha = alpha;

		if (0.f >= alpha)
		{
			mTime = 0;
			mHeartAlpha = 0;
			mObjectAlpha = 0;
		}
	}


}

