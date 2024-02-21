#pragma once
#include "yaGameObject.h"

namespace md
{
	class RigidBody;

	class Player : public GameObject
	{
	public:
		Player();
		virtual ~Player();

		virtual void Initalize();
		virtual void Update();
		virtual void FixedUpdate();
		virtual void Render();

	private:
		float mMoveSpeed;
		RigidBody* mRigidBody;
	};
}
