#include "mdUIInventory.h"


namespace md 
{
	UIInventory::UIInventory()
		: mNowCategoryNum(18)
		, bInventoryRender(false)
	{
	}

	UIInventory::~UIInventory()
	{
	}

	void UIInventory::Initalize()
	{
		

		UIBase::Initalize();
	}

	void UIInventory::Update()
	{
		

		UIBase::Update();
	}

	void UIInventory::FixedUpdate()
	{

		UIBase::FixedUpdate();
	}

	void UIInventory::Render()
	{
		if (bInventoryRender == true) 
		{
			UIBase::Render();	
		}
	}


	void UIInventory::InventoryOnOff(bool _on)
	{
		if (_on == true) 
		{
			for (int index = 0; index < GetButtonsHowMany(); index++) 
			{
				mButtonVector[index]->SetButtonRender(true);
			}
		}
		else if (_on == false)
		{
			for (int index = 0; index < GetButtonsHowMany(); index++)
			{
				mButtonVector[index]->SetButtonRender(false);
			}
		}
	}

	void UIInventory::CategoryArrangement(int _categoryNum)
	{

		if (_categoryNum == 18) //ALL
		{
			for (int index = 6; index < 18; index++)
			{
				mButtonVector[index]->SetButtonImage(1, 1, 0);
			}
			for (int index = 6; index < mCategoryAll.size()+6; index++)
			{
				mButtonVector[index]->SetButtonImage(1, 1, mCategoryAll[index - 6]); //전체가 이상함
			}
		}
		else if (_categoryNum == 19) //칼
		{
			for (int index = 6; index < 18; index++)
			{
				mButtonVector[index]->SetButtonImage(1, 1, 0);
			}
			for (int index = 6; index < mCategoryMeel.size() + 6; index++)
			{
				mButtonVector[index]->SetButtonImage(1, 1, 1);
			}
		}
		else if (_categoryNum == 20) //아머
		{
			for (int index = 6; index < 18; index++)
			{
				mButtonVector[index]->SetButtonImage(1, 1, 0);
			}
			for (int index = 6; index < mCategoryArmor.size() + 6; index++)
			{
				mButtonVector[index]->SetButtonImage(1, 1, 2);
			}
		}
		else if (_categoryNum == 21) // 활
		{
			for (int index = 6; index < 18; index++)
			{
				mButtonVector[index]->SetButtonImage(1, 1, 0);
			}
			for (int index = 6; index < mCategoryBow.size() + 6; index++)
			{
				mButtonVector[index]->SetButtonImage(1, 1, 3);
			}

		}
		else if (_categoryNum == 22) // 아이템
		{
			for (int index = 6; index < 18; index++)
			{
				mButtonVector[index]->SetButtonImage(1, 1, 0);
			}

			for (int index = 6; index < mCategoryItem.size()+6; index++)
			{

				mButtonVector[index]->SetButtonImage(1, 1,4);
			}
		}

		mNowCategoryNum = _categoryNum;
	}

	void UIInventory::CategoryVectorSet(int _categoryNum)
	{


		if (_categoryNum == 1) 
		{
			mCategoryMeel.push_back(1);
		}
		else if (_categoryNum == 2) 
		{
			mCategoryArmor.push_back(2);
		}
		else if (_categoryNum == 3)
		{
			mCategoryBow.push_back(3);
		}
		else if (_categoryNum == 4)
		{
			mCategoryItem.push_back(4);
		}
		mCategoryAll.push_back(_categoryNum);


	}

