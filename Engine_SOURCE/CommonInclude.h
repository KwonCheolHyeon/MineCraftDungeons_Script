#pragma once
#include <cstdlib>
#include <cstring>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <type_traits>
#include <Windows.h>
#include <locale>  // for std::wstring_convert
#include "magic_enum.hpp"

#define arraysize(_a) (sizeof(_a) / sizeof(_a[0]))

#define	SAFE_DELETE(_p)	if(_p)	{ delete _p; _p = nullptr; }
#define	SAFE_DELETE_ARRAY(_p)	if(_p)	{ delete[] _p; _p = nullptr; }
#define	SAFE_RELEASE(_p)	if(_p)	{ _p->Release(); _p = nullptr; }

#define GLOBAL_GRAVITY Application::GetPhysics()->GetGravity()
#define PHYSICS Application::GetPhysics()->GetPhysics()

#define MH_ASSERT(_expression) assert(_expression)

namespace md
{
	template <typename T, typename Size = uint8_t>
	class EnumFlags
	{
	public:
		explicit EnumFlags(void);
		EnumFlags(T _e);
		EnumFlags(const EnumFlags<T, Size>& _f);

		bool operator==(T _e) const;
		bool operator==(const EnumFlags<T, Size>& _f) const;
		bool operator==(bool _b) const;
		bool operator!=(T _e) const;
		bool operator!=(const EnumFlags<T, Size>& _f) const;

		EnumFlags<T, Size>& operator=(const EnumFlags<T, Size>& _f);
		EnumFlags<T, Size>& operator=(T _e);

		EnumFlags<T, Size>& operator|=(T _e);
		EnumFlags<T, Size>& operator|=(const EnumFlags<T, Size>& _f);
		EnumFlags<T, Size>	operator|(T _e) const;
		EnumFlags<T, Size>	operator|(const EnumFlags<T, Size>& _f) const;

		EnumFlags<T, Size>& operator&=(T _e);
		EnumFlags<T, Size>& operator&=(const EnumFlags<T, Size>& _f);
		EnumFlags<T, Size>	operator&(T _e) const;
		EnumFlags<T, Size>	operator&(const EnumFlags<T, Size>& _f) const;

		EnumFlags<T, Size>& operator^=(T _e);
		EnumFlags<T, Size>& operator^=(const EnumFlags<T, Size>& _f);
		EnumFlags<T, Size>	operator^(T _e) const;
		EnumFlags<T, Size>	operator^(const EnumFlags<T, Size>& _f) const;

		EnumFlags<T, Size> operator~(void) const;

		operator bool(void) const;
		operator uint8_t(void) const;
		operator uint16_t(void) const;
		operator uint32_t(void) const;

		void				clear(T e);
		void				raise(T e);
		bool				isSet(T e) const;
		EnumFlags<T, Size>& setAll(T e);

	public:
		friend EnumFlags<T, Size> operator&(T _a, EnumFlags<T, Size>& _b)
		{
			EnumFlags<T, Size> out{};
			out._bits = _a & _b._bits;
			return out;
		}

	private:
		Size _bits;
	};

	template <typename T, typename Size>
	EnumFlags<T, Size>::EnumFlags(void) { _bits = 0; }

	template <typename T, typename Size>
	EnumFlags<T, Size>::EnumFlags(T _e) { _bits = static_cast<Size>(_e); }

	template <typename T, typename Size>
	EnumFlags<T, Size>::EnumFlags(const EnumFlags<T, Size>& _other) { _bits = _other._bits; }

	template <typename T, typename Size>
	bool EnumFlags<T, Size>::operator==(T _e) const { return _bits == static_cast<Size>(_e); }

	template <typename T, typename Size>
	bool EnumFlags<T, Size>::operator==(const EnumFlags<T, Size>& _other) const { return _bits == _other._bits; }

	template <typename T, typename Size>
	bool EnumFlags<T, Size>::operator==(bool _b) const { return bool(*this) == _b; }

	template <typename T, typename Size>
	bool EnumFlags<T, Size>::operator!=(T _e) const { return _bits != static_cast<Size>(_e); }

	template <typename T, typename Size>
	bool EnumFlags<T, Size>::operator!=(const EnumFlags<T, Size>& _other) const { return _bits != _other._bits; }

