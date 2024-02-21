#include "yaBoneAnimator.h"
#include "yaGameObject.h"
#include "yaMeshRenderer.h"
#include "yaStructedBuffer.h"
#include "yaResources.h"
#include "yaTime.h"
#include "yaFbxLoader.h"
#include "yaBoneShader.h"
#include <cmath> 

namespace md
{
	BoneAnimator::BoneAnimator()
		: Component(eComponentType::BoneAnimator)
		, mBones{}
		, mAnimationClips{}
		, mAnimationUpdateTime{0.f,}
		, mAnimaitonBoneMatrix{}
		, mFrameCount(24)
		, mCurrentTime(0.0f)
		, mCurrentClip(0)
		, mFrameIdx(-1)
		, mNextFrameIdx(-1)
		, mRatio(0.0f)
		, mBoneMatrixBuffer(nullptr)
		, mbFinalMatrixUpdate(false)
		, mBoneSocketBuffer(nullptr)
		, mbUseSocket(false)
		, mEvents{}
	{
		mBoneMatrixBuffer = new graphics::StructedBuffer();
		mBoneSocketBuffer = new graphics::StructedBuffer();
	}

	BoneAnimator::~BoneAnimator()
	{
		delete mBoneMatrixBuffer;
		mBoneMatrixBuffer = nullptr;

		//delete mBoneSocketBuffer;
		//mBoneSocketBuffer = nullptr;

		mBones = nullptr;
		mAnimationClips = nullptr;

		{
			mAnimaitonBoneMatrix.clear();
			mAnimationUpdateTime.clear();
			//mSocketMatrix.clear();
		}

		auto iter = mEvents.begin();
		auto iterEnd = mEvents.end();
		for (; iter != iterEnd; ++iter)
		{
			SAFE_DELETE(iter->second);
		}
		mEvents.clear();
	}

	void BoneAnimator::Initalize()
	{
	}

	void BoneAnimator::Update()
	{
	}

	void BoneAnimator::FixedUpdate()
	{
		mCurrentTime = 0.0f;

		if (mFrameIdx >= mAnimationClips->at(mCurrentClip).originEndFrame || mAnimationUpdateTime[mCurrentClip] >= mAnimationClips->at(mCurrentClip).originTimeLength)
		{
			if (mAnimationClips->at(mCurrentClip).isLoop)
			{
				mAnimationUpdateTime[mCurrentClip] = 0.0;
				mCurrentTime = mAnimationClips->at(mCurrentClip).originStartTime;
				mNextFrameIdx = mFrameIdx + 1;

				// 현재 프레임 인덱스 구하기
				double dFrameIdx = mCurrentTime * (double)mFrameCount;
				mFrameIdx = (int)(dFrameIdx);

				// 프레임간의 시간에 따른 비율을 구해준다.
				mRatio = (float)(dFrameIdx - (double)mFrameIdx);

			}
			else
			{
				if ((false == mEvents.empty()))
				{
					std::function<void()>* completeEvent = GetCompleteEventPtr(mAnimationClips->at(mCurrentClip).name);
					if (completeEvent)
					{
						static_cast<void>(completeEvent->target<void()>());

						Events* events = FindEvents(mAnimationClips->at(mCurrentClip).name);
						if (events)
						{
							events->mCompleteEvent();
							events->ResetExecute();
						}
					}
				}
			}
		}
		else
		{
			mAnimationUpdateTime[mCurrentClip] += Time::DeltaTime();			
			mCurrentTime = mAnimationClips->at(mCurrentClip).originStartTime + mAnimationUpdateTime[mCurrentClip];

			if ((false == mEvents.empty()))
			{
				UINT originStartFrame = mAnimationClips->at(mCurrentClip).originStartFrame;
				UINT targetFrame = 0;

				if (mFrameIdx < 0)
				{
					targetFrame = 0;
				}
				else
				{
					targetFrame = mFrameIdx - originStartFrame;
				}

				Events* events = FindEvents(mAnimationClips->at(mCurrentClip).name);
				if (events)
				{
					if (events->mEvents[targetFrame].mEvent)
						events->mEvents[targetFrame]();
				}
			}
			mNextFrameIdx = mFrameIdx + 1;
			
			// 현재 프레임 인덱스 구하기
			double dFrameIdx = (mCurrentTime) * (double)mFrameCount;
			mFrameIdx = (int)(dFrameIdx);

			// 프레임간의 시간에 따른 비율을 구해준다.
			mRatio = (float)(dFrameIdx - (double)mFrameIdx);
		}

		// 컴퓨트 쉐이더 연산여부
		mbFinalMatrixUpdate = false;
	}

