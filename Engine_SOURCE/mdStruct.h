#pragma once
#include "yaEnums.h"
#include "yaMath.h"

namespace md
{
	// ============
	// PhysX
	// ============
	struct tMassProperties
	{
		tMassProperties(float _staticFriction = 0.f, float _dynamicFriction = 0.f, float _restitution = 0.f)
			: staticFriction(_staticFriction)
			, dynamicFriction(_dynamicFriction)
			, restitution(_restitution)
		{ }

		float staticFriction;
		float dynamicFriction;
		float restitution;
	};

	struct Geometries
	{
		Geometries(enums::eGeometryType _geometryType, math::Vector3 vBoxHalfSize)
			: eGeomType(enums::eGeometryType::Box)
		{
			if (enums::eGeometryType::Box == _geometryType)
			{
				boxGeom = physx::PxBoxGeometry(vBoxHalfSize);
			}
		}

		Geometries(enums::eGeometryType _geometryType, float fRadius, float fHalfHeight)
			: eGeomType(enums::eGeometryType::Capsule)
		{
			if (enums::eGeometryType::Capsule == _geometryType)
			{
				capsuleGeom = physx::PxCapsuleGeometry(fRadius, fHalfHeight);
			}
		}

		Geometries(enums::eGeometryType _geometryType, float fRadius)
			: eGeomType(enums::eGeometryType::Sphere)
		{
			if (enums::eGeometryType::Sphere == _geometryType)
			{
				sphereGeom = physx::PxSphereGeometry(fRadius);
			}
		}

		Geometries(enums::eGeometryType _geometryType)
			: eGeomType(enums::eGeometryType::Plane)
		{
			// RigidStatic일 떄,
			if (enums::eGeometryType::Plane == _geometryType)
			{
				planeGeom = physx::PxPlaneGeometry();
			}
		}

		physx::PxBoxGeometry boxGeom;
		physx::PxCapsuleGeometry capsuleGeom;
		physx::PxPlaneGeometry planeGeom;
		physx::PxSphereGeometry sphereGeom;
		enums::eGeometryType eGeomType;
	};

	struct tPhysicsInfo
	{
		tPhysicsInfo()
			: eActorType(enums::eActorType::Static)
			, eGeomType(enums::eGeometryType::Box)
			, size(math::Vector3(1.f, 1.f, 1.f))
			, massProperties(tMassProperties())
			, pGeometries(nullptr)
			, filterData{}
		{
		}

		enums::eActorType eActorType;
		enums::eGeometryType eGeomType;
		math::Vector3 size;
		tMassProperties massProperties;
		Geometries* pGeometries;
		physx::PxFilterData filterData;
	};

	class GameObject;
	struct tRaycastHit
	{
		GameObject* gameObject;
		math::Vector3 hitPosition;
		math::Vector3 hitNormal;
		float hitDistance;
		bool  hasBlocking;
	};

	/// <summary>
	/// Modified by HJ.Joo, 2023_09_20
	/// yaAnimator.h -> mdStruct.h
	/// reason : This struct needs to be used in both Animator and BoneAnimator class.
	/// </summary>
	struct Events
	{
		struct Event
		{
			void operator=(std::function<void()> func)
			{
				mEvent = std::move(func);
			}
			void operator()()
			{
				if (mEvent && (!mbIsExecute))
				{
					mEvent();
					mbIsExecute = true;
				}
			}
			bool mbIsExecute;

			std::function<void()> mEvent;
		};

		void ResetExecute()
		{
			mStartEvent.mbIsExecute = false;
			mCompleteEvent.mbIsExecute = false;
			mEndEvent.mbIsExecute = false;

			size_t frameEventSize = mEvents.size();
			for (size_t frameEventIndex = 0; frameEventIndex < frameEventSize; ++frameEventIndex)
			{
				mEvents[frameEventIndex].mbIsExecute = false;
			}
		}


		Event mStartEvent;
		Event mCompleteEvent;
		Event mEndEvent;

		std::vector<Event> mEvents;
	};

	struct tBoneSocket
	{
		UINT mBoneIndex;

		math::Vector3 mPosition;
		math::Quaternion mRotation;
		math::Matrix mMatrix;

		void DecomposeMatrix(math::Matrix _ownerMatrix, math::Matrix _socketMatrix)
		{
			mMatrix = _socketMatrix * _ownerMatrix;

			//math::Vector3 scale;
			//
			//math::Vector3 ownerScale;
			//math::Vector3 ownerPos;
			//math::Quaternion ownerQut;
			//
			//_ownerMatrix.Decompose(ownerScale, ownerQut, ownerPos);
			//
			//math::Matrix socketMatrix = _socketMatrix;
			//
			//math::Vector3 socketScale;
			//math::Vector3 socketPos;
			//math::Quaternion socketQut;
			//
			//socketMatrix.Decompose(socketScale, socketQut, socketPos);
			//
			//math::Matrix translation = math::Matrix::CreateTranslation(socketPos + ownerPos);
			//math::Matrix rotation = math::Matrix::CreateFromQuaternion(socketQut);
			//math::Matrix scaleMat = math::Matrix::CreateScale(1.f);
			//
			//mMatrix = scaleMat * rotation * translation;
			//mMatrix.Decompose(scale, mRotation, mPosition);
		}
	};
}
