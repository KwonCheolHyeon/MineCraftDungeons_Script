#pragma once
#include <string>
#include <Windows.h>
#include <assert.h>

#include <vector>
#include <list>
#include <map>
#include <bitset>
#include <set>
#include <functional>
#include <array>

#include <cmath>
#include <algorithm>
#include <limits>
#include <memory>
#include <filesystem>


#include <xstring>

#include "CommonInclude.h"
#include "yaEnums.h"
#include "yaMath.h"

#ifdef _DEBUG
#pragma comment(lib, "..\\External\\PhysX\\lib\\Debug\\PhysX_64.lib")
#pragma comment(lib, "..\\External\\PhysX\\lib\\Debug\\PhysXFoundation_64.lib")
#pragma comment(lib, "..\\External\\PhysX\\lib\\Debug\\PhysXCommon_64.lib")
#pragma comment(lib, "..\\External\\PhysX\\lib\\Debug\\PhysXExtensions_static_64.lib")
#pragma comment(lib, "..\\External\\PhysX\\lib\\Debug\\PhysXPvdSDK_static_64.lib")
#pragma comment(lib, "..\\External\\PhysX\\lib\\Debug\\PhysXCharacterKinematic_static_64.lib")
#pragma comment(lib, "..\\External\\PhysX\\lib\\Debug\\PhysXCooking_64.lib")

//#pragma comment(lib, "FBX\\debug\\libxml2-md.lib")
//#pragma comment(lib, "FBX\\debug\\zlib-md.lib")
//#pragma comment(lib, "Assimp\\assimp-vc143-mt.lib")
#else
#pragma comment(lib, "PhysX\\release\\PhysX_64.lib")
#pragma comment(lib, "PhysX\\release\\PhysXFoundation_64.lib")
#pragma comment(lib, "PhysX\\release\\PhysXCommon_64.lib")
#pragma comment(lib, "PhysX\\release\\PhysXExtensions_static_64.lib")
#pragma comment(lib, "PhysX\\release\\PhysXPvdSDK_static_64.lib")
#pragma comment(lib, "PhysX\\release\\PhysXCharacterKinematic_static_64.lib")

#endif

#ifdef NDEBUG

#define AssertEx(_expression, _message) ((void)0)

#else
#define AssertEx(_expression, _message) AssertExDebug(_expression, _message)
#endif

static void AssertExDebug(bool _expression, const std::wstring& _message)
{
	if (_expression)
		return;

	MessageBoxW(NULL, _message.c_str(), L"Assert", MB_OK);
	assert(_expression);
}


