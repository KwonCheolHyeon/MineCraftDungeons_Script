#include "yaSceneManager.h"
#include "yaTransform.h"
#include "yaMeshRenderer.h"
#include "yaRenderer.h"
#include "yaResources.h"
#include "yaTexture.h"
#include "yaPlayerScript.h"
#include "yaCamera.h"
#include "yaCameraScript.h"
#include "yaSpriteRenderer.h"
#include "yaGridScript.h"

namespace md
{
	std::vector<Scene*> SceneManager::mScenes = {};
	Scene* SceneManager::mActiveScene = nullptr;

	void SceneManager::Initalize()
	{
		mScenes.resize((UINT)eSceneType::End);
	}

	void SceneManager::Update()
	{
		mActiveScene->Update();
	}

	void SceneManager::FixedUpdate()
	{
		mActiveScene->FixedUpdate();
	}

	void SceneManager::Render()
	{
		mActiveScene->Render();
	}
	
	void SceneManager::Destroy()
	{
		mActiveScene->Destroy();
	}

	void SceneManager::Release()
	{
		for (Scene* scene : mScenes)
		{
			delete scene;
			scene = nullptr;
		}
	}
	
	void SceneManager::CreateScene(eSceneType _type, Scene* _scene)
	{
		if (mScenes[(UINT)_type])
		{
			AssertEx(true, L"이미 존재하는 씬을 생성하려고 했습니다.");
			return;
		}

		mScenes[static_cast<UINT>(_type)] = _scene;
		mScenes[static_cast<UINT>(_type)]->SetName(EnumToWString(_type));
	}

	void SceneManager::LoadScene(eSceneType _type)
	{
		std::vector<GameObject*> gameObjs;
		if (mActiveScene)
		{
			mActiveScene->OnExit();

			// 바뀔때 dontDestory 오브젝트는 다음씬으로 같이 넘겨줘야한다.
			gameObjs = mActiveScene->GetDontDestroyGameObjects();
		}

		mActiveScene = mScenes[(UINT)_type];
		
		for (GameObject* obj : gameObjs)
		{
			eLayerType layerType = obj->GetLayerType();
			mActiveScene->AddGameObject(obj, layerType);
		}

		mActiveScene->OnEnter();
	}
	GameObject* SceneManager::FindPlayerObject()
	{
		GameObject* player = nullptr;
		if (mActiveScene)
		{
			player = mActiveScene->FindPlayerObject();
		}
		
		return player;
	}
}