	void UIInventory::CategoryVectorReset(int _categoryNum)
	{
		
		if (_categoryNum == 1)
		{
			mCategoryMeel.pop_back();
		}
		else if (_categoryNum == 2)
		{
			mCategoryArmor.pop_back();
		}
		else if (_categoryNum == 3)
		{
			mCategoryBow.pop_back();
		}
		else if (_categoryNum == 4)
		{
			mCategoryItem.pop_back();
		}

		// mCategoryAll 벡터 초기화
		mCategoryAll.clear();

		// 다른 카테고리 벡터를 루프 돌면서 합칩니다.
		for (int index = 0; index < mCategoryMeel.size(); index++)
		{
			mCategoryAll.push_back(mCategoryMeel[index]);
		}
		for (int index = 0; index < mCategoryArmor.size(); index++)
		{
			mCategoryAll.push_back(mCategoryArmor[index]);
		}
		for (int index = 0; index < mCategoryBow.size(); index++)
		{
			mCategoryAll.push_back(mCategoryBow[index]);
		}
		for (int index = 0; index < mCategoryItem.size(); index++)
		{
			mCategoryAll.push_back(mCategoryItem[index]);
		}
	}



	void UIInventory::CategoryVectorArrangement(int mNowCategoryNum)//카테고리 벡터 정리 
	{

		for (int inventoryIndex = 6; inventoryIndex < 18; inventoryIndex++) //6~17은 아이템 정렬
		{
			if (mButtonVector[inventoryIndex]->GetItemType() == 0) 
			{
				mButtonVector[inventoryIndex]->SetButtonImage(1, 0, mNowCategoryNum);
				CategoryVectorSet(mNowCategoryNum);
				break;
			}
		}

	}

	void UIInventory::ExchageItem(UIButton* _buttonUI,int _index)//장비창이나 인벤토리를 우측 마우스 클릭을 했을때 해당 버튼이 아이템을 가지고 있으면     장비인 경우 인벤토리로         인벤토리일 경우 장비창으로 이동
	{
		if (_index >=0 && _index <= 5)//장비창
		{
			if (_index == 0) //무기
			{
				_buttonUI->SetButtonImage(0, 0, 0);
				CategoryVectorArrangement(1);
		
			}
			else if (_index == 1) //아머
			{
				_buttonUI->SetButtonImage(0, 0, 0);
				CategoryVectorArrangement(2);

			}
			else if(_index==2)//활
			{
				_buttonUI->SetButtonImage(0, 0, 0);
				CategoryVectorArrangement(3);
			
			}
			else if (_index == 3) //아이템
			{
				_buttonUI->SetButtonImage(0, 0, 0);
				CategoryVectorArrangement(4);
		
			}
		}
		else if (_index >= 6 && _index <= 17) 
		{
			if (_buttonUI->GetItemType() == 1) //무기
			{
				_buttonUI->SetButtonImage(1,1, 0);
				CategoryVectorReset(1);

				mButtonVector[0]->SetButtonImage(0, 0, 1);
			}
			else if (_buttonUI->GetItemType() == 2) //아머
			{
				_buttonUI->SetButtonImage(1, 1, 0);
				CategoryVectorReset(2);

				mButtonVector[1]->SetButtonImage(0, 0, 2);
			}
			else if (_buttonUI->GetItemType() == 3)//활
			{
				_buttonUI->SetButtonImage(1, 1, 0);
				CategoryVectorReset(3);

				mButtonVector[2]->SetButtonImage(0, 0, 3);
			}
			else if (_buttonUI->GetItemType() == 4) //아이템
			{
				_buttonUI->SetButtonImage(1, 1, 0);
				CategoryVectorReset(4);

				mButtonVector[3]->SetButtonImage(0, 0, 4);
			}
		}

		

	}

	void UIInventory::InitItemVector()
	{

		for (int index = 6; index < 18; index++) 
		{
			if (mButtonVector[index]->GetItemType() != 0) 
			{
				mCategoryAll.push_back(mButtonVector[index]->GetItemType());
			}
			if (mButtonVector[index]->GetItemType() == 1)
			{
				mCategoryMeel.push_back(mButtonVector[index]->GetItemType());
			}
			if (mButtonVector[index]->GetItemType() == 2)
			{
				mCategoryArmor.push_back(mButtonVector[index]->GetItemType());
			}
			if (mButtonVector[index]->GetItemType() == 3)
			{
				mCategoryBow.push_back(mButtonVector[index]->GetItemType());
			}
			if (mButtonVector[index]->GetItemType() == 4)
			{
				mCategoryItem.push_back(mButtonVector[index]->GetItemType());
			}
		}
	}

}




