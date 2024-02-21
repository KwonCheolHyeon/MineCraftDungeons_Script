#include "yaMesh.h"
#include "yaRenderer.h"
#include "yaGraphicDevice_DX11.h"
#include "yaFbxLoader.h"
#include "yaMesh.h"

namespace md
{


	Mesh::Mesh()
		: Resource(eResourceType::Mesh)
		, mVBDesc{}
		, mIndexInfos{}
	{

	}

	Mesh::~Mesh()
	{
		if (pVtxSysMem)
		{
			delete[] pVtxSysMem;
			pVtxSysMem = nullptr;
		}

		if (mIndexInfos.size() > 0)
		{
			for (auto& info : mIndexInfos)
			{
				delete info.pIdxSysMem;
				info.pIdxSysMem = nullptr;
			}
		}
		
		if (IsAnimMesh())
		{
			if (mBoneFrameData)
			{
				mBoneFrameData->Clear();
				SAFE_DELETE(mBoneFrameData);
			}
	
			if (mBoneOffset)
			{
				mBoneOffset->Clear();
				SAFE_DELETE(mBoneOffset);
			}

			auto iter = mEvents.begin();
			auto iterEnd = mEvents.end();
			for (; iter != iterEnd; ++iter)
			{
				SAFE_DELETE(iter->second);
			}
			mEvents.clear();
		}
	}

