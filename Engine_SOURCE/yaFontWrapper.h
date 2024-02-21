#pragma once
#include "yaEngine.h"

#include "..\\External\\FW1FontWrapper\\Include\\FW1FontWrapper.h"
#include "..\\External\\FW1FontWrapper\\Include\\FW1CompileSettings.h"

#ifdef _DEBUG
#pragma comment(lib, "..\\External\\FW1FontWrapper\\Library\\Debug\\FW1FontWrapperL.lib")
#else
#pragma comment(lib, "..\\External\\FW1FontWrapper\\Library\\Debug\\FW1FontWrapper.lib")
#endif

namespace md
{
	class FontWrapper
	{
	public:
		static bool Initialize();
		static void DrawFont(const wchar_t* _str, float _x, float _y, float _size, UINT _rgb, int _fontType);
		static void Release();

	private:
		static IFW1Factory* mFW1Factory;
		static std::vector<IFW1FontWrapper*> mFontWrapperVector;
	};
}
