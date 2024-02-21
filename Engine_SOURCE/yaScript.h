#pragma once
#include "yaCollider3D.h"

namespace md
{
	class Script : public Component
	{
	public:
		Script();
		virtual ~Script();

		virtual void Initalize();
		virtual void Update();
		virtual void FixedUpdate();
		virtual void Render();

		virtual void OnCollisionEnter(Collider3D* collider) {};
		virtual void OnCollisionStay(Collider3D* collider) {};
		virtual void OnCollisionExit(Collider3D* collider) {};

		virtual void OnTriggerEnter(Collider3D* collider) {};
		virtual void OnTriggerStay(Collider3D* collider) {};
		virtual void OnTriggerExit(Collider3D* collider) {};

		virtual void Start() {};
		virtual void Action() {};
		virtual void Complete() {};
		virtual void End() {};

	private:
	};
}
