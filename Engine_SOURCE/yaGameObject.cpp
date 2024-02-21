#include "yaGameObject.h"
#include "yaTransform.h"
#include "yaRigidBody.h"
#include "yaSceneManager.h"
#include "yaMeshRenderer.h"
#include "yaMeshData.h"
#include "yaBoneAnimator.h"


namespace md
{
	GameObject::GameObject()
		: mState(eState::Active)
		, mType(eLayerType::None)
		, mbDontDestroy(false)
	{
		mComponents.resize((UINT)eComponentType::End);
		AddComponent(new Transform());
	}

	GameObject::~GameObject()
	{
		for (Component* comp : mComponents)
		{
			if (comp == nullptr)
				continue;
			
			delete comp;
			comp = nullptr;
		}

		for (Component* scrComp : mScripts)
		{
			if (scrComp == nullptr)
				continue;

			delete scrComp;
			scrComp = nullptr;
		}
	}

	void GameObject::Initalize()
	{

	}

	void GameObject::Update()
	{
		for (Component* comp : mComponents)
		{
			if (comp == nullptr)
				continue;

			comp->Update();
		}

		for (Component* script : mScripts)
		{
			if (script == nullptr)
				continue;

			script->Update();
		}
	}

	void GameObject::FixedUpdate()
	{
		for (Component* comp : mComponents)
		{
			if (comp == nullptr)
				continue;

			comp->FixedUpdate();
		}

		for (Component* script : mScripts)
		{
			if (script == nullptr)
				continue;

			script->FixedUpdate();
		}
	}

	void GameObject::PrevRender()
	{
		for (Component* comp : mComponents)
		{
			if (comp == nullptr)
				continue;

			comp->PrevRender();
		}
	}

	void GameObject::Render()
	{
		for (Component* comp : mComponents)
		{
			if (comp == nullptr)
				continue;

			comp->Render();
		}

		for (Component* script : mScripts)
		{
			if (script == nullptr)
				continue;

			script->Render();
		}
	}

	void GameObject::AddComponent(Component* comp)
	{
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
	}
	bool GameObject::IsPhysicsObject()
	{
		RigidBody* rigidBody = GetComponent<RigidBody>();
		if (nullptr == rigidBody)
			return false;

		return rigidBody->IsAppliedPhysics();
	}

