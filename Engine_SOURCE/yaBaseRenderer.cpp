#include "yaBaseRenderer.h"


namespace md
{



	BaseRenderer::BaseRenderer(eComponentType type)
		: Component(type)
	{
		mMaterial.resize(20);
	}

	BaseRenderer::~BaseRenderer()
	{
	}

	void BaseRenderer::Initalize()
	{
	}

	void BaseRenderer::Update()
	{
	}

	void BaseRenderer::FixedUpdate()
	{
	}

	void BaseRenderer::Render()
	{
	}

	void BaseRenderer::SetMesh(std::shared_ptr<Mesh> mesh)
	{
		mMesh = mesh;
		//if (!mMaterial.empty())
		//	mMaterial.clear();
		
		//UINT count = mMesh->GetSubSetCount();
		//mMaterial.resize(count);
	}
	//void BaseRenderer::SetMaterial(std::shared_ptr <Material> shader, int idx)
	//{

	//}

	//std::shared_ptr<Mesh> BaseRenderer::GetMesh()
	//{

	//}
	//std::shared_ptr <Material> BaseRenderer::GetMaterial(int idx)
	//{

	//}
}
