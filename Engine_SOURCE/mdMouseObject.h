#pragma once
#include "mdUIBase.h"
#include "UIEnums.h"

namespace md
{
    class UIInventory;
    class UIButton;
    class MouseObject :
        public UIBase
    {
    public:
        MouseObject();
        virtual ~MouseObject();

        void Initalize() override;
        void Update() override;
        void FixedUpdate() override;
        void Render() override;


    public:
        void SetInventory(UIInventory* _inventory) { mInventory = _inventory; }
        
    private:
        void UIAABBSizeCompare();
        void DetectUI(UIButton* _uiButton, int _index);//UIAABBSize를 통해 받아온 UI들의 위치를 찾고 해당 버튼을 컨트롤 할 수 있음

    private:
        void MousePosUpdate();//마우스의 위치 갱신하여 Transform에 저장
        void PreProcessTransformFixedUpdate(); // Transform 의 FixedUpdate()에서 UI 레이어의 객체로 처리되기 위한 처리

    private:
        UIInventory* mInventory;
        std::vector<UIButton*> mButtonVector;

        eUIMouseStatus mMouseStatus;
    };
}


