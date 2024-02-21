#include "mdMouseObject.h"

#include "yaInput.h"
#include "mdUIInventory.h"
#include "yaCollisionManager.h"
#include "yaGameObject.h"
#include "mdUIButton.h"
#include "mdUIBase.h"
#include "yaApplication.h"

extern md::Application application;
namespace md 
{
	MouseObject::MouseObject()
	{
	}
	MouseObject::~MouseObject()
	{
	}
	void MouseObject::Initalize()
	{

		
		UIBase::Initalize();
	}

	void MouseObject::Update()
	{
		MousePosUpdate();
		UIAABBSizeCompare();
		switch (mMouseStatus)
		{
		case eUIMouseStatus::MSNormal:

			break;
		case eUIMouseStatus::MSHover:

			break;
		case eUIMouseStatus::MSAttack:

			break;
		default:
			break;
		}
		
		UIBase::Update();
	}

	void MouseObject::FixedUpdate()
	{
		UIBase::FixedUpdate();
	}

	void MouseObject::Render()
	{
		UIBase::Render();
	}

	void MouseObject::DetectUI(UIButton* _uiButton,int _index)
	{
		if (_index < 18) //만약 index가 0~17번일 때 버튼 행동
		{
			if (_uiButton->GetButtonState() != eUIButtonState::BSSelect)
			{
				_uiButton->SetButtonState(eUIButtonState::BSHover);

				if (Input::GetKeyDown(eKeyCode::LBTN))
				{
					for (int index = 0; index < 18; index++)
					{
						if (_index == index)
						{
							continue;
						}
						mButtonVector[index]->SetButtonState(eUIButtonState::BSDefault);
					}
					_uiButton->SetButtonState(eUIButtonState::BSSelect);
				}
				if (Input::GetKeyDown(eKeyCode::RBTN))
				{
					mInventory->ExchageItem(_uiButton, _index);
				}
			}
		}
		else if (_index < 23) //만약 index가 18~22번일 때 버튼 행동
		{
			if (_uiButton->GetButtonState() != eUIButtonState::BSSelect)
			{
				_uiButton->SetButtonState(eUIButtonState::BSHover);

				if (Input::GetKeyDown(eKeyCode::LBTN))
				{
					for (int index = 18; index < 23; index++)
					{
						if (_index == index)
						{
							continue;
						}
						mButtonVector[index]->SetButtonState(eUIButtonState::BSDefault);
					}
					_uiButton->SetButtonState(eUIButtonState::BSSelect);
					
					mInventory->CategoryArrangement(_index); //이부분이 잘못됨 아직 수정전 찾아보기
				}
			}
		}
		else if(_index == 23) // 나가기 버튼
		{
			if (_uiButton->GetButtonState() != eUIButtonState::BSSelect)
			{
				_uiButton->SetButtonState(eUIButtonState::BSHover);

				if (Input::GetKeyDown(eKeyCode::LBTN))
				{
					_uiButton->SetButtonState(eUIButtonState::BSSelect);
					mInventory->InventoryOnOff(false);//인벤토리 종료
					mInventory->InventoryRender(false);
				}
			}
		}
		
	}

	void MouseObject::UIAABBSizeCompare()
	{
		mButtonVector = mInventory->GetButtonsVector();
		for (int itemIndex = 0; itemIndex < 18; itemIndex++)
		{
			math::Vector4 uiAABB = mButtonVector[itemIndex]->GetUIAABBSize();
			if (GetPosition().x > uiAABB.x && GetPosition().x < uiAABB.y && GetPosition().y > uiAABB.z && GetPosition().y < uiAABB.w) 
			{
				DetectUI(mButtonVector[itemIndex], itemIndex);
			}
			else 
			{
				if (mButtonVector[itemIndex]->GetButtonState() != eUIButtonState::BSSelect) // 선택된걸 제외하고 
				{
					mButtonVector[itemIndex]->SetButtonState(eUIButtonState::BSDefault);
				}
			}
		}

		for (int categoryIndex = 18; categoryIndex < 23; categoryIndex++)
		{
			math::Vector4 uiAABB = mButtonVector[categoryIndex]->GetUIAABBSize();
			if (GetPosition().x > uiAABB.x && GetPosition().x < uiAABB.y && GetPosition().y > uiAABB.z && GetPosition().y < uiAABB.w)
			{
				DetectUI(mButtonVector[categoryIndex], categoryIndex);
			}
			else
			{
				if (mButtonVector[categoryIndex]->GetButtonState() != eUIButtonState::BSSelect) // 선택된걸 제외하고 
				{
					mButtonVector[categoryIndex]->SetButtonState(eUIButtonState::BSDefault);
				}
			}
		}

		{

			math::Vector4 uiAABBExit = mButtonVector[23]->GetUIAABBSize();
			if (GetPosition().x > uiAABBExit.x && GetPosition().x < uiAABBExit.y && GetPosition().y > uiAABBExit.z && GetPosition().y < uiAABBExit.w)
			{
				DetectUI(mButtonVector[23], 23);
			}
			else 
			{
				mButtonVector[23]->SetButtonState(eUIButtonState::BSDefault);
			}

		}

	}

	void MouseObject::MousePosUpdate()
	{
		Vector2 mousePos = Input::GetMouseInGamePosition();
		SetPositionXY(mousePos);

		PreProcessTransformFixedUpdate();
	}

	void MouseObject::PreProcessTransformFixedUpdate()
	{
		Vector2 mousePos = GetPositionXY();
		float posZ = GetPositionZ();

		mousePos.x *= (posZ + 0.1f);
		mousePos.y *= posZ * application.GetScreenRatio();
		SetPositionXY(mousePos);
	}

}
