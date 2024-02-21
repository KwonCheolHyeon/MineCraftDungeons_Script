#pragma once
#include "yaGraphics.h"
#include "yaResource.h"
#include "yaFbxLoader.h"
#include "yaStructedBuffer.h"
#include "mdStruct.h"

namespace md
{
	struct IndexInfo
	{
		Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
		D3D11_BUFFER_DESC desc;
		UINT indexCount;
		void* pIdxSysMem;
	};

	class Mesh : public Resource 
	{
	public:
		Mesh();
		virtual ~Mesh();

		static std::shared_ptr<Mesh> CreateFromContainer(FbxLoader* loader);

		virtual HRESULT Save(const std::wstring& name, FILE* file = nullptr) override;
		virtual HRESULT Load(const std::wstring& name, FILE* file = nullptr) override;

		bool CreateVertexBuffer(void* data, UINT count);
		bool CreateIndexBuffer(void* data, UINT count);
		void BindBuffer(UINT subSet);
		void Render(UINT subSet);
		void RenderInstanced(UINT count, UINT subSet = 0);

		UINT GetSubSetCount() { return (UINT)mIndexInfos.size(); } 
		graphics::Vertex* GetVtxSysMem() { return (graphics::Vertex*)pVtxSysMem; }

		const std::vector<BoneMatrix>* GetBones() { return &mBones; }
		UINT GetBoneCount() { return (UINT)mBones.size(); }
		const std::vector<BoneAnimationClip>* GetAnimClip() { return &mAnimClip; }
		bool IsAnimMesh() { return !mAnimClip.empty(); }
		bool FindAnimClip(const std::wstring& _wstrName);
		bool AddAnimationClip(const std::wstring& _wstrName, int _startFrame, int _endFrame
			, fbxsdk::FbxTime::EMode _frameMode = fbxsdk::FbxTime::EMode::eFrames24);
		std::map<std::wstring, Events*>* GetEvents() { return &mEvents; }

		graphics::StructedBuffer* GetBoneFrameDataBuffer() { return mBoneFrameData; } // 전체 본 프레임 정보
		graphics::StructedBuffer* GetBoneOffsetBuffer() { return  mBoneOffset; }	   // 각 뼈의 offset 행렬	   

	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> mVertexBuffer;
		D3D11_BUFFER_DESC mVBDesc;
		UINT mVtxCount;
		void* pVtxSysMem;

		std::vector<IndexInfo> mIndexInfos;

		//3D Animation 정보
		std::vector<BoneAnimationClip> mAnimClip;
		std::map<std::wstring, Events*> mEvents;
		std::vector<BoneMatrix> mBones;

		graphics::StructedBuffer* mBoneFrameData; // 전체 본 프레임 정보 ( 크기, 이름, 회전) 프레임 갯수만큼
		graphics::StructedBuffer* mBoneOffset; // 각 뼈의 offset 행렬 () 각뼈의 위치를 TPOSE로 되돌리는 행렬
	};
}
