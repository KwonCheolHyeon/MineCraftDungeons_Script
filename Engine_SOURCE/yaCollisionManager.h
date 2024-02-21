#pragma once
#include "CommonInclude.h"
#include "yaCollider3D.h"
#include "mdStruct.h"
namespace md
{
	union union_ColliderID
	{
		struct
		{
			UINT32 left;
			UINT32 right;
		};
		UINT64 id;
	};

	class CollisionManager
	{
		friend class Application;

	public:
		static void Update() {};
		static void SetCollisionGroup(enums::eLayerType _layerType_1, enums::eLayerType _layerType_2);
		static std::bitset<enums::LAYER_TYPE_COUNT> GetCollisionGroup(enums::eLayerType _layerType);

		static void ApplyForceInLayerAtLocation(enums::eLayerType _layerType, const math::Vector3& _location, const math::Vector3& _volume, const math::Vector3& _force);
		static void ApplyForceInLayerFromDot(enums::eLayerType _layerType, const math::Vector3& _location, const math::Vector3& _force);

		// Raycast
		//_origin: 레이의 시작점
		//_direction : 레이의 방향
		//_maxDistance : 레이가 검사할 거리
		//_outHit : 레이가 어떤 물체와 충돌했을 때, 그 충돌 정보를 저장할 구조체
		static bool Raycast(uint32_t _layerType, const math::Vector3& _origin, const math::Vector2& _mousePos, float _maxDistance, tRaycastHit* _outHit);
		static void EnableRaycast(uint32_t _leftLayerType, uint32_t _rightLayerType, bool _enable);
		static void DrawRaycast(const math::Vector3& _origin, const math::Vector3& _direction, float _maxDistance, const math::Vector3& _color = math::Vector3{ 1.f, 0.f, 0.f });
		
		static const math::Vector3& GetMouseRayHitPos(enums::eLayerType _layerType, math::Vector3 _position = math::Vector3::Zero);
		static const GameObject* GetMouseRayHitObj(enums::eLayerType _layerType, math::Vector3 _position = math::Vector3::Zero);
		//static void Update();
		//static void FixedUpdate();
		//static void Render();
		//static void CollisionLayerCheck(define::eLayerType _left, define::eLayerType _right, bool _enable = true);
		//static void LayerCollision(class IScene* _scene, define::eLayerType _left, define::eLayerType _right);
		//static void ColliderCollision(ICollider2D* _left, ICollider2D* _right);
		//static bool Intersect(ICollider2D* _left, ICollider2D* _right);

	private:
		static void Init() {};
		static void Release() {};

	private:
		static std::array<std::bitset<enums::LAYER_TYPE_COUNT>, enums::LAYER_TYPE_COUNT> mArrColGroup;

		static std::bitset<(UINT)enums::eLayerType::End> mLayerCollisionMatrix[(UINT)enums::eLayerType::End];
		static std::map<UINT64, bool> mCollisionMap;

		static std::array<std::bitset<32>, 32> mRaycastMask;
		
		static tRaycastHit mRayResult;
	};
}
