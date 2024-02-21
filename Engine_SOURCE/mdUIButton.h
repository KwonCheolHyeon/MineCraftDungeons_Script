   #pragma once
#include "mdUIBase.h"
#include "UIEnums.h"
#include "yaTexture.h"
namespace md 
{
	class UIButton :
		public UIBase
	{
	public:
		UIButton();
		virtual ~UIButton();

		void Initalize() override;
		void Update() override;
		void FixedUpdate() override;
		void Render() override;
	
		void SetButtonImage(int _buttoinType, int _equipmentOrInventory, int _itemType);
		void SetButtonState(eUIButtonState _buttonState) { mButtonState = _buttonState; }//버튼이 호버인지 선택인지 아니면 노멀인지
		eUIButtonState GetButtonState() { return mButtonState; }//버튼이 호버인지 선택인지 아니면 노멀인지
		eUIEquipmentStatus GetEquipState() { return mEquipmentStatus; }//버튼이 호버인지 선택인지 아니면 노멀인지

		void SetHowManyImage(int _type) { mHowManyImage = _type; }//버튼이 0 = 인벤토리,, 1 = 장비창,, 2 = 카테고리,, 3 = exit

		int GetButtonType() { return mButtonType; }
		int GetItemType() { return mItemType; }

		void SetButtonRender(bool _on) { bRender = _on; }

	private:
		void ChangeButtonImage(int _buttoinState, int _equipmentOrInventory, int _itemType);//첫번째 인수 어느버튼인지 두번째 인수 장비가 있는지 인벤토리 인지 세번째 아이템 타입
		void SetItemImage(int _equipmentOrInventory,int _itemtype);

	private:
		int mHowManyImage;
		int mButtonType;
		int mItemType;
		int mEquipmentOrInventory;
		bool bRender;
	private:
		std::shared_ptr<md::graphics::Texture> mInventoryTexture;
		//std::shared_ptr<Texture> mButton2;
	private:
		eUIButtonState mButtonState;//버튼의 상태 (기본, 호버, 선택됨)
		eUIEquipmentStatus mEquipmentStatus;// 버튼의 장비 상태 (없음, 있음)
	};
}


