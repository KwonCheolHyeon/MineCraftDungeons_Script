#pragma once
#include "yaComponent.h"
#include "yaScript.h"
#include "yaEntity.h"

#include "yaTransform.h"

namespace md
{
	class GameObject : public Entity
	{
	public:
		enum eState
		{
			Active,
			Paused,
			Dead,
		};

		GameObject();
		virtual ~GameObject();

		virtual void Initalize();
		virtual void Update();
		virtual void FixedUpdate();
		virtual void PrevRender();
		virtual void Render();

		template <typename T>
		T* AddComponent()
		{
			T* comp = new T();
			eComponentType order = comp->GetOrder();

			if (order != eComponentType::Script)
			{
				mComponents[(UINT)order] = comp;
				mComponents[(UINT)order]->SetOwner(this);
			}
			else
			{
				mScripts.push_back(dynamic_cast<Script*>(comp));
				comp->SetOwner(this);
			}

			if (eComponentType::RigidBody != order)
			{
				comp->Initalize();
			}

			return comp;
		}
		void AddComponent(Component* comp);

		template <typename T>
		T* GetComponent()
		{
			T* comp;
			for (auto c : mComponents)
			{
				comp = dynamic_cast<T*>(c);

				if (comp != nullptr)
					return comp;
			}

			return nullptr;
		}
		template <typename T>
		std::vector<T*> GetComponents()
		{
			std::vector<T*> components = {};
			
			T* comp;
			for (auto c : mComponents)
			{
				comp = dynamic_cast<T*>(c);

				if (comp != nullptr)
					components.push_back(comp);
			}

			return components;
		}

		template <typename T>
		T* GetScript()
		{
			T* scr;
			for (auto c : mScripts)
			{
				scr = dynamic_cast<T*>(c);

				if (scr != nullptr)
					return scr;
			}

			return nullptr;
		}

		const std::vector<Script*>& GetScripts() { return mScripts; };

	public:
		Transform* GetTransform() const
		{
			if (mComponents[(UINT)eComponentType::Transform])
				return static_cast<Transform*>(mComponents[(UINT)eComponentType::Transform]);

			return nullptr;
		}

		math::Vector3 GetPhysicalLocalPosition() { return GetTransform()->GetPhysicalLocalPosition(); }
		math::Vector3 GetPhysicalWorldPosition() { return GetTransform()->GetPhysicalLocalPosition() + GetTransform()->GetPosition(); }
		void SetPhysicalLocalPosition(math::Vector3 position) { GetTransform()->SetPhysicalLocalPosition(position); }

		math::Vector3 GetPosition() const { return GetTransform()->GetPosition(); }
		float GetPositionX() const { return GetTransform()->GetPositionX(); }
		float GetPositionY() const { return GetTransform()->GetPositionY(); }
		float GetPositionZ() const { return GetTransform()->GetPositionZ(); }
		math::Vector2 GetPositionXY() const { return GetTransform()->GetPositionXY(); }

		math::Vector3 GetRotation() const { return GetTransform()->GetRotation(); }
		float GetRotationX() const { return GetTransform()->GetRotationX(); }
		float GetRotationY() const { return GetTransform()->GetRotationY(); }
		float GetRotationZ() const { return GetTransform()->GetRotationZ(); }
		math::Vector2 GetRotationXY() const { return GetTransform()->GetRotationXY(); }

		math::Vector3 GetScale() const { return GetTransform()->GetScale(); }
		float GetScaleX() const { return GetTransform()->GetScaleX(); }
		float GetScaleY() const { return GetTransform()->GetScaleY(); }
		float GetScaleZ() const { return GetTransform()->GetScaleZ(); }
		math::Vector2 GetScaleXY() const { return GetTransform()->GetScaleXY(); }

		void SetPosition(const math::Vector3& position) { GetTransform()->SetPosition(position); }
		void SetPositionX(const float posX) { GetTransform()->SetPositionX(posX); }
		void SetPositionY(const float posY) { GetTransform()->SetPositionY(posY); }
		void SetPositionZ(const float posZ) { GetTransform()->SetPositionZ(posZ); }
		void SetPositionXY(const math::Vector2& position) { GetTransform()->SetPositionXY(position); }

