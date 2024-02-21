#include "mdUIButton.h"
#include "mdUIButton.h"
#include "yaMaterial.h"
#include "yaBaseRenderer.h"
#include "yaResources.h"
#include "yaTexture.h"
#include "yaInput.h"

namespace md
{
	UIButton::UIButton()
		:mButtonState(eUIButtonState::BSDefault)
		, mEquipmentStatus(eUIEquipmentStatus::NoEquipment)
		, mHowManyImage(3)
		, mButtonType(0)
		, mEquipmentOrInventory(0)
		, mItemType(0)
		, bRender (false)
	{
	}
	UIButton::~UIButton()
	{
	}
	void UIButton::Initalize()
	{
		
		mInventoryTexture = Resources::Find<Texture>(L"EmptyTexture");

		UIBase::Initalize();
	}
	void UIButton::Update()
	{

		
		UIBase::Update();
	}
	void UIButton::FixedUpdate()
	{
		UIBase::FixedUpdate();
	}
	void UIButton::Render()
	{
		if (bRender == true) 
		{
		
			ChangeButtonImage(mButtonType, mEquipmentOrInventory, mItemType);

			BaseRenderer* progressBarBaseRender = GetComponent<BaseRenderer>();
			if (progressBarBaseRender != nullptr)
			{
				std::shared_ptr<Material> progressBarMaterial = progressBarBaseRender->GetMaterial(0);
				if (progressBarMaterial != nullptr)
				{
					progressBarMaterial->SetData(eGPUParam::Int_0, &mHowManyImage);
				}
			}

			UIBase::Render();
		}
		
	}

	void UIButton::SetButtonImage(int _buttoinType, int _equipmentOrInventory, int _itemType)
	{
		mButtonType = _buttoinType;
		mEquipmentOrInventory = _equipmentOrInventory;
		mItemType = _itemType;
	}

	void UIButton::ChangeButtonImage(int _buttonState, int _equipmentOrInventory, int _itemType)
	{
		if (_buttonState == 0)//인벤토리 
		{
			switch (mButtonState)//버튼 상태
			{
			case eUIButtonState::BSDefault:

				mInventoryTexture = Resources::Find<Texture>(L"EmptyTexture");
				

				break;
			case eUIButtonState::BSSelect:

				mInventoryTexture = Resources::Find<Texture>(L"InventorySelectTexture");
			
				break;
			case eUIButtonState::BSHover:

				mInventoryTexture = Resources::Find<Texture>(L"InventoryHoverTexture");
				
				break;
			default:
				break;
			}
			GetComponent<BaseRenderer>()->GetMaterial(0)->SetTexture(eTextureSlot::Albedo, mInventoryTexture);
			SetItemImage(_buttonState, _itemType);
		}
		else if (_buttonState == 1)//플레이어 근처 장비창
		{
		
			switch (mButtonState)//버튼 상태
			{
			case eUIButtonState::BSDefault:

				mInventoryTexture = Resources::Find<Texture>(L"EmptyTexture");


				break;
			case eUIButtonState::BSSelect:

				mInventoryTexture = Resources::Find<Texture>(L"InventorySelectTexture");

				break;
			case eUIButtonState::BSHover:

				mInventoryTexture = Resources::Find<Texture>(L"InventoryHoverTexture");

				break;
			default:
				break;
			}
			GetComponent<BaseRenderer>()->GetMaterial(0)->SetTexture(eTextureSlot::Albedo, mInventoryTexture);
			SetItemImage(_buttonState, _itemType);
		}
		else if (_buttonState == 2)//카테고리
		{
		
			switch (mButtonState)//버튼 상태
			{
			case eUIButtonState::BSDefault:
				if (_itemType == 0)//전체
				{
					mInventoryTexture = Resources::Find<Texture>(L"FilterAllDefaultTexture");
				}
				else if (_itemType == 1)//무기
				{
					mInventoryTexture = Resources::Find<Texture>(L"FilterMeleeDefaultTexture");
				}
				else if (_itemType == 2)//방어구
				{
					mInventoryTexture = Resources::Find<Texture>(L"FilterArmourDefaultTexture");
				}
				else if (_itemType == 3)//활
				{
					mInventoryTexture = Resources::Find<Texture>(L"FilterRangedDefaultTexture");
				}
				else if (_itemType == 4) // 아이템
				{
					mInventoryTexture = Resources::Find<Texture>(L"FilterConsumeDefaultTexture");
				}

				break;
			case eUIButtonState::BSSelect:
				if (_itemType == 0)//전체
				{
					mInventoryTexture = Resources::Find<Texture>(L"FilterAllSelectTexture");
				}
				else if (_itemType == 1)//무기
				{
					mInventoryTexture = Resources::Find<Texture>(L"FilterMeleeSelectTexture");
				}
				else if (_itemType == 2)//방어구
				{
					mInventoryTexture = Resources::Find<Texture>(L"FilterArmourSelectTexture");
				}
				else if (_itemType == 3)//활
				{
					mInventoryTexture = Resources::Find<Texture>(L"FilterRangedSelectTexture");
				}
				else if (_itemType == 4) // 아이템
				{
					mInventoryTexture = Resources::Find<Texture>(L"FilterConsumeSelectTexture");
				}
				break;
			case eUIButtonState::BSHover:
				if (_itemType == 0)//전체
				{
					mInventoryTexture = Resources::Find<Texture>(L"FilterAllSelectTexture");
				}
				else if (_itemType == 1)//무기
				{
					mInventoryTexture = Resources::Find<Texture>(L"FilterMeleeSelectTexture");
				}
				else if (_itemType == 2)//방어구
				{
					mInventoryTexture = Resources::Find<Texture>(L"FilterArmourSelectTexture");
				}
				else if (_itemType == 3)//활
				{
					mInventoryTexture = Resources::Find<Texture>(L"FilterRangedSelectTexture");
				}
				else if (_itemType == 4) // 아이템
				{
					mInventoryTexture = Resources::Find<Texture>(L"FilterConsumeSelectTexture");
				}
				break;
			default:
				break;
			}

			GetComponent<BaseRenderer>()->GetMaterial(0)->SetTexture(eTextureSlot::Albedo, mInventoryTexture);

		}
		else if (_buttonState == 3) //나가기 버튼
		{
		
			switch (mButtonState)//버튼 상태
			{
			case eUIButtonState::BSDefault:

				mInventoryTexture = Resources::Find<Texture>(L"InventoryCloseNormalTexture");

				break;
			case eUIButtonState::BSSelect:

				mInventoryTexture = Resources::Find<Texture>(L"InventoryCloseActiveTexture");

				break;
			case eUIButtonState::BSHover:

				mInventoryTexture = Resources::Find<Texture>(L"InventoryCloseHoverTexture");

				break;
			default:
				break;
			}

			GetComponent<BaseRenderer>()->GetMaterial(0)->SetTexture(eTextureSlot::Albedo, mInventoryTexture);
		}
		else 
		{
			AssertEx(true, L"SetButtonType, _buttoinState이 잘못됨");
		}
	}

