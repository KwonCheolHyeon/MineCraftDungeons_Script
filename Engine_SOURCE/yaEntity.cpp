#include "yaEntity.h"

namespace md
{
	UINT32 Entity::gIDNext{};

	Entity::Entity()
		: mID(++gIDNext)
		, mName(L"")
	{
	}
	Entity::Entity(const Entity& other)
		: mID(++gIDNext)
		, mName(other.mName)
	{
	}
	Entity::~Entity()
	{
	}
}

namespace gui
{
	UINT32 Entity::gIDNext{};

	Entity::Entity()
		: mID(++gIDNext)
		, mName("")
	{
	}

	Entity::Entity(const char* name)
		: mID(++gIDNext)
		, mName(name)
	{
	}

	Entity::Entity(const Entity& other)
		: mID(++gIDNext)
		, mName(other.mName)
	{
	}
	Entity::~Entity()
	{
	}
}
