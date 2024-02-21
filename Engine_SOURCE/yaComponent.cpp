#include "yaComponent.h"
#include "yaGameObject.h"

namespace md
{
	Component::Component(eComponentType type)
		: mType(type)
		, mOwner(nullptr)
	{
	}
	Component::~Component()
	{
	}

	bool Component::IsPhysicsObject()
	{
		return GetOwner()->IsPhysicsObject();
	}
}
