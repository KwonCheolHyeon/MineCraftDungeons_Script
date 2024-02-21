#include "yaCamera.h"
#include "yaTransform.h"
#include "yaGameObject.h"
#include "yaApplication.h"
#include "yaRenderer.h"
#include "yaScene.h"
#include "yaResources.h"
#include "yaSceneManager.h"
#include "yaMaterial.h"
#include "yaBaseRenderer.h"
#include "yaSceneManager.h"

extern md::Application application;

namespace md
{
	Matrix Camera::View = Matrix::Identity;
	Matrix Camera::InverseView = Matrix::Identity;
	Matrix Camera::Projection = Matrix::Identity;

	Camera::Camera()
		: Component(eComponentType::Camera)
		, mType(eProjectionType::Perspective)
		, mCameraType(eCameraType::NoneUI)
		, mAspectRatio(1.0f)
		, mNear(1.0f)
		, mFar(100.0f)
		, mScale(1.0f)
	{
		EnableLayerMasks();
	}

	Camera::~Camera()
	{
	}

	void Camera::Initalize()
	{
		
		RegisterCameraInRenderer();
	}

	void Camera::Update()
	{

	}

	void Camera::FixedUpdate()
	{
		CreateViewMatrix();
		CreateProjectionMatrix();

		RegisterCameraInRenderer();
	}

	void Camera::Render()
	{
		sortGameObjects();

		if (eCameraType::NoneUI == mCameraType && (true != renderer::lights.empty()))
		{
			renderer::lights[0]->Decompose();
			// View proj  행렬 direction light 껄로 바뀌어야한다.
			Transform* tr = renderer::lights[0]->GetOwner()->GetComponent<Transform>();
			View = CreateViewMatrix(tr);
			Projection = CreateProjectionMatrix(eProjectionType::Orthographic, 1600, 900, 1.0f, 1000.0f);
			//= CreateProjectionMatrix(eProjectionType::Orthographic, 1600, 900, 1.0f, 1000.0f);
			//Projection = mProjection;
			//View = mView;
			//InverseView = View.Invert();
			//Projection = mProjection;

			ConstantBuffer* lightCB = renderer::constantBuffers[(UINT)eCBType::LightMatrix];

			LightMatrixCB data = {};
			data.lightView = View;
			data.lightProjection = Projection;
			lightCB->SetData(&data);
			lightCB->Bind(eShaderStage::VS);
			lightCB->Bind(eShaderStage::PS);

			// shadow
			renderTargets[(UINT)eRTType::Shadow]->OmSetRenderTarget();
			renderShadow();

			renderer::CopyRenderTarget();

			View = mView;
			InverseView = View.Invert();
			Projection = mProjection;

			// deffered opaque render
			renderTargets[(UINT)eRTType::Deffered]->OmSetRenderTarget();
			rednerDefferd();


			//// deffered light 
			renderTargets[(UINT)eRTType::Light]->OmSetRenderTarget();
			// 여러개의 모든 빛을 미리 한장의 텍스처에다가 계산을 해두고
			// 붙여버리자

			for (Light* light : renderer::lights)
			{
				light->Render();
			}


			// swapchain 
			renderTargets[(UINT)eRTType::Swapchain]->OmSetRenderTarget();

			////// defferd + swapchain merge
			std::shared_ptr<Material> mergeMaterial = Resources::Find<Material>(L"MergeMaterial");
			std::shared_ptr<Mesh> rectMesh = Resources::Find<Mesh>(L"RectMesh");

			rectMesh->BindBuffer(0);
			mergeMaterial->Bind();
			rectMesh->Render(0);
		}

		// Foward render
		renderOpaque();
		renderCutout();
		renderTransparent();
		renderPostProcess();
	}

	void Camera::CreateViewMatrix()
	{
		Transform* tr = GetOwner()->GetComponent<Transform>();
		Vector3 pos = tr->GetPosition();

		// Crate Translate view matrix
		mView = Matrix::Identity;
		mView *= Matrix::CreateTranslation(-pos);
		//회전 정보

		Vector3 up = tr->Up();
		Vector3 right = tr->Right();
		Vector3 forward = tr->Forward();

		Matrix viewRotate;
		viewRotate._11 = right.x; viewRotate._12 = up.x; viewRotate._13 = forward.x;
		viewRotate._21 = right.y; viewRotate._22 = up.y; viewRotate._23 = forward.y;
		viewRotate._31 = right.z; viewRotate._32 = up.z; viewRotate._33 = forward.z;

		mView *= viewRotate;
	}

	Matrix Camera::CreateViewMatrix(Transform* tr)
	{
		Matrix view = Matrix::Identity;
		Vector3 pos = tr->GetPosition();

		// Crate Translate view matrix
		view = Matrix::Identity;
		view *= Matrix::CreateTranslation(-pos);
		//회전 정보

		Vector3 up = tr->Up();
		Vector3 right = tr->Right();
		Vector3 forward = tr->Forward();

		Matrix viewRotate;
		viewRotate._11 = right.x; viewRotate._12 = up.x; viewRotate._13 = forward.x;
		viewRotate._21 = right.y; viewRotate._22 = up.y; viewRotate._23 = forward.y;
		viewRotate._31 = right.z; viewRotate._32 = up.z; viewRotate._33 = forward.z;

		view *= viewRotate;

		return view;
	}