	void UIButton::SetItemImage(int _buttonState, int _itemtype)
	{
		int equipmentOrInventory = 0;

		if (_buttonState == 1) 
		{
			equipmentOrInventory = 1;
		}

		if (_itemtype == 0) 
		{
			mEquipmentStatus = eUIEquipmentStatus::NoEquipment;
		}
		else 
		{
			mEquipmentStatus = eUIEquipmentStatus::HasEquipment;
		}

		switch (mEquipmentStatus)
		{
		case eUIEquipmentStatus::NoEquipment:
			if (equipmentOrInventory == 0)//장비창
			{

				mInventoryTexture = Resources::Find <Texture> (L"InventoryEquipSlotTexture");
				GetComponent<BaseRenderer>()->GetMaterial(0)->SetTexture(eTextureSlot::Normal, mInventoryTexture);
				

				mInventoryTexture = Resources::Find<Texture>(L"gear_slot_empty_iconTexture");
				GetComponent<BaseRenderer>()->GetMaterial(0)->SetTexture(eTextureSlot::PositionTarget, mInventoryTexture);
			}
			else if (equipmentOrInventory == 1)//인벤토리
			{

				mInventoryTexture = Resources::Find<Texture>(L"InventoryBoxEmptyTexture");
				GetComponent<BaseRenderer>()->GetMaterial(0)->SetTexture(eTextureSlot::Normal, mInventoryTexture);

				mInventoryTexture = Resources::Find<Texture>(L"EmptyTexture");
				GetComponent<BaseRenderer>()->GetMaterial(0)->SetTexture(eTextureSlot::PositionTarget, mInventoryTexture);
			
			}
			break;
		case eUIEquipmentStatus::HasEquipment:
			if (equipmentOrInventory == 0)//장비창
			{
				mInventoryTexture = Resources::Find<Texture>(L"InventoryEquipSlotTexture");
				GetComponent<BaseRenderer>()->GetMaterial(0)->SetTexture(eTextureSlot::Normal, mInventoryTexture);

				if (_itemtype == 1)//칼
				{
					mInventoryTexture = Resources::Find<Texture>(L"SwordInventoryTexture");
				}
				else if (_itemtype == 2) // 아머 
				{

					mInventoryTexture = Resources::Find<Texture>(L"AromorInventoryTexture");
				}
				else if (_itemtype == 3) //활
				{
					mInventoryTexture = Resources::Find<Texture>(L"BowInventoryTexture");
				}
				
				GetComponent<BaseRenderer>()->GetMaterial(0)->SetTexture(eTextureSlot::PositionTarget, mInventoryTexture);
			}
			else if (equipmentOrInventory == 1)//인벤토리 칸
			{
				mInventoryTexture = Resources::Find<Texture>(L"InventoryEquipSlotTexture");
				GetComponent<BaseRenderer>()->GetMaterial(0)->SetTexture(eTextureSlot::Normal, mInventoryTexture);

				if (_itemtype == 1)//칼
				{
					mInventoryTexture = Resources::Find<Texture>(L"SwordInventoryTexture");
				}
				else if (_itemtype == 2) // 아머 
				{
					mInventoryTexture = Resources::Find<Texture>(L"AromorInventoryTexture");
				}
				else if (_itemtype == 3) //활
				{
				
					mInventoryTexture = Resources::Find<Texture>(L"BowInventoryTexture");
				}
				GetComponent<BaseRenderer>()->GetMaterial(0)->SetTexture(eTextureSlot::PositionTarget, mInventoryTexture);
			}
			break;
		default:
			break;
		}
		
		
	}

	



}
