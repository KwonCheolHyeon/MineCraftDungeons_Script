#pragma once
#include "yaComponent.h"
#include "mdStruct.h"

using namespace md::math;
namespace md
{
	class Transform : public Component
	{
	public:
		Transform();
		virtual ~Transform();

		virtual void Initalize() override;
		virtual void Update() override;
		virtual void FixedUpdate() override;
		virtual void PrevRender() override;
		virtual void Render() override;

		void SetConstantBuffer();
		
		void SetParent(Transform* parent) { mParent = parent; }
		Transform* GetParent() { return mParent; }

		void SetSocketParent(tBoneSocket* _socket) { mParentSocket = _socket; }


		math::Vector3 GetPhysicalLocalPosition();
		void SetPhysicalLocalPosition(math::Vector3 position);
		// PhysX의 Collider 회전 시키는 함수
		void SetPhysicalRotation(const math::Vector3& rotation);

		Vector3 GetPosition() { return mPosition; };
		float	GetPositionX() const { return mPosition.x; }
		float	GetPositionY() const { return mPosition.y; }
		float	GetPositionZ() const { return mPosition.z; }
		Vector2 GetPositionXY() const { return Vector2(mPosition.x, mPosition.y); }

		Vector3 GetRotation() { return mRotation; };
		float	GetRotationX() const { return mRotation.x; }
		float	GetRotationY() const { return mRotation.y; }
		float	GetRotationZ() const { return mRotation.z; }
		Vector2 GetRotationXY() const { return Vector2(mRotation.x, mRotation.y); }

		Vector3 GetScale() { return mScale; };
		float	GetScaleX() const { return mScale.x; }
		float	GetScaleY() const { return mScale.y; }
		float	GetScaleZ() const { return mScale.z; }
		Vector2 GetScaleXY() const { return Vector2(mScale.x, mScale.y); }

		void SetPosition(const Vector3& position);
		void SetPositionX(const float posX) { SetPosition(Vector3(posX, mPosition.y, mPosition.z)); }
		void SetPositionY(const float posY) { SetPosition(Vector3(mPosition.x, posY, mPosition.z));	}
		void SetPositionZ(const float posZ) { SetPosition(Vector3(mPosition.x, mPosition.y, posZ));	}
		void SetPositionXY(const Vector2& position) { SetPosition(Vector3(position.x, position.y, mPosition.z)); }

		void SetRotation(const Vector3& rotation);
		void SetRotationX(const float rotX) { SetRotation(Vector3(rotX, mRotation.y, mRotation.z));  }
		void SetRotationY(const float rotY) { SetRotation(Vector3(mRotation.x, rotY, mRotation.z));	 }
		void SetRotationZ(const float rotZ) { SetRotation(Vector3(mRotation.x, mRotation.y, rotZ));	 }
		void SetRotationXY(const Vector2& rotation) { SetRotation(Vector3(rotation.x, rotation.y, mRotation.z)); }

		void SetScale(const Vector3& scale) { mScale = scale; }
		void SetScaleX(const float scaleX) { mScale.x = scaleX; }
		void SetScaleY(const float scaleY) { mScale.y = scaleY; }
		void SetScaleZ(const float scaleZ) { mScale.z = scaleZ; }
		void SetScaleXY(const Vector2& scale) { mScale.x = scale.x; mScale.y = scale.y; }

		void AddPosition(const Vector3& position) { mPosition += position; }
		void AddPositionX(const float posX) { mPosition.x += posX; }
		void AddPositionY(const float posY) { mPosition.y += posY; }
		void AddPositionZ(const float posZ) { mPosition.z += posZ; }
		void AddPositionXY(const Vector2& position) { mPosition.x += position.x; mPosition.y += position.y; }

		void AddRotation(const Vector3& rotation) { mRotation += rotation; }
		void AddRotationX(const float rotX) { mRotation.x += rotX; }
		void AddRotationY(const float rotY) { mRotation.y += rotY; }
		void AddRotationZ(const float rotZ) { mRotation.z += rotZ; }

		void AddScale(const Vector3& scale) { mScale += scale; }
		void AddScaleX(const float scaleX) { mScale.x += scaleX; }
		void AddScaleY(const float scaleY) { mScale.y += scaleY; }
		void AddScaleZ(const float scaleZ) { mScale.z += scaleZ; }

		Vector3 Forward() { return mForward; }
		Vector3 Right() { return mRight; }
		Vector3 Up() { return mUp; }

		Matrix& GetWorldMatrix() { return mWorld; }
		Matrix GetWorldRotation()
		{
			Matrix rotation;

			Vector3 radian(mRotation.x * (XM_PI / 180)
				, mRotation.y * (XM_PI / 180)
				, mRotation.z * (XM_PI / 180));

			rotation = Matrix::CreateRotationX(radian.x);
			rotation *= Matrix::CreateRotationY(radian.y);
			rotation *= Matrix::CreateRotationZ(radian.z);

			return rotation;
		}

	public:
		void Move(const math::Vector3& _velocity);

	private:
		Transform* mParent;

		tBoneSocket* mParentSocket;
		Vector3 mSocketOffsetRot;
		Vector3 mSocketOffset;

		Matrix mWorld;

		Vector3 mForward;
		Vector3 mRight;
		Vector3 mUp;
		
		Vector3 mPosition;
		Vector3 mRotation;
		Vector3 mScale;
	};
}
