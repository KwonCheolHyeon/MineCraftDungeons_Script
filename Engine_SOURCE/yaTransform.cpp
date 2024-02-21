#include "yaTransform.h"
#include "yaGameObject.h"
#include "yaRenderer.h"
#include "yaCamera.h"
#include "yaRigidBody.h"
#include "yaTime.h"
#include "yaApplication.h"

extern md::Application application;

namespace md
{
	Transform::Transform()
		: Component(eComponentType::Transform)
		, mForward(Vector3::Forward)
		, mRight(Vector3::Right)
		, mUp(Vector3::Up)
		, mScale(Vector3::One)
		, mRotation(Vector3::Zero)
		, mPosition(Vector3::One)
		, mParent(nullptr)
		, mParentSocket(nullptr)
	{		
	}

	Transform::~Transform()
	{
		mParentSocket = nullptr;
		mParent = nullptr;
	}

	void Transform::Initalize()
	{

	}

	void Transform::Update()
	{
		//실제 로직상 캐릭터이동 처리
	}

	void Transform::FixedUpdate()
	{
		if (true == IsPhysicsObject())
		{
			physx::PxTransform transform = GetOwner()->GetComponent<RigidBody>()->GetPhysicsTransform();
			if (GetOwner()->GetComponent<RigidBody>()->GetGeometryType() == eGeometryType::Capsule)
				mRotation.z = -90.0f;

			auto quat = math::Quaternion::CreateFromYawPitchRoll({ XMConvertToRadians(mRotation.y),XMConvertToRadians(mRotation.x) ,XMConvertToRadians(mRotation.z) });
			math::Matrix matRotation = math::Matrix::CreateFromQuaternion(quat);
			math::Matrix matScale = math::Matrix::CreateScale(mScale);

			math::Vector3 scale = GetOwner()->GetComponent<RigidBody>()->GetGeometrySize();
			matRotation *= math::Matrix::CreateFromQuaternion(transform.q);
			math::Matrix matTranslation = {};
			if (GetOwner()->GetComponent<RigidBody>()->GetGeometryType() == eGeometryType::Capsule)
				matTranslation = math::Matrix::CreateTranslation(math::Vector3(transform.p) + math::Vector3(mPosition.x, mPosition.y - scale.y /1.4f, mPosition.z));
			else
				matTranslation = math::Matrix::CreateTranslation(math::Vector3(transform.p) + mPosition);
			mWorld = matScale * matRotation * matTranslation;

			if (mParentSocket)
			{
				Matrix matSocketTransform = mParentSocket->mMatrix; // 소켓의 행렬
				Matrix matOffset = Matrix::CreateTranslation(mSocketOffset); // 오프셋 행렬
				math::Quaternion quat = math::Quaternion::CreateFromYawPitchRoll({ 
					XMConvertToRadians(mSocketOffsetRot.y),
					XMConvertToRadians(mSocketOffsetRot.x),
					XMConvertToRadians(mSocketOffsetRot.z)
					});
				Matrix matOffsetRot = math::Matrix::CreateFromQuaternion(quat); // 오프셋 회전 행렬

				// mWorld에 소켓의 트랜스폼을 반영
				//mWorld = mParentSocket->mMatrix * mWorld;
				mWorld *= matOffsetRot * matOffset * matSocketTransform;
			}


			mForward = Vector3::TransformNormal(Vector3::Forward, matRotation);
			mRight = Vector3::TransformNormal(Vector3::Right, matRotation);
			mUp = Vector3::TransformNormal(Vector3::Up, matRotation);
		}
		else
		{
			Matrix position;
			Matrix rotation;
			Matrix scale;

			if (GetOwner()->GetLayerType() == eLayerType::UI)
			{
				Vector2 windowSize = application.GetSize();
				Vector2 posCalibration = windowSize;

				Vector3 convertedPos = mPosition;
				convertedPos.x /= (posCalibration.x / 2.f);
				convertedPos.y /= (posCalibration.y / 2.f);


				Vector3 convertedScale = mScale;				
				convertedScale.x /= (posCalibration.x / 2.f);
				convertedScale.y /= (posCalibration.y / 2.f);


				// 회전 변환 행렬
				Vector3 radian(mRotation.x * (XM_PI / 180)
					, mRotation.y * (XM_PI / 180)
					, mRotation.z * (XM_PI / 180));

				rotation = Matrix::CreateRotationX(radian.x);
				rotation *= Matrix::CreateRotationY(radian.y);
				rotation *= Matrix::CreateRotationZ(radian.z);

				// 이동 변환 행렬
				position.Translation(convertedPos);

				// 크기 변환 행렬
				scale = Matrix::CreateScale(convertedScale);

				mWorld = scale * rotation * position;
			}
			else
			{
				scale = Matrix::CreateScale(mScale);

				// 회전 변환 행렬
				Vector3 radian(mRotation.x * (XM_PI / 180)
					, mRotation.y * (XM_PI / 180)
					, mRotation.z * (XM_PI / 180));

				rotation = Matrix::CreateRotationX(radian.x);
				rotation *= Matrix::CreateRotationY(radian.y);
				rotation *= Matrix::CreateRotationZ(radian.z);

				// 이동 변환 행렬
				position.Translation(mPosition);

				mWorld = scale * rotation * position;
			}
			
			mForward = Vector3::TransformNormal(Vector3::Forward, rotation);
			mRight = Vector3::TransformNormal(Vector3::Right, rotation);
			mUp = Vector3::TransformNormal(Vector3::Up, rotation);

			// 카메라 컴포넌트에서 세팅해준다
			// 뷰행렬 세팅
			// 프로젝션 행렬 세팅
			if (GetOwner()->GetLayerType() != eLayerType::UI)//UI랑 마우스는 제외
			{
				if (mParent)
				{
					mWorld *= mParent->mWorld;
				}
			}

			if (mParentSocket)
			{
				mWorld *= mParentSocket->mMatrix;
			}
		}
	}

