#pragma once
#include "yaGameObject.h"

namespace md
{
	typedef const std::vector<GameObject*>& GameObjects;
	typedef std::vector<GameObject*>::iterator GameObjectIter;

	class Layer : public Entity
	{
		friend class Scene;
	public:
		Layer();
		virtual ~Layer();

		virtual void Initalize();
		virtual void Update();
		virtual void FixedUpdate();
		virtual void Render();
		virtual void Destroy();

		template <typename T>
		void AddGameObject(GameObject* gameObject);

		GameObjects GetGameObjects() { return mGameObjects; }
		std::vector<GameObject*> GetDontDestroyGameObjects();

	private:
		eLayerType GetLayerType() { return mLayerType; }
		void SetLayerType(eLayerType _layerType);

	private:
		std::vector<GameObject*> mGameObjects;
		eLayerType mLayerType;
	};

	template <typename T>
	void md::Layer::AddGameObject(GameObject* gameObject)
	{
		if (gameObject == nullptr)
			return;

		gameObject->SetName(ClassNameToWString<T>());

		mGameObjects.push_back(gameObject);
	}

}
