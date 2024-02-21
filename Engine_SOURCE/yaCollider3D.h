#pragma once
#include "yaCollider.h"


namespace md
{
	namespace graphics
	{
		class Material;
	}
	class Mesh;
	class Collider3D : public Collider
	{
	public:
		Collider3D();
		virtual ~Collider3D();

		virtual void Initalize() override;
		virtual void Update() override;
		virtual void FixedUpdate() override;
		virtual void PrevRender() override {};
		virtual void Render() override {};


		void OnCollisionEnter(Collider3D* _otherCollider);
		void OnCollisionStay(Collider3D* _otherCollider);
		void OnCollisionExit(Collider3D* _otherCollider);

		void OnTriggerEnter(Collider3D* _otherCollider);
		void OnTriggerStay(Collider3D* _otherCollider);
		void OnTriggerExit(Collider3D* _otherCollider);

		bool IsCollision() { return mCollisionCount; }
		void ClearCollisionCount() { mCollisionCount = 0; }

		void SetType(enums::eColliderType _type) { mType = _type; }

		math::Vector3 ComputePenetration(GameObject* _gameObject);

	private:
		enums::eColliderType mType;

		std::shared_ptr<Mesh> mMesh;
		std::shared_ptr<graphics::Material> mMaterial;

		physx::PxVec3   mPenetDir;
		float mPenetDepth;

		int mCollisionCount;
	};
}