	std::shared_ptr<Mesh> Mesh::CreateFromContainer(FbxLoader* loader)
	{
		const Container& container = loader->GetContainer(0);

		UINT iVtxCount = (UINT)container.positions.size();
		D3D11_BUFFER_DESC tVtxDesc = {};

		tVtxDesc.ByteWidth = sizeof(graphics::Vertex) * iVtxCount;
		tVtxDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		tVtxDesc.Usage = D3D11_USAGE_DEFAULT;
		if (D3D11_USAGE_DYNAMIC == tVtxDesc.Usage)
			tVtxDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		D3D11_SUBRESOURCE_DATA tSub = {};
		tSub.pSysMem = malloc(tVtxDesc.ByteWidth);

		graphics::Vertex* pSys = (graphics::Vertex*)tSub.pSysMem;
		for (UINT i = 0; i < iVtxCount; ++i)
		{
			pSys[i].pos = Vector4(container.positions[i].x
				, container.positions[i].y
				, container.positions[i].z, 1.0f);
			pSys[i].uv = container.uv[i];
			pSys[i].color = Vector4(1.f, 0.f, 1.f, 1.f);
			pSys[i].normal = container.normals[i];
			pSys[i].tangent = container.tangents[i];
			pSys[i].biNormal = container.binormals[i];
			pSys[i].weight = container.skiningWeights[i];
			pSys[i].Indices = container.skiningIndices[i];
		}

		Microsoft::WRL::ComPtr<ID3D11Buffer> pVB = NULL;
		if (false == (GetDevice()->CreateBuffer(&tVtxDesc, &tSub, pVB.GetAddressOf())))
		{
			return NULL;
		}

		std::shared_ptr<Mesh> pMesh = std::make_shared<Mesh>();
		pMesh->mVertexBuffer = pVB;
		pMesh->mVtxCount = iVtxCount;
		pMesh->mVBDesc = tVtxDesc;
		pMesh->pVtxSysMem = pSys;

		// 인덱스 정보
		UINT iIdxBufferCount = (UINT)container.indices.size();
		D3D11_BUFFER_DESC tIdxDesc = {};

		for (UINT i = 0; i < iIdxBufferCount; ++i)
		{
			tIdxDesc.ByteWidth = (UINT)container.indices[i].size() * sizeof(UINT); // Index Format 이 R32_UINT 이기 때문
			tIdxDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			tIdxDesc.Usage = D3D11_USAGE_DEFAULT;
			if (D3D11_USAGE_DYNAMIC == tIdxDesc.Usage)
				tIdxDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

			void* pSysMem = malloc(tIdxDesc.ByteWidth);
			memcpy(pSysMem, container.indices[i].data(), tIdxDesc.ByteWidth);
			tSub.pSysMem = pSysMem;

			Microsoft::WRL::ComPtr<ID3D11Buffer> pIB = nullptr;
			if (false == (GetDevice()->CreateBuffer(&tIdxDesc, &tSub, pIB.GetAddressOf())))
			{
				return NULL;
			}

			IndexInfo info = {};
			info.desc = tIdxDesc;
			info.indexCount = (UINT)container.indices[i].size();
			info.pIdxSysMem = pSysMem;
			info.buffer = pIB;

			pMesh->mIndexInfos.push_back(info);
		}


		//3D Animaition
		if (!container.bAnimation)
			return pMesh;

		std::vector<Bone*>& vecBone = loader->GetBones();
		UINT iFrameCount = 0;
		for (UINT i = 0; i < vecBone.size(); ++i)
		{
			BoneMatrix bone = {};
			bone.depth = vecBone[i]->depth;
			bone.parentIdx = vecBone[i]->parentIdx;
			bone.bone = GetMatrixFromFbxMatrix(vecBone[i]->boneMatrix);
			bone.offset = GetMatrixFromFbxMatrix(vecBone[i]->offsetMatrix);
			bone.name = vecBone[i]->name;

			for (UINT j = 0; j < vecBone[i]->keyFrames.size(); ++j)
			{
				BoneKeyFrame tKeyframe = {};
				tKeyframe.time = vecBone[i]->keyFrames[j].time;
				tKeyframe.frame = j;
				tKeyframe.translate.x = (float)vecBone[i]->keyFrames[j].transform.GetT().mData[0];
				tKeyframe.translate.y = (float)vecBone[i]->keyFrames[j].transform.GetT().mData[1];
				tKeyframe.translate.z = (float)vecBone[i]->keyFrames[j].transform.GetT().mData[2];

				tKeyframe.scale.x = (float)vecBone[i]->keyFrames[j].transform.GetS().mData[0];
				tKeyframe.scale.y = (float)vecBone[i]->keyFrames[j].transform.GetS().mData[1];
				tKeyframe.scale.z = (float)vecBone[i]->keyFrames[j].transform.GetS().mData[2];

				tKeyframe.rotation.x = (float)vecBone[i]->keyFrames[j].transform.GetQ().mData[0];
				tKeyframe.rotation.y = (float)vecBone[i]->keyFrames[j].transform.GetQ().mData[1];
				tKeyframe.rotation.z = (float)vecBone[i]->keyFrames[j].transform.GetQ().mData[2];
				tKeyframe.rotation.w = (float)vecBone[i]->keyFrames[j].transform.GetQ().mData[3];

				bone.keyFrames.push_back(tKeyframe);
			}

			iFrameCount = max(iFrameCount, (UINT)bone.keyFrames.size());

			pMesh->mBones.push_back(bone);
		}

		std::vector<AnimationClip*>& vecAnimClip = loader->GetAnimClip();

		fbxsdk::FbxTime::EMode timeMode = fbxsdk::FbxTime::EMode::eFrames24;
		fbxsdk::FbxTime startTime = 0;
		fbxsdk::FbxTime endTime = 0;
		for (UINT i = 0; i < vecAnimClip.size(); ++i)
		{
			BoneAnimationClip tClip = {};

			tClip.name = vecAnimClip[i]->name;

			timeMode = vecAnimClip[i]->mode;
			startTime = vecAnimClip[i]->startTime;
			endTime = vecAnimClip[i]->endTime;

			if (startTime < 0)
			{
				startTime = 0;
			}

			tClip.mode = timeMode;

			tClip.originStartTime = startTime.GetSecondDouble();
			tClip.originEndTime = endTime.GetSecondDouble();
			tClip.originTimeLength = tClip.originEndTime - tClip.originStartTime;
			
			tClip.originStartFrame = (int)startTime.GetFrameCount(timeMode);
			tClip.originEndFrame = (int)endTime.GetFrameCount(timeMode);
			tClip.originFrameLength = tClip.originEndFrame - tClip.originStartFrame;

			pMesh->mAnimClip.push_back(tClip);
		}
		
		// Animation 이 있는 Mesh 경우 structuredbuffer 만들어두기
		if (pMesh->IsAnimMesh())
		{
			// BoneOffet 행렬
			std::vector<Matrix> vecOffset;
			std::vector<BoneFrameTransform> vecFrameTrans;
			vecFrameTrans.resize((UINT)pMesh->mBones.size() * iFrameCount);
		
			for (size_t i = 0; i < pMesh->mBones.size(); ++i)
			{
				vecOffset.push_back(pMesh->mBones[i].offset);
		
				for (size_t j = 0; j < pMesh->mBones[i].keyFrames.size(); ++j)
				{
					vecFrameTrans[(UINT)pMesh->mBones.size() * j + i]
						= BoneFrameTransform
					{
						Vector4(pMesh->mBones[i].keyFrames[j].translate.x
							, pMesh->mBones[i].keyFrames[j].translate.y
							, pMesh->mBones[i].keyFrames[j].translate.z, 0.f)
						, Vector4(pMesh->mBones[i].keyFrames[j].scale.x
							, pMesh->mBones[i].keyFrames[j].scale.y
							, pMesh->mBones[i].keyFrames[j].scale.z, 0.f)
						, pMesh->mBones[i].keyFrames[j].rotation
					};
				}
			}
		
			pMesh->mBoneOffset = new StructedBuffer();
			pMesh->mBoneOffset->Create(sizeof(Matrix), (UINT)vecOffset.size(), eSRVType::SRV, vecOffset.data(), false);
		
			pMesh->mBoneFrameData = new StructedBuffer();
			pMesh->mBoneFrameData->Create(sizeof(BoneFrameTransform), (UINT)vecOffset.size() * iFrameCount
				, eSRVType::SRV, vecFrameTrans.data(), false);
		}

		return pMesh;
	}