	void Camera::CreateProjectionMatrix()
	{
		RECT winRect;
		GetClientRect(application.GetHwnd(), &winRect);

		float width = (winRect.right - winRect.left) * mScale;
		float height = (winRect.bottom - winRect.top) * mScale;

		if (width <= 0.00001f || height <= 0.00001f)
		{
			mProjection = Matrix::Identity;
			return;
		}

		mAspectRatio = width / height;

		if (mType == eProjectionType::Perspective)
		{
			mProjection = Matrix::CreatePerspectiveFieldOfViewLH
			(
				XM_2PI / 6.0f
				, mAspectRatio
				, mNear
				, mFar
			);
		}
		else
		{
			mProjection = Matrix::CreateOrthographicLH(width /*/ 100.0f*/, height /*/ 100.0f*/, mNear, mFar);
		}
	}

	Matrix Camera::CreateProjectionMatrix(eProjectionType type, float width, float height, float Near, float Far)
	{
		Matrix proj = Matrix::Identity;

		float AspectRatio = width / height;
		if (mType == eProjectionType::Perspective)
		{
			proj = Matrix::CreatePerspectiveFieldOfViewLH
			(
				XM_2PI / 6.0f
				, 1.0f
				, Near
				, Far
			);
		}
		else
		{
			proj = Matrix::CreateOrthographicLH(width /*/ 100.0f*/, height /*/ 100.0f*/, Near, Far);
		}

		return proj;
	}

	void Camera::RegisterCameraInRenderer()
	{
		eSceneType type = SceneManager::GetActiveScene()->GetSceneType();
		renderer::cameras[(UINT)type].push_back(this);
	}

	void Camera::TurnLayerMask(eLayerType layer, bool enable)
	{
		mLayerMasks.set((UINT)layer, enable);
	}

	void Camera::SetCameraType(eCameraType _type)
	{
		mCameraType = _type;
		if (eCameraType::UI == _type)
		{
			SetProjectionType(Camera::eProjectionType::Orthographic);
			DisableLayerMasks();
			TurnLayerMask(eLayerType::UI, true);
		}
		else
		{
			SetProjectionType(Camera::eProjectionType::Perspective);
			EnableLayerMasks();
			TurnLayerMask(eLayerType::UI, false);
		}
	}

	void Camera::sortGameObjects()
	{
		mDefferdOpaqueGameObjects.clear();
		mOpaqueGameObjects.clear();
		mCutoutGameObjects.clear();
		mTransparentGameObjects.clear();
		mPostProcessGameObjects.clear();

		Scene* scene = SceneManager::GetActiveScene();
		for (size_t i = 0; i < (UINT)eLayerType::End; i++)
		{
			if (mLayerMasks[i] == true)
			{
				Layer& layer = scene->GetLayer((eLayerType)i);
				GameObjects gameObjects = layer.GetGameObjects();
				if (gameObjects.size() == 0)
					continue;

				for (GameObject* obj : gameObjects)
				{
					pushGameObjectToRenderingModes(obj);
				}
			}
		}
	}

	void Camera::renderShadow()
	{


		for (GameObject* obj : mDefferdOpaqueGameObjects)
		{
			if (obj == nullptr)
				continue;

			obj->PrevRender();
		}

		for (GameObject* obj : mOpaqueGameObjects)
		{
			if (obj == nullptr)
				continue;

			obj->PrevRender();
		}

		//for (GameObject* obj : mTransparentGameObjects)
		//{
		//	if (obj == nullptr)
		//		continue;

		//	obj->PrevRender();
		//}

		//for (GameObject* obj : mCutoutGameObjects)
		//{
		//	if (obj == nullptr)
		//		continue;

		//	obj->PrevRender();
		//}
	}

	void Camera::rednerDefferd()
	{
		for (GameObject* obj : mDefferdOpaqueGameObjects)
		{
			if (obj == nullptr)
				continue;

			obj->Render();
		}
	}

	void Camera::renderOpaque()
	{
		for (GameObject* obj : mOpaqueGameObjects)
		{
			if (obj == nullptr)
				continue;

			obj->Render();
		}
	}

	void Camera::renderCutout()
	{
		for (GameObject* obj : mCutoutGameObjects)
		{
			if (obj == nullptr)
				continue;

			obj->Render();
		}
	}

	void Camera::renderTransparent()
	{
		for (GameObject* obj : mTransparentGameObjects)
		{
			if (obj == nullptr)
				continue;

			obj->Render();
		}
	}

	void Camera::renderPostProcess()
	{

		for (GameObject* obj : mPostProcessGameObjects)
		{
			if (obj == nullptr)
				continue;
			renderer::CopyRenderTarget();
			obj->Render();
		}
	}

	void Camera::pushGameObjectToRenderingModes(GameObject* gameObj)
	{
		BaseRenderer* renderer
			= gameObj->GetComponent<BaseRenderer>();
		if (renderer == nullptr)
			return;

		std::shared_ptr<Material> material = renderer->GetMaterial(0);
		if (material == nullptr)
			return;

		eRenderingMode mode = material->GetRenderingMode();

		switch (mode)
		{
		case md::graphics::eRenderingMode::DefferdOpaque:
		case md::graphics::eRenderingMode::DefferdMask:
			mDefferdOpaqueGameObjects.push_back(gameObj);
			break;
		case md::graphics::eRenderingMode::Opaque:
			mOpaqueGameObjects.push_back(gameObj);
			break;
		case md::graphics::eRenderingMode::CutOut:
			mCutoutGameObjects.push_back(gameObj);
			break;
		case md::graphics::eRenderingMode::Transparent:
			mTransparentGameObjects.push_back(gameObj);
			break;
		case md::graphics::eRenderingMode::PostProcess:
			mPostProcessGameObjects.push_back(gameObj);
			break;
		default:
			break;
		}
	}
}