		void SetRotation(const math::Vector3& rotation) { GetTransform()->SetRotation(rotation); }
		void SetRotationX(const float rotX) { GetTransform()->SetRotationX(rotX); }
		void SetRotationY(const float rotY) { GetTransform()->SetRotationY(rotY); }
		void SetRotationZ(const float rotZ) { GetTransform()->SetRotationZ(rotZ); }
		void SetRotationXY(const math::Vector2& rotation) { GetTransform()->SetRotationXY(rotation); }

		void SetScale(const math::Vector3& scale) { GetTransform()->SetScale(scale); }
		void SetScaleX(const float scaleX) { GetTransform()->SetScaleX(scaleX); }
		void SetScaleY(const float scaleY) { GetTransform()->SetScaleY(scaleY); }
		void SetScaleZ(const float scaleZ) { GetTransform()->SetScaleZ(scaleZ); }
		void SetScaleXY(const math::Vector2& scale) { GetTransform()->SetScaleXY(scale); }

		void AddPosition(const math::Vector3& position) { GetTransform()->AddPosition(position); }
		void AddPositionX(const float posX) { GetTransform()->AddPositionX(posX); }
		void AddPositionY(const float posY) { GetTransform()->AddPositionY(posY); }
		void AddPositionZ(const float posZ) { GetTransform()->AddPositionZ(posZ); }
		void AddPositionXY(const math::Vector2& position) { GetTransform()->AddPositionXY(position); }

		void AddRotation(const math::Vector3& rotation) { GetTransform()->AddRotation(rotation); }
		void AddRotationX(const float rotX) { GetTransform()->AddRotationX(rotX); }
		void AddRotationY(const float rotY) { GetTransform()->AddRotationY(rotY); }
		void AddRotationZ(const float rotZ) { GetTransform()->AddRotationZ(rotZ); }

		void AddScale(const math::Vector3& scale) { GetTransform()->AddScale(scale); }
		void AddScaleX(const float scaleX) { GetTransform()->AddScaleX(scaleX); }
		void AddScaleY(const float scaleY) { GetTransform()->AddScaleY(scaleY); }
		void AddScaleZ(const float scaleZ) { GetTransform()->AddScaleZ(scaleZ); }

		math::Vector3 Forward() const { return GetTransform()->Forward(); }
		math::Vector3 Right() const { return GetTransform()->Right(); }
		math::Vector3 Up() const { return GetTransform()->Up(); }

	public:

		inline bool IsDead()
		{
			bool result = false;

			if (mState == eState::Dead)
			{
				result = true;
			}
			
			return result;
		}

		inline bool IsNeedToProcess()
		{
			bool result = false;

			if (eState::Active == mState)
			{
				result = true;
			}

			return result;
		}


		void Activate() { mState = eState::Active; }
		void Pause() { mState = eState::Paused; }
		void Death() { mState = eState::Dead; }
		eState GetState() { return mState; }
		
		bool IsDontDestroy() { return mbDontDestroy; }
		void DontDestroy(bool enable) { mbDontDestroy = enable; }
		eLayerType GetLayerType() { return mType; }
		eLayerType GetLayerType() const { return mType; } // raycast에서 const로 gameobject를 받아오기 때문에 
		void SetLayerType(eLayerType type) { mType = type; }

		/* 물리적 객체인지를 검사하는 함수 */
		bool IsPhysicsObject();

		// PhysX Object끼리 충돌했는지 확인 후 충돌전의 위치로 세팅
		void ReorganizePosition(eLayerType layerType); // 충돌체 겹친 크기만큼 밀어냄

	public:
		void SettingFromMeshData(class MeshData* data);

	protected:
		std::vector<Component*> mComponents;


	private:
		eState mState;
		eLayerType mType;
		std::vector<Script*> mScripts;
		bool mbDontDestroy;
		//Scene* mScene;
	};
}

