#include "yaCollisionManager.h"
#include "yaSceneManager.h"
#include "yaRigidBody.h"
#include "yaEnums.h"
#include "yaPhysics.h"
#include "yaApplication.h"
#include "yaShader.h"
#include "yaMesh.h"
#include "yaResources.h"
#include "yaCamera.h"
#include "yaRenderer.h"
#include "yaMeshRenderer.h"
#include "yaInput.h"

extern md::Application application;
namespace md
{
	std::array<std::bitset<enums::LAYER_TYPE_COUNT>, enums::LAYER_TYPE_COUNT> CollisionManager::mArrColGroup{};

	std::bitset<(UINT)enums::eLayerType::End> CollisionManager::mLayerCollisionMatrix[(UINT)enums::eLayerType::End]{};
	std::map<UINT64, bool> CollisionManager::mCollisionMap{};

	std::array<std::bitset<32>, 32> CollisionManager::mRaycastMask{};
	tRaycastHit CollisionManager::mRayResult{};

	void CollisionManager::SetCollisionGroup(enums::eLayerType _layerType_1, enums::eLayerType _layerType_2)
	{
		mArrColGroup[static_cast<int>(_layerType_1)][static_cast<int>(_layerType_2)] = true;

		Scene* pActiveScene = SceneManager::GetActiveScene();

		std::vector<GameObject*> gameObjects_1 = pActiveScene->GetGameObjects(_layerType_1);
		for (GameObject* pGameObject : gameObjects_1)
		{
			pGameObject->GetComponent<RigidBody>()->SetOtherLayerInFilterData(_layerType_2);
		}
	}

	std::bitset<enums::LAYER_TYPE_COUNT> CollisionManager::GetCollisionGroup(enums::eLayerType _layerType)
	{
		return mArrColGroup[static_cast<int>(_layerType)];
	}

	void CollisionManager::ApplyForceInLayerAtLocation(enums::eLayerType _layerType, const math::Vector3& _location, const math::Vector3& _volume, const math::Vector3& _force)
	{
	}

	void CollisionManager::ApplyForceInLayerFromDot(enums::eLayerType _layerType, const math::Vector3& _location, const math::Vector3& _force)
	{
	}

	void CollisionManager::EnableRaycast(uint32_t _leftLayerType, uint32_t _rightLayerType, bool _enable)
	{
		Physics* physics = Application::GetPhysics();
		physics->EnableRaycast(_leftLayerType, _rightLayerType, _enable);
	}
	const math::Vector3& CollisionManager::GetMouseRayHitPos(enums::eLayerType _layerType, math::Vector3 _position )
	{
		mRayResult = {};

		Camera* camera = renderer::mainCamera;
		math::Vector3 cameraPos = camera->GetOwner()->GetPosition();
		math::Vector2 mousePos = Input::GetMouseWinPosition();

		CollisionManager::Raycast(static_cast<UINT>(_layerType), cameraPos, mousePos, 100.0f, &mRayResult);
		return mRayResult.hitPosition;
	}

	const GameObject* CollisionManager::GetMouseRayHitObj(enums::eLayerType _layerType, math::Vector3 _position)
	{
		mRayResult = {};

		Camera* camera = renderer::mainCamera;
		math::Vector3 cameraPos = camera->GetOwner()->GetPosition();
		math::Vector2 mousePos = Input::GetMouseWinPosition();

		CollisionManager::Raycast(static_cast<UINT>(_layerType), cameraPos, mousePos, 100.0f, &mRayResult);
		return mRayResult.gameObject;
	}

	bool CollisionManager::Raycast(uint32_t _layerType, const math::Vector3& _origin, const math::Vector2& _mousePos, float _maxDistance, tRaycastHit* _outHit)
	{
		float windowX = static_cast<float>(application.GetWidth());
		float windowY = static_cast<float>(application.GetHeight());

		float ndcX = (2.0f * _mousePos.x / windowX) - 1.0f;
		float ndcY = 1.0f - (2.0f * _mousePos.y / windowY);

		// Create the NDC position with z = --1.0
		Vector4 ndcPos = { ndcX, ndcY, 0.f, 1.0f };
		//DirectX::XMFLOAT4 ndcPos(ndcX, ndcY, 0.f, 1.0f);
		//DirectX::XMFLOAT4 ndcPos2(ndcX, ndcY, 1.f, 1.0f);

		// NDC position to WorldSpace
		const Matrix& invProj = renderer::mainCamera->GetProjectionMatrix().Invert();
		const Matrix& invView = renderer::mainCamera->GetViewMatrix().Invert();

		ndcPos = Vector4::Transform(ndcPos, invProj);
		ndcPos = Vector4::Transform(ndcPos, invView);

		// Camera Position
		const Vector3& RayOrigin = renderer::mainCamera->GetOwner()->GetPosition();
		// 마우스까지의 방향
		Vector3 RayDirection = Vector3(ndcPos.x - RayOrigin.x, ndcPos.y - RayOrigin.y, ndcPos.z - RayOrigin.z);
		RayDirection.Normalize();

		Physics* physics = Application::GetPhysics();
		return physics->Raycast(_layerType, _origin, RayDirection, _maxDistance, _outHit);
	}
	void CollisionManager::DrawRaycast(const math::Vector3& _origin, const math::Vector3& _direction, float _maxDistance, const math::Vector3& _color)
	{
		// 아직 구현 X
		//Vector3 forward{};
		//_direction.Normalize(forward);
		//const Matrix world = Matrix::CreateWorld(_origin, forward, Vector3{ 0.f, 1.f, 0.f });

		//renderer::TransformCB buffer{};
		//buffer.world = Matrix::CreateScale(_maxDistance) * world;
		//const Camera* camera = renderer::mainCamera;
		//buffer.view = camera->GetGpuViewMatrix();
		//buffer.projection = camera->GetGpuProjectionMatrix();

		//ConstantBuffer* cb = renderer::constantBuffers[(UINT)eCBType::Transform];
		//cb->SetData(&buffer);
		//cb->Bind(graphics::eShaderStage::ALL);

		//static std::shared_ptr<Mesh>   line = Resources::Find<Mesh>(L"LineMesh");
		//static std::shared_ptr<Shader> shader = Resources::Find<Shader>(L"DebugShader");

		//shader->SetDSState(eDSType::NoWrite);
		//shader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
		//shader->Binds();
		//line->Render(0);
		//shader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		//shader->SetDSState(eDSType::Less);
	}
}