	void GameObject::ReorganizePosition(eLayerType layerType)
	{
		assert(GetComponent<RigidBody>());
		assert(GetComponent<Collider3D>());

		const auto& gameObjects = SceneManager::GetActiveScene()->GetGameObjects(layerType);

		math::Vector3 vResult = {};
		for (const auto& gameObject : gameObjects)
		{
			enums::eAXIS axis = enums::eAXIS::END;

			// 충돌 (방향 * 깊이)를 계산해서 반환
			vResult = GetComponent<Collider3D>()->ComputePenetration(gameObject);
			if (vResult != math::Vector3::Zero && vResult.x > vResult.z)
			{
				if(vResult.x > 0.f)
					axis = enums::eAXIS::X;
				else 
					axis = enums::eAXIS::Z;
			}
			else if (vResult != math::Vector3::Zero && vResult.x < vResult.z)
			{
				if (vResult.z > 0.f)
					axis = enums::eAXIS::Z;
				else
					axis = enums::eAXIS::X;
			}
			switch (axis)
			{
			case enums::eAXIS::X:
				vResult.y = 0.f;
				vResult.z = 0.f;
				if (vResult.x > 0.f)
				{
					GetComponent<RigidBody>()->SetVelocity(eAXIS::X, math::Vector3(0.f, 0.f, 0.f));

					vResult *= 1.f;
					math::Vector3 pos = GetTransform()->GetPhysicalLocalPosition();
					math::Vector3 position = {};
					position = pos + vResult;
					GetTransform()->SetPhysicalLocalPosition(position);
				}
				else if(vResult.x < 0.f)
				{
					GetComponent<RigidBody>()->SetVelocity(eAXIS::X, math::Vector3(0.f, 0.f, 0.f));

					vResult *= -1.f;
					math::Vector3 pos = GetTransform()->GetPhysicalLocalPosition();
					math::Vector3 position = {};
					position = pos - vResult;
					GetTransform()->SetPhysicalLocalPosition(position);
				}
				break;
			case enums::eAXIS::Y:
				break;
			case enums::eAXIS::Z:
				vResult.x = 0.f;
				vResult.y = 0.f;
				if (vResult.z > 0.f)
				{
					GetComponent<RigidBody>()->SetVelocity(eAXIS::Z, math::Vector3(0.f, 0.f, 0.f));

					vResult *= 1.f;
					math::Vector3 pos = GetTransform()->GetPhysicalLocalPosition();
					math::Vector3 position = {};
					position = pos + vResult;
					GetTransform()->SetPhysicalLocalPosition(position);
				}
				else if (vResult.z < 0.f)
				{
					GetComponent<RigidBody>()->SetVelocity(eAXIS::Z, math::Vector3(0.f, 0.f, 0.f));

					vResult *= -1.f;
					math::Vector3 pos = GetTransform()->GetPhysicalLocalPosition();
					math::Vector3 position = {};
					position = pos - vResult;
					GetTransform()->SetPhysicalLocalPosition(position);
				}
				break;

			case enums::eAXIS::XY:
				vResult.z = 0.f;
				GetComponent<RigidBody>()->SetVelocity(eAXIS::XY, math::Vector3(0.f, 0.f, 0.f));
				break;
			case enums::eAXIS::XZ:
				vResult.y = 0.f;
				GetComponent<RigidBody>()->SetVelocity(eAXIS::XZ, math::Vector3(0.f, 0.f, 0.f));
				break;
			case enums::eAXIS::YZ:
				vResult.x = 0.f;
				GetComponent<RigidBody>()->SetVelocity(eAXIS::YZ, math::Vector3(0.f, 0.f, 0.f));
				break;
			case enums::eAXIS::XYZ:
				GetComponent<RigidBody>()->SetVelocity(eAXIS::XYZ, math::Vector3(0.f, 0.f, 0.f));
				break;
			case enums::eAXIS::END:
				break;
			default:
				break;
			}
			return;
		}
	}
	void GameObject::SettingFromMeshData(MeshData* data)
	{
		MeshRenderer* mr = GetComponent<MeshRenderer>();
		if (mr)
		{			
			AssertEx(mr, L"GameObject::SettingFromMeshData - 이미 MeshRenderer가 존재합니다.\n다시 확인하고 mesh가 설정되지 않은 GameObject에 해당 MeshData를 설정해 주세요.");		
			return;
		}

		if (!data)
		{
			AssertEx(!data, L"GameObject::SettingFromMeshData - 존재하지 않는 MeshData를 전달하여 설정하려고 했습니다.");
			return;
		}

		std::shared_ptr<Mesh> mesh = data->GetMesh();
		if (!mesh)
		{
			AssertEx(!mesh, L"GameObject::SettingFromMeshData - 존재하지 않는 MeshData를 전달하여 설정하려고 했습니다.");
			return;
		}

		mr = AddComponent<MeshRenderer>();
		mr->SetMesh(mesh);

		std::vector<std::shared_ptr<Material>>* materialsInMeshData = data->GetMaterials();
		for (size_t i = 0; i < materialsInMeshData->size(); i++)
		{
			mr->SetMaterial((*materialsInMeshData)[i], static_cast<int>(i));
		}

		// Animation 파트 
		if (mesh->IsAnimMesh())
		{
			BoneAnimator* animator = AddComponent<BoneAnimator>();
			animator->SetBones(mesh->GetBones());
			animator->SetAnimaitionClip(const_cast<std::vector<BoneAnimationClip>*>(mesh->GetAnimClip()));
		}

	}
}
