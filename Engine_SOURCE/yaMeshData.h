#pragma once
#include "yaResource.h"
#include "yaMaterial.h"
#include "yaMesh.h"


namespace md
{
	class MeshData : public Resource
	{
	public:
		MeshData();
		~MeshData();

		static MeshData* LoadFromFbx(const std::wstring& path);

		virtual HRESULT Save(const std::wstring& path, FILE* file = nullptr);
		virtual HRESULT Load(const std::wstring& path, FILE* file = nullptr);

		GameObject* Instantiate(enums::eLayerType _layerType);
		std::shared_ptr<Mesh> GetMesh() { return mMesh; }
		std::vector<std::shared_ptr<Material>>* GetMaterials() { return &mMaterials; }

	private:
		std::shared_ptr<Mesh> mMesh;
		std::vector<std::shared_ptr<Material>> mMaterials;
	};
}
