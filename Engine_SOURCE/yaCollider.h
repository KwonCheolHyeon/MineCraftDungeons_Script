#pragma once
#include "yaComponent.h"

namespace md
{
	class Collider : public Component
	{
	public:
		Collider(enums::eColliderType _Type);
		Collider(const Collider& _collider) = default;
		virtual ~Collider();


		virtual void Initalize() override {};
		virtual void Update() override {};
		virtual void FixedUpdate() override {};
		virtual void PrevRender() override {};
		virtual void Render() override = 0;


	private:
		enums::eColliderType mType;
	};
}
