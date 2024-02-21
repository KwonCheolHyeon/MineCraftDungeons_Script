#pragma once
#include "yaEntity.h"

using namespace md::enums;
namespace md
{
	class Resource : public Entity
	{
	public:
		Resource(eResourceType type);
		virtual ~Resource();

		virtual HRESULT Save(const std::wstring& path, FILE* file = nullptr) { return S_OK; };
		virtual HRESULT Load(const std::wstring& path, FILE* file = nullptr) = 0;

		const std::wstring& GetKey() { return mKey; }
		const std::wstring& GetPath() { return mPath; }
		eResourceType GetType() { return mType; }

		void SetKey(const std::wstring& key) { mKey = key; }
		void SetPath(const std::wstring& path) { mPath = path; }

	private:
		std::wstring mKey;
		std::wstring mPath;
		eResourceType mType;
	};
}
