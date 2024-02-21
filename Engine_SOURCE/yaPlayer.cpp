#include "yaPlayer.h"

#include "yaMeshData.h"
#include "yaResources.h"
#include "yaRigidBody.h"
#include "yaPlayerScript.h"
#include "yaInput.h"
#include "mdDetourManager.h"
#include "yaCollisionManager.h"
#include "mdDamageScript.h"

namespace md
{
	Player::Player()
		: mMoveSpeed(10.0f)
		, mRigidBody(nullptr)
	{

	}

	Player::~Player()
	{
	}

	void Player::Initalize()
	{
	

		GameObject::Initalize();
	}

	void Player::Update()
	{
		GameObject::Update();

	}

	void Player::FixedUpdate()
	{
		GameObject::FixedUpdate();
	}

	void Player::Render()
	{
		/*DamageScript* scr = GetScript<DamageScript>();
		scr->HitFlash();*/
		GameObject::Render();
	}

}
