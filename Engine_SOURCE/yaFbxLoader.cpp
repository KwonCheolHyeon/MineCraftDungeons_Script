#include "yaFbxLoader.h"
#include "yaResources.h"
#include "yaTexture.h"
#include "yaMaterial.h"


namespace md
{
	fbxsdk::FbxManager* FbxLoader::mManager = nullptr;
	fbxsdk::FbxScene* FbxLoader::mScene = nullptr;
	fbxsdk::FbxImporter* FbxLoader::mImporter = nullptr;
	std::vector<Container> FbxLoader::mContainers = {};
	std::vector<Bone*> FbxLoader::mBones = {};
	fbxsdk::FbxArray<fbxsdk::FbxString*> FbxLoader::mAnimationNames = {};
	std::vector<AnimationClip*> FbxLoader::mAnimationClips = {};

	FbxLoader::~FbxLoader()
	{
		if (mScene)
		{
			mScene->Destroy();
		}

		// 매니저 안에서 모든 메모리를 해제 해준다.
		if (mManager)
		{
			mManager->Destroy();
		}

		{
			size_t Size = mBones.size();
			for (size_t index = 0; index < Size; ++index)
			{
				if (mBones[index])
				{
					SAFE_DELETE(mBones[index]);
				}
			}

			mBones.clear();
		}
		{
			size_t Size = mAnimationClips.size();
			for (size_t index = 0; index < Size; ++index)
			{
				if (mAnimationClips[index])
				{
					SAFE_DELETE(mAnimationClips[index]);
				}
			}

			mAnimationClips.clear();
		}
		
		{
			size_t Size = mAnimationNames.Size();
			for (size_t index = 0; index < Size; ++index)
			{
				if (mAnimationNames[index])
				{
					SAFE_DELETE(mAnimationNames[index]);
				}
			}

			mAnimationNames.Clear();
		}
	}