	template <typename T, typename Size>
	EnumFlags<T, Size>& EnumFlags<T, Size>::operator=(T _e)
	{
		_bits = static_cast<Size>(_e);
		return *this;
	}

	template <typename T, typename Size>
	EnumFlags<T, Size>& EnumFlags<T, Size>::operator=(const EnumFlags<T, Size>& _f)
	{
		_bits = _f._bits;
		return *this;
	}

	template <typename T, typename Size>
	EnumFlags<T, Size>& EnumFlags<T, Size>::operator|=(T _e)
	{
		_bits |= static_cast<Size>(_e);
		return *this;
	}

	template <typename T, typename Size>
	EnumFlags<T, Size>& EnumFlags<T, Size>::operator|=(const EnumFlags<T, Size>& _f)
	{
		_bits |= _f._bits;
		return *this;
	}

	template <typename T, typename Size>
	EnumFlags<T, Size> EnumFlags<T, Size>::operator|(T _e) const
	{
		EnumFlags<T, Size> out(*this);
		out |= _e;
		return out;
	}

	template <typename T, typename Size>
	EnumFlags<T, Size> EnumFlags<T, Size>::operator|(const EnumFlags<T, Size>& _f) const
	{
		EnumFlags<T, Size> out(*this);
		out |= _f;
		return out;
	}

	template <typename T, typename Size>
	EnumFlags<T, Size>& EnumFlags<T, Size>::operator&=(T _e)
	{
		_bits &= static_cast<Size>(_e);
		return *this;
	}

	template <typename T, typename Size>
	EnumFlags<T, Size>& EnumFlags<T, Size>::operator&=(const EnumFlags<T, Size>& _f)
	{
		_bits &= _f._bits;
		return *this;
	}

	template <typename T, typename Size>
	EnumFlags<T, Size> EnumFlags<T, Size>::operator&(T _e) const
	{
		EnumFlags<T, Size> out = EnumFlags<T, Size>{ *this };
		out._bits &= static_cast<Size>(_e);
		return EnumFlags<T, Size>{out};
	}

	template <typename T, typename Size>
	EnumFlags<T, Size> EnumFlags<T, Size>::operator&(const EnumFlags<T, Size>& _f) const
	{
		EnumFlags<T, Size> out = *this;
		out._bits &= _f._bits;
		return out;
	}

	template <typename T, typename Size>
	EnumFlags<T, Size>& EnumFlags<T, Size>::operator^=(T _e)
	{
		_bits ^= static_cast<Size>(_e);
		return *this;
	}

	template <typename T, typename Size>
	EnumFlags<T, Size>& EnumFlags<T, Size>::operator^=(const EnumFlags<T, Size>& _f)
	{
		_bits ^= _f._bits;
		return *this;
	}

	template <typename T, typename Size>
	EnumFlags<T, Size> EnumFlags<T, Size>::operator^(T _e) const
	{
		EnumFlags<T, Size> out = *this;
		out._bits ^= static_cast<Size>(_e);
		return out;
	}

	template <typename T, typename Size>
	EnumFlags<T, Size> EnumFlags<T, Size>::operator^(const EnumFlags<T, Size>& _f) const
	{
		EnumFlags<T, Size> out = *this;
		out._bits ^= _f._bits;
		return out;
	}

	template <typename T, typename Size>
	EnumFlags<T, Size> EnumFlags<T, Size>::operator~(void) const
	{
		EnumFlags<T, Size> out{};
		out._bits = static_cast<Size>(~_bits);
		return EnumFlags<T, Size>{out};
	}

	template <typename T, typename Size>
	EnumFlags<T, Size>::operator bool(void) const { return _bits ? true : false; }

	template <typename T, typename Size>
	EnumFlags<T, Size>::operator uint8_t(void) const { return static_cast<uint8_t>(_bits); }

	template <typename T, typename Size>
	EnumFlags<T, Size>::operator uint16_t(void) const { return static_cast<uint16_t>(_bits); }

	template <typename T, typename Size>
	EnumFlags<T, Size>::operator uint32_t(void) const { return static_cast<uint32_t>(_bits); }

	template <typename T, typename Size>
	void EnumFlags<T, Size>::clear(T _e) { _bits &= ~static_cast<Size>(_e); }

	template <typename T, typename Size>
	void EnumFlags<T, Size>::raise(T _e) { _bits |= static_cast<Size>(_e); }

