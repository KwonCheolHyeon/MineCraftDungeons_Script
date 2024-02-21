#pragma once
#include "yaScene.h"

namespace md
{
	class SceneManager
	{
	public:
		static void Initalize();
		static void Update();
		static void FixedUpdate();
		static void Render();
		static void Destroy();
		static void Release();

		static void CreateScene(eSceneType _type, Scene* _scene);
		static void LoadScene(eSceneType _type);
		static Scene* GetActiveScene() { return mActiveScene; }
		static class GameObject* FindPlayerObject();

	private:
		static std::vector<Scene*> mScenes;
		static Scene* mActiveScene;
	};
}