	Matrix GetMatrixFromFbxMatrix(fbxsdk::FbxAMatrix& _mat)
	{
		Matrix mat;
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				mat.m[i][j] = (float)_mat.Get(i, j);
			}
		}
		return mat;
	}

	void FbxLoader::Initialize()
	{
		mManager = fbxsdk::FbxManager::Create();
		fbxsdk::FbxIOSettings* ioSettings
			= fbxsdk::FbxIOSettings::Create(mManager, IOSROOT);
		mManager->SetIOSettings(ioSettings);
		mScene = fbxsdk::FbxScene::Create(mManager, "");
		mImporter = fbxsdk::FbxImporter::Create(mManager, "");
	}

	bool FbxLoader::LoadFbx(const std::wstring& path)
	{
		mContainers.clear();

		std::string strPath = WStringToString(path);

		if (!mImporter->Initialize(strPath.c_str(), -1, mManager->GetIOSettings()))
			return false;
	
		mImporter->Import(mScene);

		mScene->GetGlobalSettings().SetAxisSystem(fbxsdk::FbxAxisSystem::Max);

		// Bone 정보 읽기
		LoadSkeleton(mScene->GetRootNode());

		// Animation 이름정보 
		mScene->FillAnimStackNameArray(mAnimationNames);

		// Animation Clip 정보
		LoadAnimationClip();

		// 삼각화(Triangulate)
		Triangulate(mScene->GetRootNode());

		// 메쉬 데이터 얻기
		LoadMeshDataFromNode(mScene->GetRootNode());

		mImporter->Destroy();

		// 필요한 텍스쳐 로드
		LoadTexture();

		// 필요한 메테리얼 생성
		CreateMaterial();

		return true;
	}

	void FbxLoader::LoadMeshDataFromNode(fbxsdk::FbxNode* node)
	{
		// 노드의 메쉬정보 읽기
		fbxsdk::FbxNodeAttribute* nodeAttribute = node->GetNodeAttribute();

		if (nodeAttribute && fbxsdk::FbxNodeAttribute::eMesh == nodeAttribute->GetAttributeType())
		{
			fbxsdk::FbxAMatrix globalMatrix = node->EvaluateGlobalTransform();
			globalMatrix.GetR();

			fbxsdk::FbxMesh* mesh = node->GetMesh();
			if (NULL != mesh)
				LoadMesh(mesh);
		}

		// 해당 노드의 재질정보 읽기
		UINT materialCount = node->GetMaterialCount();

		if (materialCount > 0)
		{
			for (UINT i = 0; i < materialCount; ++i)
			{
				fbxsdk::FbxSurfaceMaterial* surface = node->GetMaterial(i);
				LoadMaterial(surface);
			}
		}

		// 자식 노드 정보 읽기
		int childCount = node->GetChildCount();
		for (int i = 0; i < childCount; ++i)
			LoadMeshDataFromNode(node->GetChild(i));
	}

	void FbxLoader::LoadMesh(fbxsdk::FbxMesh* _pFbxMesh)
	{
		mContainers.push_back(Container{});
		Container& container = mContainers[mContainers.size() - 1];

		std::string strName = _pFbxMesh->GetName();
		container.name = std::wstring(strName.begin(), strName.end());

		int iVtxCnt = _pFbxMesh->GetControlPointsCount();
		container.Resize(iVtxCnt);

		fbxsdk::FbxVector4* pFbxPos = _pFbxMesh->GetControlPoints();

		for (int i = 0; i < iVtxCnt; ++i)
		{
			container.positions[i].x = (float)pFbxPos[i].mData[0];
			container.positions[i].y = (float)pFbxPos[i].mData[2];
			container.positions[i].z = (float)pFbxPos[i].mData[1];
		}

		// 폴리곤 개수
		int iPolyCnt = _pFbxMesh->GetPolygonCount();

		// 재질의 개수 ( ==> SubSet 개수 ==> Index Buffer Count)
		int iMtrlCnt = _pFbxMesh->GetNode()->GetMaterialCount();
		container.indices.resize(iMtrlCnt);

		// 정점 정보가 속한 subset 을 알기위해서...
		fbxsdk::FbxGeometryElementMaterial* pMtrl = _pFbxMesh->GetElementMaterial();

		// 폴리곤을 구성하는 정점 개수
		int iPolySize = _pFbxMesh->GetPolygonSize(0);
		if (3 != iPolySize)
			assert(NULL); // Polygon 구성 정점이 3개가 아닌 경우

		UINT arrIdx[3] = {};
		UINT iVtxOrder = 0; // 폴리곤 순서로 접근하는 순번

		for (int i = 0; i < iPolyCnt; ++i)
		{
			for (int j = 0; j < iPolySize; ++j)
			{
				// i 번째 폴리곤에, j 번째 정점
				int iIdx = _pFbxMesh->GetPolygonVertex(i, j);
				arrIdx[j] = iIdx;

				GetTangent(_pFbxMesh, &container, iIdx, iVtxOrder);
				GetBinormal(_pFbxMesh, &container, iIdx, iVtxOrder);
				GetNormal(_pFbxMesh, &container, iIdx, iVtxOrder);
				GetUV(_pFbxMesh, &container, iIdx, _pFbxMesh->GetTextureUVIndex(i, j));

				++iVtxOrder;
			}
			UINT iSubsetIdx = pMtrl->GetIndexArray().GetAt(i);
			container.indices[iSubsetIdx].push_back(arrIdx[0]);
			container.indices[iSubsetIdx].push_back(arrIdx[2]);
			container.indices[iSubsetIdx].push_back(arrIdx[1]);
		}

		LoadAnimationData(_pFbxMesh, &container);
	}

	void FbxLoader::LoadMaterial(fbxsdk::FbxSurfaceMaterial* _pMtrlSur)
	{
		FbxMaterial materialInfo{};

		std::string str = _pMtrlSur->GetName();
		materialInfo.name = std::wstring(str.begin(), str.end());

		// Diff
		materialInfo.color.diffuseColor = GetMtrlData(_pMtrlSur
			, fbxsdk::FbxSurfaceMaterial::sDiffuse
			, fbxsdk::FbxSurfaceMaterial::sDiffuseFactor);

		// Amb
		materialInfo.color.AmbientColor = GetMtrlData(_pMtrlSur
			, fbxsdk::FbxSurfaceMaterial::sAmbient
			, fbxsdk::FbxSurfaceMaterial::sAmbientFactor);

		// Spec
		materialInfo.color.specularColor = GetMtrlData(_pMtrlSur
			, fbxsdk::FbxSurfaceMaterial::sSpecular
			, fbxsdk::FbxSurfaceMaterial::sSpecularFactor);

		// Emisv
		materialInfo.color.EmessiveColor = GetMtrlData(_pMtrlSur
			, fbxsdk::FbxSurfaceMaterial::sEmissive
			, fbxsdk::FbxSurfaceMaterial::sEmissiveFactor);

		// Texture Name
		materialInfo.diffuse = GetMtrlTextureName(_pMtrlSur, fbxsdk::FbxSurfaceMaterial::sDiffuse);
		materialInfo.normal = GetMtrlTextureName(_pMtrlSur, fbxsdk::FbxSurfaceMaterial::sNormalMap);
		materialInfo.specular = GetMtrlTextureName(_pMtrlSur, fbxsdk::FbxSurfaceMaterial::sSpecular);
		materialInfo.emissive = GetMtrlTextureName(_pMtrlSur, fbxsdk::FbxSurfaceMaterial::sEmissive);

		mContainers.back().materials.push_back(materialInfo);
	}
	void FbxLoader::GetTangent(fbxsdk::FbxMesh* _pMesh, Container* _pContainer, int _iIdx, int _iVtxOrder)
	{
		//int iTangentCnt = _pMesh->GetElementTangentCount();
		//if (1 != iTangentCnt)
		//   assert(NULL); // 정점 1개가 포함하는 탄젠트 정보가 2개 이상이다.

		//// 탄젠트 data 의 시작 주소
		//fbxsdk::FbxGeometryElementTangent* pTangent = _pMesh->GetElementTangent();
		//UINT iTangentIdx = 0;

		//if (pTangent->GetMappingMode() == fbxsdk::FbxGeometryElement::eByPolygonVertex)
		//{
		//   if (pTangent->GetReferenceMode() == fbxsdk::FbxGeometryElement::eDirect)
		//      iTangentIdx = _iVtxOrder;
		//   else
		//      iTangentIdx = pTangent->GetIndexArray().GetAt(_iVtxOrder);
		//}
		//else if (pTangent->GetMappingMode() == fbxsdk::FbxGeometryElement::eByControlPoint)
		//{
		//   if (pTangent->GetReferenceMode() == fbxsdk::FbxGeometryElement::eDirect)
		//      iTangentIdx = _iIdx;
		//   else
		//      iTangentIdx = pTangent->GetIndexArray().GetAt(_iIdx);
		//}

		//fbxsdk::FbxVector4 vTangent = pTangent->GetDirectArray().GetAt(iTangentIdx);

		//_pContainer->tangents[_iIdx].x = (float)vTangent.mData[0];
		//_pContainer->tangents[_iIdx].y = (float)vTangent.mData[2];
		//_pContainer->tangents[_iIdx].z = (float)vTangent.mData[1];

		// 메쉬로부터 ElementTangent 정보를 얻어온다.
		fbxsdk::FbxGeometryElementTangent* Tangent = _pMesh->GetElementTangent();

		if (nullptr == Tangent)
			return;

		//_pContainer.bBump = true;

		int   tangentIndex = _iVtxOrder;

		// MappingMode와 ReferenceMode에 따라서 인덱스로 사용할 정보가 달라진다.
		if (Tangent->GetMappingMode() == fbxsdk::FbxGeometryElement::eByPolygonVertex)
		{
			switch (Tangent->GetReferenceMode())
			{
			case fbxsdk::FbxGeometryElement::eIndexToDirect:
				tangentIndex = Tangent->GetIndexArray().GetAt(_iVtxOrder);
				break;
			}
		}

		else if (Tangent->GetMappingMode() == fbxsdk::FbxGeometryElement::eByControlPoint)
		{
			switch (Tangent->GetReferenceMode())
			{
			case fbxsdk::FbxGeometryElement::eDirect:
				tangentIndex = _iVtxOrder;
				break;
			case fbxsdk::FbxGeometryElement::eIndexToDirect:
				tangentIndex = Tangent->GetIndexArray().GetAt(_iVtxOrder);
				break;
			}
		}
	}
	void FbxLoader::GetBinormal(fbxsdk::FbxMesh* _pMesh, Container* _pContainer, int _iIdx, int _iVtxOrder)
	{
		//int iBinormalCnt = _pMesh->GetElementBinormalCount();
		//if (1 != iBinormalCnt)
		//   assert(NULL); // 정점 1개가 포함하는 종법선 정보가 2개 이상이다.
		//
		//// 종법선 data 의 시작 주소
		//fbxsdk::FbxGeometryElementBinormal* pBinormal = _pMesh->GetElementBinormal();
		//UINT iBinormalIdx = 0;
		//
		//if (pBinormal->GetMappingMode() == fbxsdk::FbxGeometryElement::eByPolygonVertex)
		//{
		//   if (pBinormal->GetReferenceMode() == fbxsdk::FbxGeometryElement::eDirect)
		//      iBinormalIdx = _iVtxOrder;
		//   else
		//      iBinormalIdx = pBinormal->GetIndexArray().GetAt(_iVtxOrder);
		//}
		//else if (pBinormal->GetMappingMode() == fbxsdk::FbxGeometryElement::eByControlPoint)
		//{
		//   if (pBinormal->GetReferenceMode() == fbxsdk::FbxGeometryElement::eDirect)
		//      iBinormalIdx = _iIdx;
		//   else
		//      iBinormalIdx = pBinormal->GetIndexArray().GetAt(_iIdx);
		//}
		//
		//fbxsdk::FbxVector4 vBinormal = pBinormal->GetDirectArray().GetAt(iBinormalIdx);
		//
		//_pContainer->binormals[_iIdx].x = (float)vBinormal.mData[0];
		//_pContainer->binormals[_iIdx].y = (float)vBinormal.mData[2];
		//_pContainer->binormals[_iIdx].z = (float)vBinormal.mData[1];

		// 메쉬로부터 ElementBinormal 정보를 얻어온다.
		fbxsdk::FbxGeometryElementBinormal* Binormal = _pMesh->GetElementBinormal();

		if (!Binormal)
			return;

		//_container.bBump = true;

		int   BinormalIndex = _iVtxOrder;

		// MappingMode와 ReferenceMode에 따라서 인덱스로 사용할 정보가 달라진다.
		if (Binormal->GetMappingMode() == fbxsdk::FbxGeometryElement::eByPolygonVertex)
		{
			switch (Binormal->GetReferenceMode())
			{
			case fbxsdk::FbxGeometryElement::eIndexToDirect:
				BinormalIndex = Binormal->GetIndexArray().GetAt(_iVtxOrder);
				break;
			}
		}

		else if (Binormal->GetMappingMode() == fbxsdk::FbxGeometryElement::eByControlPoint)
		{
			switch (Binormal->GetReferenceMode())
			{
			case fbxsdk::FbxGeometryElement::eDirect:
				BinormalIndex = _iVtxOrder;
				break;
			case fbxsdk::FbxGeometryElement::eIndexToDirect:
				BinormalIndex = Binormal->GetIndexArray().GetAt(_iVtxOrder);
				break;
			}
		}

		fbxsdk::FbxVector4   vBinormal = Binormal->GetDirectArray().GetAt(BinormalIndex);

		_pContainer->binormals[_iIdx].x = (float)vBinormal.mData[0];
		_pContainer->binormals[_iIdx].y = (float)vBinormal.mData[2];
		_pContainer->binormals[_iIdx].z = (float)vBinormal.mData[1];
	}
	void FbxLoader::GetNormal(fbxsdk::FbxMesh* _pMesh, Container* _pContainer, int _iIdx, int _iVtxOrder)
	{
		int iNormalCnt = _pMesh->GetElementNormalCount();
		if (1 != iNormalCnt)
			assert(NULL); // 정점 1개가 포함하는 종법선 정보가 2개 이상이다.

		// 종법선 data 의 시작 주소
		fbxsdk::FbxGeometryElementNormal* pNormal = _pMesh->GetElementNormal();
		UINT iNormalIdx = 0;

		if (pNormal->GetMappingMode() == fbxsdk::FbxGeometryElement::eByPolygonVertex)
		{
			if (pNormal->GetReferenceMode() == fbxsdk::FbxGeometryElement::eDirect)
				iNormalIdx = _iVtxOrder;
			else
				iNormalIdx = pNormal->GetIndexArray().GetAt(_iVtxOrder);
		}
		else if (pNormal->GetMappingMode() == fbxsdk::FbxGeometryElement::eByControlPoint)
		{
			if (pNormal->GetReferenceMode() == fbxsdk::FbxGeometryElement::eDirect)
				iNormalIdx = _iIdx;
			else
				iNormalIdx = pNormal->GetIndexArray().GetAt(_iIdx);
		}

		fbxsdk::FbxVector4 vNormal = pNormal->GetDirectArray().GetAt(iNormalIdx);

		_pContainer->normals[_iIdx].x = (float)vNormal.mData[0];
		_pContainer->normals[_iIdx].y = (float)vNormal.mData[2];
		_pContainer->normals[_iIdx].z = (float)vNormal.mData[1];
	}
	void FbxLoader::GetUV(fbxsdk::FbxMesh* _pMesh, Container* _pContainer, int _iIdx, int _iUVIndex)
	{
		fbxsdk::FbxGeometryElementUV* pUV = _pMesh->GetElementUV();

		UINT iUVIdx = 0;
		if (pUV->GetReferenceMode() == fbxsdk::FbxGeometryElement::eDirect)
			iUVIdx = _iIdx;
		else
			iUVIdx = pUV->GetIndexArray().GetAt(_iIdx);

		iUVIdx = _iUVIndex;
		fbxsdk::FbxVector2 vUV = pUV->GetDirectArray().GetAt(iUVIdx);
		_pContainer->uv[_iIdx].x = (float)vUV.mData[0];
		_pContainer->uv[_iIdx].y = 1.f - (float)vUV.mData[1]; // fbx uv 좌표계는 좌하단이 0,0
	}
	Vector4 FbxLoader::GetMtrlData(fbxsdk::FbxSurfaceMaterial* _pSurface, const char* _pMtrlName, const char* _pMtrlFactorName)
	{
		fbxsdk::FbxDouble3  vMtrl;
		fbxsdk::FbxDouble	dFactor = 0.;

		fbxsdk::FbxProperty tMtrlProperty = _pSurface->FindProperty(_pMtrlName);
		fbxsdk::FbxProperty tMtrlFactorProperty = _pSurface->FindProperty(_pMtrlFactorName);

		if (tMtrlProperty.IsValid() && tMtrlFactorProperty.IsValid())
		{
			vMtrl = tMtrlProperty.Get<fbxsdk::FbxDouble3>();
			dFactor = tMtrlFactorProperty.Get<fbxsdk::FbxDouble>();
		}

		Vector4 vRetVal = Vector4((float)vMtrl.mData[0] * (float)dFactor
			, (float)vMtrl.mData[1] * (float)dFactor
			, (float)vMtrl.mData[2] * (float)dFactor
			, (float)dFactor);
		return vRetVal;

		return Vector4();
	}
	std::wstring FbxLoader::GetMtrlTextureName(fbxsdk::FbxSurfaceMaterial* _pSurface, const char* _pMtrlProperty)
	{
		std::string strName;

		fbxsdk::FbxProperty TextureProperty = _pSurface->FindProperty(_pMtrlProperty);
		if (TextureProperty.IsValid())
		{
			UINT iCnt = TextureProperty.GetSrcObjectCount();

			if (1 <= iCnt)
			{
				fbxsdk::FbxFileTexture* pFbxTex = TextureProperty.GetSrcObject<fbxsdk::FbxFileTexture>(0);
				if (NULL != pFbxTex)
					strName = pFbxTex->GetFileName();
			}
		}

		return std::wstring(strName.begin(), strName.end());
	}
	void FbxLoader::LoadTexture()
	{
		// 텍스처 로드
		std::filesystem::path parentPath = std::filesystem::current_path().parent_path();
		std::filesystem::path path_fbx_texture = parentPath.wstring() + L"\\Resources\\fbx\\Texture\\";

		//std::filesystem::path path_fbx_texture = path_content.wstring() + L"texture\\FBXTexture\\";
		//if (false == exists(path_fbx_texture))
		//{
		//	create_directory(path_fbx_texture);
		//}

		std::filesystem::path path_origin;
		std::filesystem::path path_filename; 
		std::filesystem::path path_dest;

		for (UINT i = 0; i < mContainers.size(); ++i)
		{
			for (UINT j = 0; j < mContainers[i].materials.size(); ++j)
			{
				std::vector<std::filesystem::path> vecPath;
				vecPath.push_back(mContainers[i].materials[j].diffuse.c_str());
				vecPath.push_back(mContainers[i].materials[j].normal.c_str());
				vecPath.push_back(mContainers[i].materials[j].specular.c_str());
				vecPath.push_back(mContainers[i].materials[j].emissive.c_str());

				for (size_t k = 0; k < vecPath.size(); ++k)
				{
					path_origin = vecPath[k];
					path_filename = vecPath[k].filename();
					if (path_filename == L"")
						continue;

					path_dest = path_fbx_texture.wstring() + path_filename.wstring();

					if (false == exists(path_dest))
					{
						copy(path_origin, path_dest);
					}

					//path_dest = GetRelativePath(CPathMgr::GetInst()->GetContentPath(), path_dest);
					//CResMgr::GetInst()->Load<CTexture>(path_dest, path_dest);
					std::wstring texPath = L"fbx\\Texture\\";
					texPath += path_filename;
					Resources::Load<Texture>(path_filename, texPath);



					switch (k)
					{
					case 0: mContainers[i].materials[j].diffuse = path_filename; break;
					case 1: mContainers[i].materials[j].normal = path_filename; break;
					case 2: mContainers[i].materials[j].specular = path_filename; break;
					case 3: mContainers[i].materials[j].emissive = path_filename; break;
					}
				}
			}
			path_origin = path_origin.parent_path();
			remove_all(path_origin);
		}

	}
	void FbxLoader::CreateMaterial()
	{
		// 메테리얼 로드
		std::wstring strKey;
		std::wstring strPath;

		for (UINT i = 0; i < mContainers.size(); ++i)
		{
			for (UINT j = 0; j < mContainers[i].materials.size(); ++j)
			{
				std::shared_ptr<Material> pMaterial = std::make_shared<Material>();

				// Material 이름짓기
				strKey = mContainers[i].materials[j].name;
				if (strKey.empty())
					strKey = std::filesystem::path(mContainers[i].materials[j].diffuse).stem();

				strPath = L"material\\";
				strPath += strKey;

				// 상대경로가 곧 이름(확장자명은 제외)
				mContainers[i].materials[j].name = strKey;
				pMaterial->SetKey(strKey);
				pMaterial->SetPath(strPath + L".mtrl");

				std::shared_ptr<Shader> defferdShader = Resources::Find<Shader>(L"DefferdShader");
				//std::shared_ptr<Shader> defferdShader = Resources::Find<Shader>(L"BasicShader");
				pMaterial->SetShader(defferdShader);

				std::wstring strTexKey = mContainers[i].materials[j].diffuse;
				std::shared_ptr<Texture> pTex = Resources::Find<Texture>(strTexKey);
				if (NULL != pTex)
					pMaterial->SetTexture(eTextureSlot::Albedo , pTex);

				strTexKey = mContainers[i].materials[j].normal;
				pTex = Resources::Find<Texture>(strTexKey);
				if (NULL != pTex)
					pMaterial->SetTexture(eTextureSlot::Normal, pTex);

				strTexKey = mContainers[i].materials[j].specular;
				pTex = Resources::Find<Texture>(strTexKey);
				if (NULL != pTex)
					pMaterial->SetTexture(eTextureSlot::Specular, pTex);

				pMaterial->SetMaterialCoefficient(
					mContainers[i].materials[j].color.diffuseColor
					, mContainers[i].materials[j].color.specularColor
					, mContainers[i].materials[j].color.AmbientColor
					, mContainers[i].materials[j].color.EmessiveColor);

				pMaterial->SetRenderingMode(eRenderingMode::DefferdOpaque);
				//pMaterial->SetRenderingMode(eRenderingMode::Opaque);
				Resources::Insert<Material>(pMaterial->GetKey(), pMaterial);
			}
		}

	}
	void FbxLoader::LoadSkeleton(fbxsdk::FbxNode* _pNode)
	{
		int iChildCount = _pNode->GetChildCount();
		LoadSkeleton_Re(_pNode, 0, 0, -1);
	}
	void FbxLoader::LoadSkeleton_Re(fbxsdk::FbxNode* _pNode, int _iDepth, int _iIdx, int _iParentIdx)
	{
		fbxsdk::FbxNodeAttribute* pAttr = _pNode->GetNodeAttribute();

		if (pAttr && pAttr->GetAttributeType() == fbxsdk::FbxNodeAttribute::eSkeleton)
		{
			Bone* pBone = new Bone;

			std::string strBoneName = _pNode->GetName();

			pBone->name = std::wstring(strBoneName.begin(), strBoneName.end());
			pBone->depth = _iDepth++;
			pBone->parentIdx = _iParentIdx;

			mBones.push_back(pBone);
		}

		int iChildCount = _pNode->GetChildCount();
		for (int i = 0; i < iChildCount; ++i)
		{
			LoadSkeleton_Re(_pNode->GetChild(i), _iDepth, (int)mBones.size(), _iIdx);
		}
	}
	void FbxLoader::LoadAnimationClip()
	{
		int iAnimCount = mAnimationNames.GetCount();

		for (int i = 0; i < iAnimCount; ++i)
		{
			fbxsdk::FbxAnimStack* pAnimStack 
				= mScene->FindMember<fbxsdk::FbxAnimStack>(mAnimationNames[i]->Buffer());


			//FbxAnimEvaluator* pevaluator = m_pScene->GetAnimationEvaluator();
			//m_pScene->SetCurrentAnimationStack();


			if (!pAnimStack)
				continue;

			AnimationClip* pAnimClip = new AnimationClip();

			std::string strClipName = pAnimStack->GetName();
			pAnimClip->name = std::wstring(strClipName.begin(), strClipName.end());

			fbxsdk::FbxTakeInfo* pTakeInfo = mScene->GetTakeInfo(pAnimStack->GetName());
			pAnimClip->startTime = pTakeInfo->mLocalTimeSpan.GetStart();
			pAnimClip->endTime = pTakeInfo->mLocalTimeSpan.GetStop();

			pAnimClip->mode = mScene->GetGlobalSettings().GetTimeMode();
			pAnimClip->timeLength = pAnimClip->endTime.GetFrameCount(pAnimClip->mode) 
				- pAnimClip->startTime.GetFrameCount(pAnimClip->mode);

			mAnimationClips.push_back(pAnimClip);
		}
	}
	void FbxLoader::Triangulate(fbxsdk::FbxNode* _pNode)
	{
		fbxsdk::FbxNodeAttribute* pAttr = _pNode->GetNodeAttribute();

		if (pAttr &&
			(pAttr->GetAttributeType() == fbxsdk::FbxNodeAttribute::eMesh
				|| pAttr->GetAttributeType() == fbxsdk::FbxNodeAttribute::eNurbs
				|| pAttr->GetAttributeType() == fbxsdk::FbxNodeAttribute::eNurbsSurface))
		{
			fbxsdk::FbxGeometryConverter converter(mManager);
			converter.Triangulate(pAttr, true);
		}

		int iChildCount = _pNode->GetChildCount();

		for (int i = 0; i < iChildCount; ++i)
		{
			Triangulate(_pNode->GetChild(i));
		}
	}
	void FbxLoader::LoadAnimationData(fbxsdk::FbxMesh* _pMesh, Container* _pContainer)
	{
		// Animation Data 로드할 필요가 없음
		int iSkinCount = _pMesh->GetDeformerCount(fbxsdk::FbxDeformer::eSkin);
		if (iSkinCount <= 0 || mAnimationClips.empty())
			return;

		_pContainer->bAnimation = true;

		// Skin 개수만큼 반복을하며 읽는다.	
		for (int i = 0; i < iSkinCount; ++i)
		{
			fbxsdk::FbxSkin* pSkin = (fbxsdk::FbxSkin*)_pMesh->GetDeformer(i, fbxsdk::FbxDeformer::eSkin);

			if (pSkin)
			{
				fbxsdk::FbxSkin::EType eType = pSkin->GetSkinningType();
				if (fbxsdk::FbxSkin::eRigid == eType || fbxsdk::FbxSkin::eLinear)
				{
					// Cluster 를 얻어온다
					// Cluster == Joint == 관절
					int iClusterCount = pSkin->GetClusterCount();

					for (int j = 0; j < iClusterCount; ++j)
					{
						fbxsdk::FbxCluster* pCluster = pSkin->GetCluster(j);

						if (!pCluster->GetLink())
							continue;

						// 현재 본 인덱스를 얻어온다.
						int iBoneIdx = FindBoneIndex(pCluster->GetLink()->GetName());
						if (-1 == iBoneIdx)
							assert(NULL);

						fbxsdk::FbxAMatrix matNodeTransform = GetTransform(_pMesh->GetNode());

						// Weights And Indices 정보를 읽는다.
						LoadWeightsAndIndices(pCluster, iBoneIdx, _pContainer);

						// Bone 의 OffSet 행렬 구한다.
						LoadOffsetMatrix(pCluster, matNodeTransform, iBoneIdx, _pContainer);

						// Bone KeyFrame 별 행렬을 구한다.
						LoadKeyframeTransform(_pMesh->GetNode(), pCluster, matNodeTransform, iBoneIdx, _pContainer);
					}
				}
			}
		}

		CheckWeightAndIndices(_pMesh, _pContainer);
	}

	void FbxLoader::CheckWeightAndIndices(fbxsdk::FbxMesh* _pMesh, Container* _pContainer)
	{
		std::vector<std::vector<WeightAndIndices>>::iterator iter = _pContainer->weightAndIndices.begin();

		int iVtxIdx = 0;
		for (iter; iter != _pContainer->weightAndIndices.end(); ++iter, ++iVtxIdx)
		{
			if ((*iter).size() > 1)
			{
				// 가중치 값 순으로 내림차순 정렬
				sort((*iter).begin(), (*iter).end()
					, [](const WeightAndIndices& left, const WeightAndIndices& right)
					{
						return left.weight > right.weight;
					}
				);

				double dWeight = 0.f;
				for (UINT i = 0; i < (*iter).size(); ++i)
				{
					dWeight += (*iter)[i].weight;
				}

				// 가중치의 합이 1이 넘어가면 처음부분에 더해준다.
				double revision = 0.f;
				if (dWeight > 1.0)
				{
					revision = 1.0 - dWeight;
					(*iter)[0].weight += revision;
				}

				if ((*iter).size() >= 4)
				{
					(*iter).erase((*iter).begin() + 4, (*iter).end());
				}
			}

			// 정점 정보로 변환, 
			float fWeights[4] = {};
			float fIndices[4] = {};

			for (UINT i = 0; i < (*iter).size(); ++i)
			{
				fWeights[i] = (float)(*iter)[i].weight;
				fIndices[i] = (float)(*iter)[i].boneIdx;
			}

			memcpy(&_pContainer->skiningWeights[iVtxIdx], fWeights, sizeof(Vector4));
			memcpy(&_pContainer->skiningIndices[iVtxIdx], fIndices, sizeof(Vector4));
		}
	}

	void FbxLoader::LoadWeightsAndIndices(fbxsdk::FbxCluster* _pCluster, int _iBoneIdx, Container* _pContainer)
	{
		int iIndicesCount = _pCluster->GetControlPointIndicesCount();

		for (int i = 0; i < iIndicesCount; ++i)
		{
			WeightAndIndices tWI = {};

			// 각 정점에게 본 인덱스 정보와, 가중치 값을 알린다.
			tWI.boneIdx = _iBoneIdx;
			tWI.weight = _pCluster->GetControlPointWeights()[i];

			int iVtxIdx = _pCluster->GetControlPointIndices()[i];

			_pContainer->weightAndIndices[iVtxIdx].push_back(tWI);
		}
	}
	void FbxLoader::LoadOffsetMatrix(fbxsdk::FbxCluster* _pCluster, const fbxsdk::FbxAMatrix& _matNodeTransform, int _iBoneIdx, Container* _pContainer)
	{
		fbxsdk::FbxAMatrix matClusterTrans;
		fbxsdk::FbxAMatrix matClusterLinkTrans;

		_pCluster->GetTransformMatrix(matClusterTrans);
		_pCluster->GetTransformLinkMatrix(matClusterLinkTrans);

		// Reflect Matrix
		fbxsdk::FbxVector4 V0 = { 1, 0, 0, 0 };
		fbxsdk::FbxVector4 V1 = { 0, 0, 1, 0 };
		fbxsdk::FbxVector4 V2 = { 0, 1, 0, 0 };
		fbxsdk::FbxVector4 V3 = { 0, 0, 0, 1 };

		fbxsdk::FbxAMatrix matReflect;
		matReflect[0] = V0;
		matReflect[1] = V1;
		matReflect[2] = V2;
		matReflect[3] = V3;

		fbxsdk::FbxAMatrix matOffset;
		matOffset = matClusterLinkTrans.Inverse() * matClusterTrans * _matNodeTransform;
		matOffset = matReflect * matOffset * matReflect;

		mBones[_iBoneIdx]->offsetMatrix = matOffset;
	}
	void FbxLoader::LoadKeyframeTransform(fbxsdk::FbxNode* _pNode, fbxsdk::FbxCluster* _pCluster, const fbxsdk::FbxAMatrix& _matNodeTransform, int _iBoneIdx, Container* _pContainer)
	{
		if (mAnimationClips.empty())
			return;

		fbxsdk::FbxVector4	v1 = { 1, 0, 0, 0 };
		fbxsdk::FbxVector4	v2 = { 0, 0, 1, 0 };
		fbxsdk::FbxVector4	v3 = { 0, 1, 0, 0 };
		fbxsdk::FbxVector4	v4 = { 0, 0, 0, 1 };
		fbxsdk::FbxAMatrix	matReflect;
		matReflect.mData[0] = v1;
		matReflect.mData[1] = v2;
		matReflect.mData[2] = v3;
		matReflect.mData[3] = v4;

		mBones[_iBoneIdx]->boneMatrix = _matNodeTransform;

		fbxsdk::FbxTime::EMode eTimeMode = mScene->GetGlobalSettings().GetTimeMode();

		fbxsdk::FbxLongLong llStartFrame = mAnimationClips[0]->startTime.GetFrameCount(eTimeMode);
		fbxsdk::FbxLongLong llEndFrame = mAnimationClips[0]->endTime.GetFrameCount(eTimeMode);

		for (fbxsdk::FbxLongLong i = llStartFrame; i < llEndFrame; ++i)
		{
			KeyFrame tFrame = {};
			fbxsdk::FbxTime   tTime = 0;

			tTime.SetFrame(i, eTimeMode);

			fbxsdk::FbxAMatrix matFromNode = _pNode->EvaluateGlobalTransform(tTime) * _matNodeTransform;
			fbxsdk::FbxAMatrix matCurTrans = matFromNode.Inverse() * _pCluster->GetLink()->EvaluateGlobalTransform(tTime);
			matCurTrans = matReflect * matCurTrans * matReflect;

			tFrame.time = tTime.GetSecondDouble();
			tFrame.transform = matCurTrans;

			mBones[_iBoneIdx]->keyFrames.push_back(tFrame);
		}
	}
	int FbxLoader::FindBoneIndex(std::string _strBoneName)
	{
		std::wstring strBoneName = std::wstring(_strBoneName.begin(), _strBoneName.end());

		for (UINT i = 0; i < mBones.size(); ++i)
		{
			if (mBones[i]->name == strBoneName)
				return i;
		}

		return -1;
	}
	fbxsdk::FbxAMatrix FbxLoader::GetTransform(fbxsdk::FbxNode* _pNode)
	{
		const fbxsdk::FbxVector4 vT = _pNode->GetGeometricTranslation(fbxsdk::FbxNode::eSourcePivot);
		const fbxsdk::FbxVector4 vR = _pNode->GetGeometricRotation(fbxsdk::FbxNode::eSourcePivot);
		const fbxsdk::FbxVector4 vS = _pNode->GetGeometricScaling(fbxsdk::FbxNode::eSourcePivot);

		return fbxsdk::FbxAMatrix(vT, vR, vS);
	}
	void FbxLoader::Release()
	{
		//if (mScene)
		//{
		//	mScene->Destroy();
		//}
		//
		//// 매니저 안에서 모든 메모리를 해제 해준다.
		//if (mManager)
		//{
		//	mManager->Destroy();
		//}
	}
}
