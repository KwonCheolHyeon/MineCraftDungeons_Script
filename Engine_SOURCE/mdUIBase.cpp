#include "mdUIBase.h"

#include "yaMeshRenderer.h"
#include "yaResources.h"
#include "yaMaterial.h"

namespace md
{

	UIBase::UIBase()
		: mMeshRender(nullptr)
	{
	}

	UIBase::~UIBase()
	{
	}

	void UIBase::Initalize()
	{

		SetMeshRender();
		GameObject::Initalize();

	}

	void UIBase::Update()
	{

		GameObject::Update();
	}

	void UIBase::FixedUpdate()
	{
		GameObject::FixedUpdate();
	}

	void UIBase::Render()
	{

		GameObject::Render();
	}

	void UIBase::SetMeshRender()
	{
		mMeshRender = this->AddComponent<MeshRenderer>();
		mMeshRender->SetMesh(Resources::Find<Mesh>(L"RectMesh"));
	}

	void UIBase::SetMaterial(const wchar_t* _str)
	{
		std::shared_ptr<Material> material = Resources::Find<Material>(_str);
		mMeshRender->SetMaterial(material, 0);

	}

	math::Vector4 UIBase::GetUIAABBSize() const
	{
		Vector3 uiPos = GetPosition();
		Vector3 uiScale = GetScale();

		math::Vector4 uiAABB = math::Vector4((uiPos.x - uiScale.x/2), (uiPos.x + uiScale.x / 2), (uiPos.y - uiScale.y / 2), (uiPos.y + uiScale.y / 2));

		return uiAABB;
	}



}
