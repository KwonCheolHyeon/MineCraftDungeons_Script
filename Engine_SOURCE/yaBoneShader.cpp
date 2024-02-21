#include "yaBoneShader.h"
#include "yaConstantBuffer.h"
#include "yaRenderer.h"


namespace md::graphics
{
	BoneShader::BoneShader()
		: mFrameDataBuffer(nullptr)
		, mOffsetMatBuffer(nullptr)			
		, mOutputBuffer(nullptr)
		, mBoneAnimationData{}
		//, mBoneSocketBuffer(nullptr)
	{

	}

	BoneShader::~BoneShader()
	{
		if (mFrameDataBuffer)
		{
			mFrameDataBuffer->Clear();
			SAFE_DELETE(mFrameDataBuffer);			// t13
		}
		if (mOffsetMatBuffer)
		{
			mOffsetMatBuffer->Clear();
			SAFE_DELETE(mOffsetMatBuffer);			// t14 
		}
		if (mOutputBuffer)
		{
			mOutputBuffer->Clear();
			SAFE_DELETE(mOutputBuffer);				// u2
		}
		//if (mBoneSocketBuffer)
		//{
		//	mBoneSocketBuffer->Clear();
		//	SAFE_DELETE(mBoneSocketBuffer);				// u1
		//}
	}

	void BoneShader::Binds()
	{
		ConstantBuffer* cb = renderer::constantBuffers[(UINT)eCBType::Bone];
		cb->SetData(&mBoneAnimationData);
		cb->Bind(eShaderStage::VS);
		cb->Bind(eShaderStage::PS);
		cb->Bind(eShaderStage::CS);
		//mBoneAnimationData

		mFrameDataBuffer->BindSRV(eShaderStage::CS, 17);
		mOffsetMatBuffer->BindSRV(eShaderStage::CS, 18);
		mOutputBuffer->BindUAV(eShaderStage::CS, 2);

		mGroupX = (mBoneAnimationData.boneCount / mThreadGroupCountX + 1);
		mGroupY = 1;
		mGroupZ = 1;
	}

	void BoneShader::Clear()
	{
		if (mFrameDataBuffer)
		{
			mFrameDataBuffer->Clear();
			mFrameDataBuffer = nullptr;
		}
		if (mOffsetMatBuffer)
		{
			mOffsetMatBuffer->Clear();
			mOffsetMatBuffer = nullptr;
		}
		if (mOutputBuffer)
		{
			mOutputBuffer->Clear();
			mOutputBuffer = nullptr;
		}
	}

}
