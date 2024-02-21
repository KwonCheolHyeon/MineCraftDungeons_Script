#include "yaFontWrapper.h"
#include "yaGraphicDevice_DX11.h"

namespace md
{
	IFW1Factory* FontWrapper::mFW1Factory = nullptr;
	std::vector<IFW1FontWrapper*> md::FontWrapper::mFontWrapperVector;

	bool FontWrapper::Initialize()
	{
		if (FAILED(FW1CreateFactory(FW1_VERSION, &mFW1Factory)))
			return false;

		ID3D11Device* pDevice = graphics::GetDevice()->GetID3D11Device();

		// 폰트 추가
		const wchar_t* fontNames[] = { L"Arial", L"Agency FB",L"고도 B",L"둥근모꼴" };//여기에 폰트를 추가 하면 됨 arial = 0, Agency = 1, 고도 =2, 둥근모꼴 =3  ;
		for (const wchar_t* fontName : fontNames)
		{
			IFW1FontWrapper* fontWrapper = nullptr;
			if (FAILED(mFW1Factory->CreateFontWrapper(pDevice, fontName, &fontWrapper)))
			{
				// 실패 시 폰트 래퍼들 해제하고 false 반환
				Release();
				return false;
			}
			mFontWrapperVector.push_back(fontWrapper);
		}
		return true;
	}

	void FontWrapper::DrawFont(const wchar_t* _str, float _x, float _y, float _size, UINT _rgb, int _fontType)
	{
		//RGB();
		ID3D11DeviceContext* pContext = graphics::GetDevice()->GetID3D11DeviceContext();
		mFontWrapperVector[_fontType]->DrawString(
			pContext,
			_str, // String
			_size,// Font _size
			_x,// X position
			_y,// Y position
			_rgb,// Text color, 0xAaBbGgRr
			FW1_RESTORESTATE      // Flags (for example FW1_RESTORESTATE to keep context states unchanged)
		);
	}

	void FontWrapper::Release()
	{
		mFW1Factory->Release();
		mFW1Factory = nullptr;

		for (int index = 0; index < mFontWrapperVector.size(); index++)
		{
			mFontWrapperVector[index]->Release();
		}
		mFontWrapperVector.clear();
	}
}
