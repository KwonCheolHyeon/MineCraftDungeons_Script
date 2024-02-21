#include "yaMeshData.h"
#include "yaGameObject.h"
#include "yaFbxLoader.h"
#include "yaResources.h"
#include "yaObject.h"
#include "yaMeshRenderer.h"
#include "yaBoneAnimator.h"

namespace md
{
	MeshData::MeshData()
		: Resource(eResourceType::MeshData)
	{

	}
	MeshData::~MeshData()
	{
		
	}

	MeshData* MeshData::LoadFromFbx(const std::wstring& path)
	{
		std::filesystem::path parentPath = std::filesystem::current_path().parent_path();
		std::wstring fullPath = parentPath.wstring() + L"\\Resources\\" + path;

		const std::wstring meshDataName
			= std::filesystem::path(fullPath).stem();
		std::shared_ptr<MeshData> data = Resources::Find<MeshData>(meshDataName);
		if (data)
		{
			return data.get();
		}


		FbxLoader loader;
		loader.Initialize();
		loader.LoadFbx(fullPath);

		// 메시 가져오기
		std::shared_ptr<Mesh> mesh = nullptr;
		mesh = Mesh::CreateFromContainer(&loader);

		// 리소스에 넣어주기

		std::wstring name = std::filesystem::path(fullPath).stem();
		name += L".mesh";
		Resources::Insert(name, mesh);

		
		//mesh->Save(name);

		// 메테리얼 가져오기

		std::vector<std::shared_ptr<Material>> materials = {};
		for (size_t i = 0; i < loader.GetContainer(0).materials.size(); i++)
		{
			std::shared_ptr<Material> material
				= Resources::Find<Material>(loader.GetContainer(0).materials[i].name);

			materials.push_back(material);
		}

		std::shared_ptr<MeshData> meshData = std::make_shared<MeshData>();
		meshData->mMesh = mesh;
		meshData->mMaterials = materials;

		Resources::Insert(meshDataName, meshData);

		meshData->Save(name);

		return meshData.get();
	}

	HRESULT MeshData::Save(const std::wstring& path, FILE* file)
	{
		std::wstring name = std::filesystem::path(path).stem();
		name += L".meshdata";

		std::filesystem::path parentPath = std::filesystem::current_path().parent_path();
		std::wstring fullPath = parentPath.wstring() + L"\\Resources\\MeshData\\" + name;

		file = nullptr;
		_wfopen_s(&file, fullPath.c_str(), L"wb");
		if (file == nullptr)
			return S_FALSE;

		mMesh->Save(name, file);

		for (size_t i = 0; i < mMaterials.size(); i++)
		{
			std::wstring matName = mMaterials[i]->GetName();
			

			mMaterials[i]->Save(name, file);
		}

		fclose(file);

		return S_OK;
	}
	HRESULT MeshData::Load(const std::wstring& path, FILE* file)
	{
		std::wstring name = std::filesystem::path(path).stem();
		name += L".meshdata";

		std::filesystem::path parentPath = std::filesystem::current_path().parent_path();
		std::wstring fullPath = parentPath.wstring() + L"\\Resources\\MeshData\\" + name;

		file = nullptr;
		_wfopen_s(&file, fullPath.c_str(), L"rb");
		if (file == nullptr)
			return S_FALSE;

		mMesh = std::make_shared<Mesh>();
		mMesh->Load(name, file);

		UINT matCount = mMesh->GetSubSetCount();
		mMaterials.resize(matCount);
		for (size_t i = 0; i < matCount; i++)
		{
			mMaterials[i] = std::make_shared<Material>();
			mMaterials[i]->Load(name, file);
			mMaterials[i]->SetRenderingMode(eRenderingMode::DefferdOpaque);
		}

		fclose(file);

		return S_OK;
	}
	GameObject* MeshData::Instantiate(enums::eLayerType _layerType)
	{
		GameObject* gameObj = object::Instantiate<GameObject>(_layerType);
		MeshRenderer* mr = gameObj->AddComponent<MeshRenderer>();
		mr->SetMesh(mMesh);
		
		for (size_t i = 0; i < mMaterials.size(); i++)
		{
			mr->SetMaterial(mMaterials[i], static_cast<int>(i));
		}

		// Animation 파트 
		if (mMesh->IsAnimMesh())
		{
			BoneAnimator* animator = gameObj->AddComponent<BoneAnimator>();
			animator->SetBones(mMesh->GetBones());
			animator->SetAnimaitionClip(const_cast<std::vector<BoneAnimationClip>*>(mMesh->GetAnimClip()));
			animator->SetAnimEvents(mMesh->GetEvents());
		}

		return gameObj;
	}
}