	void BoneAnimator::Binds()
	{
		if (!mbFinalMatrixUpdate)
		{
			//Compute shader
			std::shared_ptr<BoneShader> boneShader 
				= Resources::Find<BoneShader>(L"BoneComputeShader");

			// Bone Data
			GameObject* gameObj = GetOwner();
			MeshRenderer* mr = gameObj->GetComponent<MeshRenderer>();
			std::shared_ptr<Mesh> mesh = mr->GetMesh();
			CheckBone(mesh);

			boneShader->SetFrameDataBuffer(mesh->GetBoneFrameDataBuffer());
			boneShader->SetOffsetMatBuffer(mesh->GetBoneOffsetBuffer());
			boneShader->SetOutputBuffer(mBoneMatrixBuffer);

			UINT iBoneCount = (UINT)mBones->size();
			boneShader->SetBoneCount(iBoneCount);
			boneShader->SetFrameIndex(mFrameIdx);
			boneShader->SetNextFrameIdx(mNextFrameIdx);
			boneShader->SetFrameRatio(mRatio);
			
			// 업데이트 쉐이더 실행
			boneShader->OnExcute();

			mbFinalMatrixUpdate = true;
		}

		mBoneMatrixBuffer->BindSRV(eShaderStage::VS, 30);

		if (mbUseSocket)
		{
			mBoneSocketBuffer->GetData(mSocketMatrix.data(), mSocketMatrix.size() * sizeof(math::Matrix));
			UpdateSocketInfo();
		}
	}

	void BoneAnimator::Render()
	{
	}

	void BoneAnimator::CheckBone(std::shared_ptr<Mesh> mesh)
	{
		//UINT boneCount = mesh->get 메시가 본 정보를 다 들고있게 해두자
		UINT boneCount = mesh->GetBoneCount();
		if (mBoneMatrixBuffer->GetStride() != boneCount)
		{
			//mBoneMatrixBuffer->Create(sizeof(Matrix), boneCount, eSRVType::UAV, nullptr, false);
			mBoneMatrixBuffer->Create(sizeof(Matrix), boneCount, eSRVType::UAV, nullptr, true);
			ResizeSocketMatrix(boneCount);
		}
	}

	UINT BoneAnimator::FindBoneIndex(const std::wstring& _boneName)
	{
		UINT boneIndex = UINT_MAX;

		size_t Size = mBones->size();
		for (size_t index = 0; index < Size; index++)
		{
			if (_boneName._Equal((*mBones)[index].name))
			{
				boneIndex = index;
			}
		}

		return boneIndex;
	}

	void BoneAnimator::SetAnimaitionClip(std::vector<BoneAnimationClip>* clips)
	{
		mAnimationClips = clips;
		mAnimationUpdateTime.resize(mAnimationClips->size());
		static float fTime = 0.f;
		fTime += 1.f;
		mAnimationUpdateTime[0] = fTime;
	}

