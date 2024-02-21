#include "yaCollider.h"


namespace md
{
	Collider::Collider(enums::eColliderType _Type)
		: Component(enums::eComponentType::Collider)
		, mType(_Type)
	{
	}

	Collider::~Collider()
	{
	}
}