	HRESULT Mesh::Save(const std::wstring& name, FILE* file)
	{

		// 정점정보 저장
		int byteSize = mVBDesc.ByteWidth;
		fwrite(&byteSize, sizeof(int), 1, file);
		fwrite(pVtxSysMem, byteSize, 1, file);

		// 인덱스 정보 
		UINT materialCount = static_cast<UINT>(mIndexInfos.size());
		fwrite(&materialCount, sizeof(int), 1, file);

		//mIndexInfos
		UINT idxBufferSize = 0;
		for (size_t i = 0; i < materialCount; i++)
		{
			fwrite(&mIndexInfos[i], sizeof(IndexInfo), 1, file);
			fwrite(mIndexInfos[i].pIdxSysMem
				, mIndexInfos[i].indexCount * sizeof(UINT), 1, file);
		}

		// 애니메이션 클립에있는 정보
		UINT animationCount = static_cast<UINT>(mAnimClip.size());
		fwrite(&animationCount, sizeof(UINT), 1, file);
		if (mAnimClip.size() == 0)
			return S_OK;

		for (size_t i = 0; i < animationCount; i++)
		{
			UINT nameSize = static_cast<UINT>(mAnimClip[i].name.size());
			fwrite(&nameSize, sizeof(UINT), 1, file);
			fwrite(mAnimClip[i].name.c_str(), mAnimClip[i].name.size() * sizeof(wchar_t), 1, file);
			fwrite(&mAnimClip[i].originStartTime, sizeof(double), 1, file);
			fwrite(&mAnimClip[i].originEndTime, sizeof(double), 1, file);
			fwrite(&mAnimClip[i].originTimeLength, sizeof(double), 1, file);
			fwrite(&mAnimClip[i].mode, sizeof(int), 1, file);
			fwrite(&mAnimClip[i].updateTime, sizeof(float), 1, file);
			fwrite(&mAnimClip[i].originStartFrame, sizeof(int), 1, file);
			fwrite(&mAnimClip[i].originEndFrame, sizeof(int), 1, file);
			fwrite(&mAnimClip[i].originFrameLength, sizeof(int), 1, file);
		}

		// 본정보들 전부 저장
		UINT boneCount = static_cast<UINT>(mBones.size());
		fwrite(&boneCount, sizeof(UINT), 1, file);
		for (size_t i = 0; i < boneCount; i++)
		{
			UINT nameSize = static_cast<UINT>(mBones[i].name.size());
			fwrite(&nameSize, sizeof(UINT), 1, file);
			fwrite(mBones[i].name.c_str(), mBones[i].name.size() * sizeof(wchar_t), 1, file);

			fwrite(&mBones[i].depth, sizeof(int), 1, file);
			fwrite(&mBones[i].parentIdx, sizeof(int), 1, file);
			fwrite(&mBones[i].bone, sizeof(Matrix), 1, file);
			fwrite(&mBones[i].offset, sizeof(Matrix), 1, file);

			int frameCount = static_cast<int>(mBones[i].keyFrames.size());
			fwrite(&frameCount, sizeof(int), 1, file);

			for (size_t j = 0; j < frameCount; j++)
			{
				fwrite(&mBones[i].keyFrames[j], sizeof(BoneKeyFrame), 1, file);
			}
		}

		return S_OK;
	}

