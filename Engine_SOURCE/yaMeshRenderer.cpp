#include "yaMeshRenderer.h"
#include "yaGameObject.h"
#include "yaTransform.h"
#include "yaAnimator.h"
#include "yaBoneAnimator.h"
#include "yaMaterial.h"
#include "yaResources.h"

namespace md
{
	MeshRenderer::MeshRenderer()
		: BaseRenderer(eComponentType::MeshRenderer)
	{
	}

	MeshRenderer::~MeshRenderer()
	{
	}

	void MeshRenderer::Initalize()
	{
	}

	void MeshRenderer::Update()
	{
	}

	void MeshRenderer::FixedUpdate() 
	{
	}

	void MeshRenderer::PrevRender()
	{
		std::shared_ptr<Material> material 
			= Resources::Find<Material>(L"ShadowMapMaterial");

		material->Bind();

		GetOwner()->GetComponent<Transform>()->SetConstantBuffer();

		BoneAnimator* animator
			= GetOwner()->GetComponent<BoneAnimator>();

		UINT subSetCount = GetMesh()->GetSubSetCount();
		for (size_t i = 0; i < subSetCount; i++)
		{
			if (animator)
			{
				animator->Binds();
				material->SetAnimation(true);
				UINT boneCount = animator->GetBoneCount();
				material->SetBoneCount(boneCount);
			}

			UINT castedIndex = static_cast<UINT>(i);

			GetMesh()->BindBuffer(castedIndex);
			GetMesh()->Render(castedIndex);

			material->Clear();
		}

		if (animator)
		{
			animator->ClearData();
		}
	}

	void MeshRenderer::Render()
	{
		GetOwner()->GetComponent<Transform>()->SetConstantBuffer();
		BoneAnimator* animator	= GetOwner()->GetComponent<BoneAnimator>();

		UINT subSetCount = GetMesh()->GetSubSetCount();
		for (size_t i = 0; i < subSetCount; i++)
		{
			std::shared_ptr<Material> MTRL = GetMaterial(static_cast<int>(i));
			if (animator)
			{

				animator->Binds();
				MTRL->SetAnimation(true);

				UINT boneCount = animator->GetBoneCount();
				MTRL->SetBoneCount(boneCount);
			}

			std::shared_ptr<Mesh> Mesh = GetMesh();
			UINT castedIndex = static_cast<UINT>(i);

			Mesh->BindBuffer(castedIndex);
			MTRL->Bind();
			Mesh->Render(castedIndex);

			MTRL->Clear();
		}

		if (animator)
		{
			animator->ClearData();
		}
	}
}