	void BoneAnimator::ClearData()
	{
		mBoneMatrixBuffer->Clear();
		GameObject* gameObj = GetOwner();
		MeshRenderer* mr = gameObj->GetComponent<MeshRenderer>();
		UINT iMtrlCount = mr->GetMaterialCount();
		std::shared_ptr<Material> pMtrl = nullptr;
		for (UINT i = 0; i < iMtrlCount; ++i)
		{
			pMtrl = mr->GetMaterial(i);
			if (nullptr == pMtrl)
				continue;
			pMtrl->SetAnimation(false); // Animation Mesh 알리기
			pMtrl->SetBoneCount(0);
		}
	}
	void BoneAnimator::ResizeSocketMatrix(UINT _boneCount)
	{
		mBoneSocketBuffer->Create(sizeof(Matrix), _boneCount, eSRVType::UAV, nullptr, true);
		mSocketMatrix.clear();
		mSocketMatrix.shrink_to_fit();
		mSocketMatrix.resize(_boneCount);
	}

	md::tBoneSocket* const BoneAnimator::GetSocket(const std::wstring& _boneName)
	{
		tBoneSocket* const findResult = FindSocket(_boneName);

		return findResult;
	}

	md::tBoneSocket* BoneAnimator::FindSocket(const std::wstring& _boneName)
	{
		tBoneSocket* result = nullptr;

		// Bone Index 먼저 찾기
		UINT boneIndex = FindBoneIndex(_boneName);
		if (UINT_MAX == boneIndex)
		{
			return result;
		}

		// Bone Index와 일치하는 BoneSocket이 존재하는지 찾기
		result = FindSocket(boneIndex);
		if (nullptr == result)
		{
			result = CreateSocket(boneIndex);
		}

		return result;
	}

	md::tBoneSocket* BoneAnimator::FindSocket(UINT _boneIndex)
	{
		tBoneSocket* result = nullptr;

		size_t Size = mSockets.size();
		for (size_t index = 0; index < Size; ++index)
		{
			if (mSockets[index].mBoneIndex == _boneIndex)
			{
				result = &mSockets[index];
			}
		}

		return result;
	}

	md::tBoneSocket* BoneAnimator::CreateSocket(UINT _boneIndex)
	{
		tBoneSocket newSocket = {};
		newSocket.mBoneIndex = _boneIndex;

		mSockets.push_back(newSocket);

		return &mSockets.back();
	}

	void BoneAnimator::UpdateSocketInfo()
	{
		size_t Size = mSockets.size();
		for (size_t index = 0; index < Size; ++index)
		{

			math::Matrix ownerScale = math::Matrix::CreateScale(1.f);

			math::Vector3 ownerEulerRot = GetOwner()->GetRotation();

			Matrix ownerRot;
			ownerRot = Matrix::CreateRotationX(DegreeToRadian(ownerEulerRot.x));
			ownerRot *= Matrix::CreateRotationY(DegreeToRadian(ownerEulerRot.y));
			ownerRot *= Matrix::CreateRotationZ(DegreeToRadian(ownerEulerRot.z));
			math::Matrix ownerPos = math::Matrix::CreateTranslation(GetOwner()->GetPhysicalWorldPosition());

			Matrix ownerWorldMat = ownerScale * ownerRot * ownerPos;

			mSockets[index].DecomposeMatrix(ownerWorldMat, mSocketMatrix[mSockets[index].mBoneIndex]);
		}
	}

	const Matrix& BoneAnimator::GetSocketMatrix(int index)
	{
		int findResultIndex = index;
		int maxSocketCount = static_cast<int>(mSocketMatrix.size());
		if (findResultIndex < 0)
		{
			findResultIndex = 0;
		}
		else if (findResultIndex > maxSocketCount)
		{
			findResultIndex = maxSocketCount;
		}

		mbUseSocket = true;
		return mSocketMatrix[findResultIndex];
	}

	void BoneAnimator::AddAnimationClip(const std::wstring& _wstrName, int _startFrame, int _endFrame, fbxsdk::FbxTime::EMode _frameMode)
	{
		bool findResult = CheckIfAnimationExists(_wstrName);

		if (!findResult)
		{
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

			mAnimationClips->push_back(newClip);

			Events* events = new Events();
			events->mEvents.resize(newClip.originFrameLength);
			mEvents.insert(std::make_pair(_wstrName, events));
		}
		else
		{
			Events* events = new Events();
			int clipNum = FindClipNumber(_wstrName);
			
			events->mEvents.resize((mAnimationClips->begin() + clipNum)->originFrameLength);
			mEvents.insert(std::make_pair(_wstrName, events));
		}

		return;
	}

