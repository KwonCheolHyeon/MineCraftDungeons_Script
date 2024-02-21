#include "mdUIEquipment.h"
#include "yaRenderer.h"
#include "yaBaseRenderer.h"
#include "yaMaterial.h"
#include "yaResources.h"
#include "yaTexture.h"
#include "yaInput.h"
namespace md 
{
	UIEquipment::UIEquipment()
		: mItmeType(0)
		, mPrevItemType(0)
	{
		mUITexture = Resources::Find<Texture>(L"EmptyEquipTexture");
		LoadUIEquipmentTexture();
	}

	UIEquipment::~UIEquipment()
	{
	}

	void UIEquipment::Initalize()
	{
		UIBase::Initalize();
	}

	void UIEquipment::Update()
	{
		UIBase::Update();
	}

	void UIEquipment::FixedUpdate()
	{
		UIBase::FixedUpdate();
	}

	void UIEquipment::Render()
	{
		GetComponent<BaseRenderer>()->GetMaterial(0)->SetTexture(eTextureSlot::Albedo, mUITexture);
		UIBase::Render();
	}

	void UIEquipment::SetItemType(int _type)
	{
		mItmeType = _type;
		mPrevItemType = mItmeType;


		if (mItmeType == 0) //장비칸 아무것도 없을 때
		{
			mUITexture = Resources::Find<Texture>(L"EmptyEquipTexture");
			SetScale(Vector3(0.15f, 0.15f, 1.f));
		}
		else if (mItmeType == 1)//화살 없을 때 
		{
			mUITexture = Resources::Find<Texture>(L"arrows_emptyTexture");
			SetScale(Vector3(0.42f, 0.4f, 1.f));
		}
		else if (mItmeType == 2) //화살 있을 때
		{
			mUITexture = Resources::Find<Texture>(L"arrowsTexture");
			SetScale(Vector3(0.42f, 0.4f, 1.f));
		}
		else if (mItmeType == 3)
		{

		}
	}

	void UIEquipment::LoadUIEquipmentTexture()
	{
		Resources::Load<Texture>(L"arrows_emptyTexture", L"UI\\game\\SmallInventory\\Arrow\\arrows_empt.png");
		Resources::Load<Texture>(L"arrowsTexture", L"UI\\game\\SmallInventory\\Arrow\\arrows.png");
	}

}