	template <typename T, typename Size>
	bool EnumFlags<T, Size>::isSet(T _e) const { return (_bits & static_cast<Size>(_e)) == static_cast<Size>(_e); }

	template <typename T, typename Size>
	EnumFlags<T, Size>& EnumFlags<T, Size>::setAll(T e)
	{
		_bits = static_cast<Size>(e);
		return *this;
	}
}
// Enable enum flags:
// https://www.justsoftwaresolutions.co.uk/cplusplus/using-enum-classes-as-bitfields.html


template<typename E>
struct enable_bitmask_operators
{
	static constexpr bool enable = false;
};

template<typename E>
constexpr typename std::enable_if<enable_bitmask_operators<E>::enable, E>::type operator|(E _lhs, E _rhs)
{
	typedef typename std::underlying_type<E>::type underlying;
	return static_cast<E>
		(
			static_cast<underlying>(_lhs) | static_cast<underlying>(_rhs)
			);
}

template<typename E>
constexpr typename std::enable_if<enable_bitmask_operators<E>::enable, E&>::type operator|=(E& _lhs, E _rhs)
{
	typedef typename std::underlying_type<E>::type underlying;
	_lhs = static_cast<E>
		(
			static_cast<underlying>(_lhs) | static_cast<underlying>(_rhs)
			);

	return _lhs;
}

template<typename E>
constexpr typename std::enable_if<enable_bitmask_operators<E>::enable, E>::type operator&(E _lhs, E _rhs)
{
	typedef typename std::underlying_type<E>::type underlying;
	return static_cast<E>
		(
			static_cast<underlying>(_lhs) & static_cast<underlying>(_rhs)
			);
}

template<typename E>
constexpr typename std::enable_if<enable_bitmask_operators<E>::enable, E&>::type operator&=(E& _lhs, E _rhs)
{
	typedef typename std::underlying_type<E>::type underlying;
	_lhs = static_cast<E>
		(
			static_cast<underlying>(_lhs) & static_cast<underlying>(_rhs)
			);

	return _lhs;
}

template<typename E>
constexpr typename std::enable_if<enable_bitmask_operators<E>::enable, E>::type operator~(E _rhs)
{
	typedef typename std::underlying_type<E>::type underlying;
	_rhs = static_cast<E>
		(
			~static_cast<underlying>(_rhs)
			);

	return _rhs;
}

template<typename E>
constexpr bool has_flag(E _lhs, E _rhs)
{
	return (_lhs & _rhs) == _rhs;
}

static inline std::wstring StringToWString(const std::string& _string)
{
	std::wstring result;
	int	ConvertLength = (int)MultiByteToWideChar(CP_ACP, 0, _string.c_str(), -1, nullptr, 0);
	wchar_t* wcChar = new wchar_t[ConvertLength];
	MultiByteToWideChar(CP_ACP, 0, _string.c_str(), -1, wcChar, ConvertLength);

	result = wcChar;
	SAFE_DELETE_ARRAY(wcChar);

	return result;
}

static inline std::string WStringToString(const std::wstring& _wstring)
{
	std::string result;
	int Length = (int)WideCharToMultiByte(CP_ACP, 0, _wstring.c_str(), -1, nullptr, 0, nullptr, nullptr);
	char* mbChar = new char[Length];
	WideCharToMultiByte(CP_ACP, 0, _wstring.c_str(), -1, mbChar, Length, nullptr, nullptr);

	result = mbChar;

	SAFE_DELETE_ARRAY(mbChar);

	return result;
}

template<typename Enum>
static inline std::wstring EnumToWString(Enum _value)
{
	std::string strResult(magic_enum::enum_name(_value));

	return StringToWString(strResult);
}

template<typename Enum>
static inline std::wstring EnumToString(Enum _value)
{
	std::string strResult(magic_enum::enum_name(_value));

	return strResult;
}

template<typename Class>
static inline std::string ClassNameToString() {
	const char* rawName = typeid(Class).name();
	std::string fullName = rawName;

	size_t pos = fullName.rfind("::");
	if (pos != std::string::npos && pos + 2 < fullName.size()) {
		return fullName.substr(pos + 2);
	}

	return fullName;
}

template<typename Class>
static inline std::wstring ClassNameToWString()
{
	return StringToWString(ClassNameToString<Class>());
}