	int BoneAnimator::FindClipNumber(const std::string& _clibNameStr)
	{
		return FindClipNumber(StringToWString(_clibNameStr));
	}
	int BoneAnimator::FindClipNumber(const std::wstring& _clibNameWStr)
	{
		int findResult = -1;

		size_t Size = mAnimationClips->size();
		std::vector<BoneAnimationClip>::const_iterator iter = mAnimationClips->begin();

		for (size_t index = 0; index < Size; ++index)
		{
			if (_clibNameWStr._Equal((iter + index)->name))
			{
				findResult = static_cast<int>(index);
			}
		}

		return findResult;
	}

	bool BoneAnimator::CheckIfAnimationExists(UINT _clipNumber)
	{
		bool result = false;
		
		size_t Size = mAnimationClips->size();
		size_t clipIndex = static_cast<size_t>(_clipNumber);

		if (clipIndex < Size)
		{
			result = true;
		}

		return result;
	}
	bool BoneAnimator::CheckIfAnimationExists(const std::string& _clipNameStr)
	{
		return CheckIfAnimationExists(StringToWString(_clipNameStr));
	}
	bool BoneAnimator::CheckIfAnimationExists(const std::wstring& _clipNameWStr)
	{
		bool result = false;

		std::wstring animClipName;

		size_t Size = mAnimationClips->size();
		std::vector<BoneAnimationClip>::const_iterator iter = mAnimationClips->begin();

		for (size_t index = 0; index < Size; ++index)
		{
			if (_clipNameWStr._Equal((iter + index)->name))
			{
				result = true;
			}
		}

		return result;
	}

	void BoneAnimator::ChangeAnimation(UINT _clipNumber)
	{
		if (mAnimationUpdateTime.size() != mAnimationClips->size())
		{
			mAnimationUpdateTime.resize(mAnimationClips->size());
		}

		bool findResult = CheckIfAnimationExists(_clipNumber);
		if (!findResult)
		{
			return;
		}

		/// 애니메이션 변경에 따른 종료 이벤트 처리
		if (CheckIfAnimationExists(mCurrentClip))
		{
			Events* events = FindEvents(mAnimationClips->at(mCurrentClip).name);
			if (events)
			{
				events->mEndEvent();
				events->ResetExecute();
			}
		}

		mCurrentClip = static_cast<int>(_clipNumber);
		mFrameIdx = mAnimationClips->at(mCurrentClip).originStartFrame;
		mAnimationUpdateTime[mCurrentClip] = 0.f;
		
		// 현재 프레임 재생 시간을 0으로 변경
		mCurrentTime = 0.0;

		/// 애니메이션 시작 이벤트 처리
		Events* events = FindEvents(mAnimationClips->at(mCurrentClip).name);
		if (events)
		{
			events->mStartEvent();
		}
	}
	void BoneAnimator::ChangeAnimation(const std::string& _clipNameStr)
	{
		ChangeAnimation(StringToWString(_clipNameStr));
	}
	void BoneAnimator::ChangeAnimation(const std::wstring& _clipNameWStr)
	{
		int foundClipNumber = FindClipNumber(_clipNameWStr);
		if (-1 == foundClipNumber)
		{
			AssertEx(false, L"BoneAnimator::ChangeAnimation - 전달받은 이름과 일치하는 애니메이션을 찾아내지 못했습니다.");
			return;
		}

		ChangeAnimation(foundClipNumber);
	}

