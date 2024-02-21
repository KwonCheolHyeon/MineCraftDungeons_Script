#pragma once
#include "yaScene.h"
#include "yaLayer.h"
#include "yaGameObject.h"
#include "yaSceneManager.h"
#include "yaTransform.h"
#include "yaRenderer.h"

namespace md::object
{
	template <typename T>
	static T* Instantiate(enums::eLayerType type)
	{
		T* gameObj = new T();
		Scene* scene = SceneManager::GetActiveScene();
		Layer& layer = scene->GetLayer(type);
		layer.AddGameObject<T>(gameObj);
		gameObj->SetLayerType(type);

		gameObj->Initalize();


		return gameObj;
	}

	template <typename T>
	static T* Instantiate(enums::eLayerType type, Scene* scene)
	{
		T* gameObj = new T();
		Layer& layer = scene->GetLayer(type);
		layer.AddGameObject<T>(gameObj);
		gameObj->SetLayerType(type);

		gameObj->Initalize();

		return gameObj;
	}

	template <typename T>
	static T* Instantiate(enums::eLayerType type, Transform* parent)
	{
		T* gameObj = Instantiate<T>();

		Transform* tr = gameObj->GameObject::GetComponent<Transform>();
		tr->SetParent(parent);

		return gameObj;
	}

	template <typename T>
	static T* Instantiate(enums::eLayerType type, Vector3 position, Vector3 rotation)
	{
		T* gameObj = Instantiate<T>();

		Transform* tr = gameObj->GameObject::GetComponent<Transform>();
		tr->SetPosition(position);
		tr->SetRotation(rotation);

		return gameObj;
	}

	template <typename T>
	static T* Instantiate(enums::eLayerType type, Vector3 position, Vector3 rotation, Transform* parent)
	{
		T* gameObj = Instantiate<T>();

		Transform* tr = gameObj->GameObject::GetComponent<Transform>();
		tr->SetPosition(position);
		tr->SetRotation(rotation);
		tr->SetParent(parent);

		return gameObj;
	}

	template <typename T>
	static T* InstantiateUI()
	{
		if (!renderer::mainUICamera)
		{
			AssertEx(FALSE, L"main UI Camera가 존재하지 않습니다.");
			return nullptr;
		}

		T* gameObj = Instantiate<T>(eLayerType::UI);

		Transform* tr = gameObj->GameObject::GetComponent<Transform>();
		tr->SetParent(renderer::mainUICamera->GetOwner()->GetTransform());

		return gameObj;
	}

	static void Destroy(GameObject* gameObject)
	{
		gameObject->Death();
	}

	static void DontDestroyOnLoad(GameObject* gameObject)   //씬 이동시 이 오브젝트는 삭제하지 않는다
	{
		if (gameObject == nullptr)
			return;

		gameObject->DontDestroy(true);
	}
}
