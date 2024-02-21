#pragma once
#include "yaGameObject.h"

namespace md
{
	class UIBase :
		public GameObject
	{
	public:
		UIBase();
		virtual ~UIBase();

		void Initalize() override;
		void Update() override;
		void FixedUpdate() override;
		void Render() override;

		void SetMeshRender();
		void SetMaterial(const wchar_t* _str);

		math::Vector4 GetUIAABBSize() const;
	

	private:
		class MeshRenderer* mMeshRender;
		math::Vector4 mUIAABBSize;

	};


}