	void BoneAnimator::SetAnimationLoop(UINT _clipNumber, bool _isLoop)
	{
		bool findResult = CheckIfAnimationExists(_clipNumber);
		if (!findResult)
		{
			AssertEx(false, L"BoneAnimator::SetAnimationLoop - 전달받은 인덱스의 애니메이션이 존재하지 않습니다.");
			return;
		}
		std::vector<BoneAnimationClip>* animClipsPtr = const_cast<std::vector<BoneAnimationClip>*>(mAnimationClips);
		(*animClipsPtr)[_clipNumber].isLoop = _isLoop;
	}
	void BoneAnimator::SetAnimationLoop(const std::string& _clipNameStr, bool _isLoop)
	{
		SetAnimationLoop(StringToWString(_clipNameStr), _isLoop);
	}
	void BoneAnimator::SetAnimationLoop(const std::wstring& _clipNameWStr, bool _isLoop)
	{
		int foundClipNumber = FindClipNumber(_clipNameWStr);
		if (-1 == foundClipNumber)
		{
			AssertEx(false, L"BoneAnimator::SetAnimationLoop - 전달받은 이름과 일치하는 애니메이션을 찾아내지 못했습니다.");
			return;
		}

		SetAnimationLoop(foundClipNumber, _isLoop);
	}

	void BoneAnimator::SetAnimEvents(std::map<std::wstring, Events*>* _events)
	{
		//mEvents = _events;
		for (auto event : *_events)
		{
			Events* events = new Events();
			size_t Size = event.second->mEvents.size();
			events->mEvents.resize(Size);
			for (size_t index = 0; index < Size; ++index)
			{
				events->mEvents[index].mEvent = event.second->mEvents[index].mEvent;
			}

			if (event.second->mStartEvent.mEvent)
			{
				events->mStartEvent.mEvent = event.second->mStartEvent.mEvent;
			}
			if (event.second->mEndEvent.mEvent)
			{
				events->mEndEvent.mEvent = event.second->mEndEvent.mEvent;
			}
			if (event.second->mCompleteEvent.mEvent)
			{
				events->mCompleteEvent.mEvent = event.second->mCompleteEvent.mEvent;
			}

			mEvents.insert(std::make_pair(event.first, events));
		}
	}

	Events* BoneAnimator::FindEvents(const std::wstring& name)
	{
		std::map<std::wstring, Events*>::iterator iter
			= mEvents.find(name);

		if (iter == mEvents.end())
		{
			return nullptr;
		}

		return iter->second;
	}
	std::function<void()>& BoneAnimator::GetStartEvent(const std::wstring& name)
	{
		Events* events = FindEvents(name);

		return events->mStartEvent.mEvent;
	}
	std::function<void()>& BoneAnimator::GetCompleteEventRef(const std::wstring& name)
	{
		Events* events = FindEvents(name);

		return events->mCompleteEvent.mEvent;
	}
	std::function<void()>& BoneAnimator::GetEndEventRef(const std::wstring& name)
	{
		Events* events = FindEvents(name);

		return events->mEndEvent.mEvent;
	}
	std::function<void()>& BoneAnimator::GetEventRef(const std::wstring& name, UINT index)
	{
		Events* events = FindEvents(name);

		return events->mEvents[index].mEvent;
	}
	std::function<void()>* BoneAnimator::GetStartEventPtr(const std::wstring& name)
	{
		Events* events = FindEvents(name);

		if (!events)
		{
			return nullptr;
		}

		return &(events->mStartEvent.mEvent);
	}
	std::function<void()>* BoneAnimator::GetCompleteEventPtr(const std::wstring& name)
	{
		Events* events = FindEvents(name);

		if (!events)
		{
			return nullptr;
		}

		return &(events->mCompleteEvent.mEvent);
	}
	std::function<void()>* BoneAnimator::GetEndEventPtr(const std::wstring& name)
	{
		Events* events = FindEvents(name);

		if (!events)
		{
			return nullptr;
		}

		return &(events->mEndEvent.mEvent);
	}
	std::function<void()>* BoneAnimator::GetEventPtr(const std::wstring& name, UINT index)
	{
		Events* events = FindEvents(name);

		if (!events)
		{
			return nullptr;
		}

		return &(events->mEvents[index].mEvent);
	}
}