	HRESULT Mesh::Load(const std::wstring& name, FILE* file)
	{
		std::filesystem::path parentPath = std::filesystem::current_path().parent_path();
		std::wstring fullPath = parentPath.wstring() + L"\\Resources\\Mesh\\" + name;

		// 정점정보 저장
		int byteSize = 0;
		fread(&byteSize, sizeof(int), 1, file);
		pVtxSysMem = (Vertex*)malloc(byteSize);
		fread(pVtxSysMem, 1, byteSize, file);

		D3D11_BUFFER_DESC desc = {};
		desc.ByteWidth = byteSize;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.Usage = D3D11_USAGE_DEFAULT;

		D3D11_SUBRESOURCE_DATA data = {};
		data.pSysMem = pVtxSysMem;

		if (false == (GetDevice()->CreateBuffer(&desc, &data, mVertexBuffer.GetAddressOf())))
			return S_FALSE;


		// 인덱스 정보 
		UINT materialCount = 0;
		fread(&materialCount, sizeof(int), 1, file);

		for (size_t i = 0; i < materialCount; i++)
		{
			IndexInfo info = {};
			fread(&info, sizeof(IndexInfo), 1, file);

			UINT byteWidth = info.indexCount * sizeof(UINT);
			void* pSysMem = malloc(byteWidth);
			info.pIdxSysMem = pSysMem;
			fread(info.pIdxSysMem, info.indexCount * sizeof(UINT), 1, file);

			D3D11_SUBRESOURCE_DATA idxData = {};
			idxData.pSysMem = info.pIdxSysMem;

			if (false == (GetDevice()->CreateBuffer(&info.desc, &idxData, info.buffer.GetAddressOf())))
				return S_FALSE;

			mIndexInfos.push_back(info);
		}


		// 애니메이션 클립에있는 정보
		UINT animationCount = 0;
		fread(&animationCount, sizeof(UINT), 1, file);
		if (animationCount <= 0)
			return S_OK;

		
		//mAnimClip.resize(animationCount);

		for (size_t i = 0; i < animationCount; i++)
		{
			BoneAnimationClip clip = {};

			UINT nameSize = 0;
			fread(&nameSize, sizeof(UINT), 1, file);
			wchar_t buffer[256] = L"";
			fread(buffer, sizeof(wchar_t), nameSize, file);
			clip.name = buffer;

			fread(&clip.originStartTime, sizeof(double), 1, file);
			fread(&clip.originEndTime, sizeof(double), 1, file);
			fread(&clip.originTimeLength, sizeof(double), 1, file);
			fread(&clip.mode, sizeof(int), 1, file);
			fread(&clip.updateTime, sizeof(float), 1, file);
			fread(&clip.originStartFrame, sizeof(int), 1, file);
			fread(&clip.originEndFrame, sizeof(int), 1, file);
			fread(&clip.originFrameLength, sizeof(int), 1, file);

			mAnimClip.push_back(clip);
		}

		// 본정보들 전부 저장
		UINT boneCount = 0;
		fread(&boneCount, sizeof(UINT), 1, file);
		mBones.resize(boneCount);
		int frameCount = 0;
		for (size_t i = 0; i < boneCount; i++)
		{
			UINT nameSize = 0;
			fread(&nameSize, sizeof(UINT), 1, file);
			wchar_t buffer[256] = L"";
			fread(buffer, sizeof(wchar_t), nameSize, file);
			mBones[i].name = buffer;

			fread(&mBones[i].depth, sizeof(int), 1, file);
			fread(&mBones[i].parentIdx, sizeof(int), 1, file);
			fread(&mBones[i].bone, sizeof(Matrix), 1, file);
			fread(&mBones[i].offset, sizeof(Matrix), 1, file);

			int curFrameCount = 0;
			fread(&curFrameCount, sizeof(int), 1, file);
			mBones[i].keyFrames.resize(curFrameCount);
			frameCount = max(frameCount, curFrameCount);

			for (size_t j = 0; j < curFrameCount; j++)
			{
				fread(&mBones[i].keyFrames[j], sizeof(BoneKeyFrame), 1, file);
			}
		}



		if (mAnimClip.size() > 0)
		{
			// BoneOffet 행렬
			std::vector<Matrix> vecOffset;
			std::vector<BoneFrameTransform> vecFrameTrans;
			vecFrameTrans.resize((UINT)mBones.size() * frameCount);

			for (size_t i = 0; i < mBones.size(); ++i)
			{
				vecOffset.push_back(mBones[i].offset);

				for (size_t j = 0; j < mBones[i].keyFrames.size(); ++j)
				{
					vecFrameTrans[(UINT)mBones.size() * j + i]
						= BoneFrameTransform
					{
						Vector4(mBones[i].keyFrames[j].translate.x
							, mBones[i].keyFrames[j].translate.y
							, mBones[i].keyFrames[j].translate.z, 0.f)
						, Vector4(mBones[i].keyFrames[j].scale.x
							, mBones[i].keyFrames[j].scale.y
							, mBones[i].keyFrames[j].scale.z, 0.f)
						, mBones[i].keyFrames[j].rotation
					};
				}
			}

			mBoneOffset = new StructedBuffer();
			mBoneOffset->Create(sizeof(Matrix), (UINT)vecOffset.size(), eSRVType::SRV, vecOffset.data(), false);

			mBoneFrameData = new StructedBuffer();
			mBoneFrameData->Create(sizeof(BoneFrameTransform), (UINT)vecOffset.size() * frameCount
				, eSRVType::SRV, vecFrameTrans.data(), false);
		}

		
		return S_OK;
	}

