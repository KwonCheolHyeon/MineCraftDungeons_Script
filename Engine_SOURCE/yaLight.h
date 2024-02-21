#pragma once
#include "yaComponent.h"
#include "yaGraphics.h"
#include "yaMesh.h"

namespace md
{
	using namespace graphics;
	class Light : public Component
	{
	public:
		Light();
		~Light();

		virtual void Initalize() override;
		virtual void Update() override;
		virtual void FixedUpdate() override;
		void Decompose() ;
		virtual void Render() override;
		void SetType(eLightType type);

		graphics::LightAttribute GetLightAttribute() { return mAttribute; }

		void SetDiffuse(Vector4 diffuse) { mAttribute.diffuse = diffuse; }
		void SetSpecular(Vector4 spec) { mAttribute.specular = spec; }
		void SetAmbient(Vector4 ambient) { mAttribute.ambient = ambient; }
		void SetRadius(float radius) { mAttribute.radius = radius; }
		void SetAngle(float angle) { mAttribute.angle = angle; }
		Vector4 GetDiffuse() { mAttribute.diffuse; }
		eLightType GetType() { return (eLightType)mAttribute.type; }
		float GetRadius() { mAttribute.radius; }
		float GetAngle() { mAttribute.angle; }
		void SetIndex(UINT idx) { mIndex = idx; }

	private:
		graphics::LightAttribute mAttribute;
		std::shared_ptr<class Mesh> mVolumeMesh; // 광원의 범위를 표현하는 메쉬
		UINT mIndex;
	};
}