	void Transform::PrevRender()
	{
	}

	void Transform::Render()
	{
	}

	void Transform::SetConstantBuffer()
	{
		renderer::TransformCB trCb = {};
		
		trCb.world = mWorld;
		trCb.inverseWorld = mWorld.Invert();
		trCb.view = Camera::GetGpuViewMatrix();
		trCb.inverseView = trCb.view.Invert();
		trCb.projection = Camera::GetGpuProjectionMatrix();

		ConstantBuffer* cb = renderer::constantBuffers[(UINT)eCBType::Transform];
		cb->SetData(&trCb);
		cb->Bind(eShaderStage::VS);
		cb->Bind(eShaderStage::HS);
		cb->Bind(eShaderStage::DS);
		cb->Bind(eShaderStage::GS);
		cb->Bind(eShaderStage::PS);
		cb->Bind(eShaderStage::CS);
	}

	void Transform::SetPosition(const Vector3& position)
	{
		if (true == IsPhysicsObject())
		{
			RigidBody* rigid = GetOwner()->GetComponent<RigidBody>();
			physx::PxTransform transform = rigid->GetActor<physx::PxRigidActor>()->getGlobalPose();
			transform.p = position;

			//rigid->GetActor<physx::PxRigidActor>()->setGlobalPose(transform);
			eActorType eActorType = rigid->GetActorType();
			if (eActorType::Kinematic == eActorType)
				rigid->GetDynamicActor()->setKinematicTarget(transform);
			else if (eActorType::Dynamic == eActorType)
				rigid->GetDynamicActor()->setGlobalPose(transform);
			else if (eActorType::Static == eActorType)
				rigid->GetStaticActor()->setGlobalPose(transform);

		}
		else
		{
			mPosition = position;
		}
	}

	void Transform::SetRotation(const Vector3& rotation)
	{
		if (true == IsPhysicsObject())
		{
			RigidBody* rigid = GetOwner()->GetComponent<RigidBody>();
			physx::PxTransform transform = rigid->GetActor<physx::PxRigidActor>()->getGlobalPose();
			math::Quaternion quat = math::Quaternion::CreateFromYawPitchRoll({ XMConvertToRadians(mRotation.y),XMConvertToRadians(mRotation.x) ,XMConvertToRadians(mRotation.z) });

			transform.q = physx::PxQuatT<float>(quat.x, quat.y, quat.z, quat.w);
			rigid->GetActor<physx::PxRigidActor>()->setGlobalPose(transform);
		}
		else
		{
			mRotation = rotation;
		}
	}

	math::Vector3 Transform::GetPhysicalLocalPosition()
	{
		RigidBody* rigidBody = GetOwner()->GetComponent<RigidBody>();
		physx::PxTransform transform = rigidBody->GetPhysicsTransform();
		math::Vector3 physxPos = transform.p;
		return physxPos;
	}

	void Transform::SetPhysicalLocalPosition(math::Vector3 position)
	{
		RigidBody* rigidBody = GetOwner()->GetComponent<RigidBody>();
		physx::PxTransform transform = rigidBody->GetPhysicsTransform();
		transform.p = position;
		rigidBody->SetPhysicsTransform(transform);
	}
	void Transform::SetPhysicalRotation(const math::Vector3& rotation)
	{
		assert(GetOwner()->GetComponent<RigidBody>());

		physx::PxQuat rotX(physx::PxPi * rotation.x / 180.0f, physx::PxVec3(1.0f, 0.0f, 0.0f));
		physx::PxQuat rotY(physx::PxPi * rotation.y / 180.0f, physx::PxVec3(0.0f, 1.0f, 0.0f));
		physx::PxQuat rotZ(physx::PxPi * rotation.z / 180.0f, physx::PxVec3(0.0f, 0.0f, 1.0f));
		
		physx::PxQuat rot = rotX * rotY * rotZ;
		RigidBody* rigid = GetOwner()->GetComponent<RigidBody>();
		physx::PxTransform tr = rigid->GetActor<physx::PxRigidActor>()->getGlobalPose();
		tr.q = rot;
		GetOwner()->GetComponent<RigidBody>()->GetActor<physx::PxRigidActor>()->setGlobalPose(tr);
	}
	void Transform::Move(const math::Vector3& _velocity)
	{
		if (true == IsPhysicsObject())
		{
			RigidBody* rigidBody = GetOwner()->GetComponent<RigidBody>();
			physx::PxTransform transform = rigidBody->GetPhysicsTransform();
			transform.p += _velocity * Time::DeltaTime();
			eActorType eActorType = rigidBody->GetActorType();

			if (eActorType::Kinematic == eActorType)
				rigidBody->GetDynamicActor()->setKinematicTarget(transform);
			else if (eActorType::Dynamic == eActorType)
				rigidBody->GetDynamicActor()->setGlobalPose(transform);
			else
				AssertEx(false, L"Transform::Move() - Static Actor에 대한 Move 호출");
		}
		else
		{
			mPosition += _velocity * Time::DeltaTime();
		}
	}
}
