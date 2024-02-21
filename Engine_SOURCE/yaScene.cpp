#include "yaScene.h"
#include "yaPlayer.h"


namespace md
{
	Scene::Scene(eSceneType type)
		: mType(type)
	{
		mLayers.resize((UINT)eLayerType::End);
		size_t Size = mLayers.size();
		for (size_t layerIdx = 0; layerIdx < Size; ++layerIdx)
		{
			mLayers[layerIdx].SetLayerType(static_cast<eLayerType>(layerIdx));
		}
	}

	Scene::~Scene()
	{

	}

	void Scene::Initalize()
	{
		for (Layer& layer : mLayers)
		{
			layer.Initalize();
		}
	}
	void Scene::Update()
	{
		for (Layer& layer : mLayers)
		{
			layer.Update();
		}
	}
	void Scene::FixedUpdate()
	{
		for (Layer& layer : mLayers)
		{
			layer.FixedUpdate();
		}
	}
	void Scene::Render()
	{
		for (Layer& layer : mLayers)
		{
			layer.Render();
		}
	}
	void Scene::Destroy()
	{
		for (Layer& layer : mLayers)
		{
			layer.Destroy();
		}
	}
	void Scene::OnEnter()
	{
		Initalize();
	}
	void Scene::OnExit()
	{
	}
	void Scene::AddGameObject(GameObject* gameObj, const eLayerType type)
	{
		mLayers[(UINT)type].AddGameObject<GameObject>(gameObj);
		gameObj->SetLayerType(type);
	}
	
	std::vector<GameObject*> Scene::GetDontDestroyGameObjects()
	{
		std::vector<GameObject*> gameObjects;
		for (Layer& layer : mLayers)
		{
			std::vector<GameObject*> dontGameObjs
				= layer.GetDontDestroyGameObjects();

			gameObjects.insert(gameObjects.end()
			, dontGameObjs.begin()
			, dontGameObjs.end());
		}

		return gameObjects;
	}
	const std::vector<GameObject*>& Scene::GetGameObjects(const eLayerType type)
	{
		return mLayers[(UINT)type].GetGameObjects();
	}

	md::GameObject* Scene::FindPlayerObject()
	{
		GameObject* player = nullptr;

		size_t layerSize = mLayers.size();

		std::vector<GameObject*> layerObjects;
		size_t objectsSize = 0;

		for (size_t layerIdx = 0; layerIdx < layerSize; ++layerIdx)
		{
			layerObjects = mLayers[layerIdx].GetGameObjects();
			objectsSize = layerObjects.size();

			for (size_t objectIdx = 0; objectIdx < objectsSize; ++objectIdx)
			{
				if (dynamic_cast<Player*>(layerObjects[objectIdx]))
				{
					player = layerObjects[objectIdx];
					break;
				}
			}
		}

		return player;
	}

}
