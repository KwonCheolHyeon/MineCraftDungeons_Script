#include "yaLight.h"
#include "yaTransform.h"
#include "yaGameObject.h"
#include "yaRenderer.h"
#include "yaResources.h"
#include "yaMaterial.h"

namespace md
{
	bool closeEnough(const float& a, const float& b
		, const float& epsilon = std::numeric_limits<float>::epsilon())
	{
		return (epsilon > std::abs(a - b));
	}

	Vector3 DecomposeRotMat(const Matrix _matRot)
	{
		// _mat 을 분해 후 다시 행렬 만들기	
		Vector4 vMat[4];

		vMat[0] = Vector4(_matRot._11, _matRot._12, _matRot._13, _matRot._14);
		vMat[1] = Vector4(_matRot._21, _matRot._22, _matRot._23, _matRot._24);
		vMat[2] = Vector4(_matRot._31, _matRot._32, _matRot._33, _matRot._34);
		vMat[3] = Vector4(_matRot._41, _matRot._42, _matRot._43, _matRot._44);

		/*XMStoreFloat4(&vMat[0], _matRot._11);
		XMStoreFloat4(&vMat[1], _matRot.r[1]);
		XMStoreFloat4(&vMat[2], _matRot.r[2]);
		XMStoreFloat4(&vMat[3], _matRot.r[3]);*/

		Vector3 vNewRot;
		if (closeEnough(vMat[0].z, -1.0f)) {
			float x = 0; //gimbal lock, value of x doesn't matter
			float y = XM_PI / 2;
			float z = x + atan2f(vMat[1].x, vMat[2].x);
			vNewRot = Vector3{ x, y, z };
		}
		else if (closeEnough(vMat[0].z, 1.0f)) {
			float x = 0;
			float y = -XM_PI / 2;
			float z = -x + atan2f(-vMat[1].x, -vMat[2].x);
			vNewRot = Vector3{ x, y, z };
		}
		else { //two solutions exist
			float y1 = -asinf(vMat[0].z);
			float y2 = XM_PI - y1;

			float x1 = atan2f(vMat[1].z / cosf(y1), vMat[2].z / cosf(y1));
			float x2 = atan2f(vMat[1].z / cosf(y2), vMat[2].z / cosf(y2));

			float z1 = atan2f(vMat[0].y / cosf(y1), vMat[0].x / cosf(y1));
			float z2 = atan2f(vMat[0].y / cosf(y2), vMat[0].x / cosf(y2));

			//choose one solution to return
			//for example the "shortest" rotation
			if ((std::abs(x1) + std::abs(y1) + std::abs(z1)) <= (std::abs(x2) + std::abs(y2) + std::abs(z2)))
			{
				vNewRot = Vector3{ x1, y1, z1 };
			}
			else {
				vNewRot = Vector3{ x2, y2, z2 };
			}
		}
		return vNewRot;
	}

	Light::Light()
		: Component(eComponentType::Light)
	{
		renderer::lights.push_back(this);
	}

	Light::~Light()
	{

	}

	void Light::Initalize()
	{

	}

	void Light::Update()
	{

	}

	void Light::FixedUpdate()
	{
		Transform* tr = GetOwner()->GetComponent<Transform>();
		if (eLightType::Point == mAttribute.type)
		{
			tr->SetScale(Vector3(mAttribute.radius * 5.f, mAttribute.radius * 5.f, mAttribute.radius * 5.f));
		}

		Vector3 position = tr->GetPosition();
		mAttribute.position = Vector4(position.x, position.y, position.z, 1.0f);
		mAttribute.direction = Vector4(tr->Forward().x, tr->Forward().y, tr->Forward().z, 0.0f);

		renderer::PushLightAttribute(mAttribute);
	}

	void Light::Decompose()
	{
		//Component::PrevRender();

		Transform* tr = GetOwner()->GetComponent<Transform>();
		//tr->SetPosition(Transform()->GetWorldPos());
		tr->SetRotation(DecomposeRotMat(tr->GetWorldRotation()));
	}

	void Light::Render()
	{

		std::shared_ptr<Material> material = nullptr;

		if (mAttribute.type == eLightType::Directional)
		{
			material = Resources::Find<Material>(L"LightMaterial");
		}
		else if (mAttribute.type == eLightType::Point)
		{
			material = Resources::Find<Material>(L"LightPointMaterial");
		}


		if (material == nullptr)
			return;

		Transform* tr = GetOwner()->GetComponent<Transform>();
		tr->SetConstantBuffer();

		ConstantBuffer* cb = renderer::constantBuffers[(UINT)eCBType::Light];
		LightCB data = {};
		data.numberOfLight = static_cast<UINT>(renderer::lights.size());
		data.indexOfLight = mIndex;

		cb->SetData(&data);
		cb->Bind(eShaderStage::VS);
		cb->Bind(eShaderStage::PS);

		mVolumeMesh->BindBuffer(0);
		material->Bind();
		mVolumeMesh->Render(0);
	}
	void Light::SetType(eLightType type)
	{
		mAttribute.type = type;
		if (mAttribute.type == eLightType::Directional)
		{
			mVolumeMesh = Resources::Find<Mesh>(L"RectMesh");
		}
		else if (mAttribute.type == eLightType::Point)
		{
			mVolumeMesh = Resources::Find<Mesh>(L"SphereMesh");
		}
		else if (mAttribute.type == eLightType::Spot)
		{
			//
		}
	}
}
