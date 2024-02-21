#pragma once
#include "mdUIBase.h"
#include "mdUIButton.h"
namespace md 
{
	class UIInventory :
		public UIBase
	{
	public:
		UIInventory();
		virtual ~UIInventory();

		void Initalize() override;
		void Update() override;
		void FixedUpdate() override;
		void Render() override;

	public:
		void SetButtons(UIButton *_buttonUI) { mButtonVector.push_back(_buttonUI); }//0~5 = 장비 버튼,, 6~17 인벤토리 버튼,, 18~22 카테고리,, 23은 나가기 버튼
		std::vector<UIButton*> GetButtonsVector() { return mButtonVector; }
		int GetButtonsHowMany() { return static_cast<int>(mButtonVector.size()); }//버튼이 지금 몇개이니?
		void InventoryOnOff(bool _on);

		void CategoryArrangement(int _categoryNum);
		void CategoryVectorSet(int _categoryNum); 
		void CategoryVectorReset(int _categoryNum); 
		void CategoryVectorArrangement(int _categoryNum);
		void ExchageItem(UIButton *_buttonUI,int _index);
		void InitItemVector();

		void InventoryRender(bool _on) { bInventoryRender = _on; }
		bool GetInventoryRender() { return bInventoryRender; }

		
		//현재 생각 6~17 아이템 타입을 받아서 

	private:
		int mNowCategoryNum;
		bool bInventoryRender;
	private:
		std::vector<UIButton*> mButtonVector;
		std::vector<int> mCategoryAll; // 18
		std::vector<int> mCopyCategoryAll; // 18
		std::vector<int> mCategoryMeel; // 19
		std::vector<int> mCategoryArmor;// 20
		std::vector<int> mCategoryBow; // 21
		std::vector<int> mCategoryItem; // 22
		


	};
}