	bool Mesh::CreateVertexBuffer(void* data, UINT count)
	{
		// 버텍스 버퍼
		mVBDesc.ByteWidth = sizeof(graphics::Vertex) * count;
		mVBDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
		mVBDesc.CPUAccessFlags = 0;
		mVBDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;

		mVBDesc.MiscFlags = 0;
		mVBDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA subData = {};
		subData.pSysMem = data;

		if (!GetDevice()->CreateBuffer(&mVBDesc, &subData, mVertexBuffer.GetAddressOf()))
			return false;

		pVtxSysMem = new graphics::Vertex[count];
		memcpy(pVtxSysMem, data, sizeof(graphics::Vertex) * count);

		return true;
	}

	bool Mesh::CreateIndexBuffer(void* data, UINT indexBufferCount)
	{
		IndexInfo indexInfo = {};
		indexInfo.indexCount = indexBufferCount;
		indexInfo.desc.ByteWidth = sizeof(UINT) * indexBufferCount;


		indexInfo.desc.CPUAccessFlags = 0;
		indexInfo.desc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
		indexInfo.desc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER;
		indexInfo.desc.MiscFlags = 0;
		indexInfo.desc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA subData = {};
		subData.pSysMem = data;

		if (!GetDevice()->CreateBuffer(&indexInfo.desc, &subData, indexInfo.buffer.GetAddressOf()))
			return false;

		indexInfo.pIdxSysMem = new UINT[indexInfo.indexCount];
		memcpy(indexInfo.pIdxSysMem, data, sizeof(UINT) * indexInfo.indexCount);

		mIndexInfos.push_back(indexInfo);
		return true;
	}

