#pragma once
#include "mdUIBase.h"
#include "yaTexture.h"

namespace md 
{
	class UIEquipment
		: public UIBase
	{
	public:
		UIEquipment();
		virtual ~UIEquipment();

		void Initalize() override;
		void Update() override;
		void FixedUpdate() override;
		void Render() override;

		void SetItemType(int _type);

	private:
		void LoadUIEquipmentTexture();

	private:
		int mItmeType;
		int mPrevItemType;
		std::shared_ptr<md::graphics::Texture> mUITexture;

	};
}