	void Mesh::BindBuffer(UINT subSet)
	{
		// Input Assembeler 단계에 버텍스버퍼 정보 지정
		UINT stride = sizeof(graphics::Vertex);
		UINT offset = 0;

		GetDevice()->BindVertexBuffer(0, 1, mVertexBuffer.GetAddressOf(), &stride, &offset);
		GetDevice()->BindIndexBuffer(mIndexInfos[subSet].buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	}

	void Mesh::Render(UINT subSet)
	{
		GetDevice()->DrawIndexed(mIndexInfos[subSet].indexCount, 0, 0);
	}

	void Mesh::RenderInstanced(UINT count, UINT subSet)
	{
		GetDevice()->DrawIndexedInstanced(mIndexInfos[subSet].indexCount, count, 0, 0, 0);
	}

	bool Mesh::FindAnimClip(const std::wstring& _wstrName)
	{
		size_t Size = mAnimClip.size();
		bool findResult = false;
		for (size_t iter = 0; iter < Size; ++iter)
		{
			if (mAnimClip[iter].name == _wstrName)
			{
				findResult = true;
			}
		}

		return findResult;
	}

	bool Mesh::AddAnimationClip(const std::wstring& _wstrName, int _startFrame, int _endFrame, fbxsdk::FbxTime::EMode _frameMode)
	{		
		bool findResult = FindAnimClip(_wstrName);

		if (findResult)
		{
			AssertEx(false, L"Mesh::AddAnimationClip - 이미 존재하는 애니메이션 클립을 추가하려고 했습니다.");
			return false;
		}

		BoneAnimationClip newClip = {};
		newClip.name = _wstrName;
		newClip.originStartFrame = _startFrame;
		newClip.originEndFrame = _endFrame;
		newClip.originFrameLength = _endFrame - _startFrame;

		double frameModeTime = 0.0;
		switch (_frameMode)
		{
		case fbxsdk::FbxTime::EMode::eFrames60:
			frameModeTime = 60.0;
			break;
		case fbxsdk::FbxTime::EMode::eFrames30:
			frameModeTime = 30.0;
			break;
		case fbxsdk::FbxTime::EMode::eFrames24:
		default:
			frameModeTime = 24.0;
			break;
		}
		newClip.originStartTime = _startFrame / frameModeTime;
		newClip.originEndTime = _endFrame / frameModeTime;
		newClip.originTimeLength = newClip.originEndTime - newClip.originStartTime;
		newClip.mode = static_cast<UINT>(_frameMode);

		mAnimClip.push_back(newClip);

		Events* events = new Events();
		events->mEvents.resize(newClip.originFrameLength);
		mEvents.insert(std::make_pair(_wstrName, events));

		return true;
	}

}
