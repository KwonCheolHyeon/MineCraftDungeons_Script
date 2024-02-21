#pragma once
#include "yaEngine.h"
#include "yaMath.h"
#include "yaGraphics.h"

#include <fbxsdk/scene/geometry/fbxnurbs.h >
#include <fbxsdk.h>

#ifdef _DEBUG
#pragma comment(lib, "..\\External\\fbx\\lib\\Debug\\libfbxsdk-md.lib") 
#else 
#pragma comment(lib, "..\\External\\fbx\\lib\\Release\\libfbxsdk-md.lib") 
#endif

//#ifdef _DEBUG
//#pragma comment(lib, "..\\External\\fbx\\lib\\Debug\\libxml2-md.lib") 
//#else 
//#pragma comment(lib, "..\\External\\fbx\\lib\\Release\\libxml2-md.lib") 
//#endif

namespace md
{
	using namespace math;
	using namespace graphics;

	struct FbxMaterial
	{
		MaterialColr color;

		std::wstring name;
		std::wstring diffuse;
		std::wstring normal;
		std::wstring specular;
		std::wstring emissive;
	};

	struct WeightAndIndices
	{
		int boneIdx;
		double weight;
	};

	struct Container
	{
		std::wstring name;
		std::vector<Vector3> positions;
		std::vector<Vector3> tangents;
		std::vector<Vector3> binormals;
		std::vector<Vector3> normals;
		std::vector<Vector2> uv;

		std::vector<std::vector<UINT>> indices;
		std::vector<FbxMaterial> materials;

		bool bAnimation;
		std::vector<std::vector<WeightAndIndices>> weightAndIndices;
		std::vector<Vector4> skiningWeights;
		std::vector<Vector4> skiningIndices;

		void Resize(UINT size)
		{
			positions.resize(size);
			tangents.resize(size);
			binormals.resize(size);
			normals.resize(size);
			uv.resize(size);
			indices.resize(size);
			weightAndIndices.resize(size);

			skiningWeights.resize(size);
			skiningIndices.resize(size);
		}
	};

	struct KeyFrame
	{
		fbxsdk::FbxAMatrix transform;
		double time;
	};

	struct Bone
	{
		std::wstring name;
		int depth;		//계층구조 깊이
		int parentIdx; //부모의 본 인덱스
		fbxsdk::FbxAMatrix offsetMatrix; // 나 --> 부모 --> 뿌리 --> local
		fbxsdk::FbxAMatrix boneMatrix;
		std::vector<KeyFrame> keyFrames;

		Bone()
			: depth(0)
			, parentIdx(0)
		{
		}
	};

	struct AnimationClip
	{
		std::wstring name;
		fbxsdk::FbxTime startTime;
		fbxsdk::FbxTime endTime;
		fbxsdk::FbxLongLong timeLength;
		fbxsdk::FbxTime::EMode mode;

		AnimationClip()
			: timeLength{}
			, mode(fbxsdk::FbxTime::EMode::eDefaultMode)
		{
		}
	};

	Matrix GetMatrixFromFbxMatrix(fbxsdk::FbxAMatrix& _mat);

	class FbxLoader
	{
	public:
		~FbxLoader();

	public:
		static void Initialize();
		static bool LoadFbx(const std::wstring& path);
		static void LoadMeshDataFromNode(fbxsdk::FbxNode* node);
		static void LoadMesh(fbxsdk::FbxMesh* _pFbxMesh);
		static void LoadMaterial(fbxsdk::FbxSurfaceMaterial* _pMtrlSur);

		static void GetTangent(fbxsdk::FbxMesh* _pMesh, Container* _pContainer, int _iIdx, int _iVtxOrder);
		static void GetBinormal(fbxsdk::FbxMesh* _pMesh, Container* _pContainer, int _iIdx, int _iVtxOrder);
		static void GetNormal(fbxsdk::FbxMesh* _pMesh, Container* _pContainer, int _iIdx, int _iVtxOrder);
		static void GetUV(fbxsdk::FbxMesh* _pMesh, Container* _pContainer, int _iIdx, int _iUVIndex);

		static Vector4 GetMtrlData(fbxsdk::FbxSurfaceMaterial* _pSurface, const char* _pMtrlName, const char* _pMtrlFactorName);
		static std::wstring GetMtrlTextureName(fbxsdk::FbxSurfaceMaterial* _pSurface, const char* _pMtrlProperty);

		static void LoadTexture();
		static void CreateMaterial();

		// Animation
		static void LoadSkeleton(fbxsdk::FbxNode* _pNode);
		static void LoadSkeleton_Re(fbxsdk::FbxNode* _pNode, int _iDepth, int _iIdx, int _iParentIdx);
		static void LoadAnimationClip();
		static void Triangulate(fbxsdk::FbxNode* _pNode);

		static void LoadAnimationData(fbxsdk::FbxMesh* _pMesh, Container* _pContainer);
		static void CheckWeightAndIndices(fbxsdk::FbxMesh* _pMesh, Container* _pContainer);
		static void LoadWeightsAndIndices(fbxsdk::FbxCluster* _pCluster, int _iBoneIdx, Container* _pContainer);
		static void LoadOffsetMatrix(fbxsdk::FbxCluster* _pCluster, const fbxsdk::FbxAMatrix& _matNodeTransform, int _iBoneIdx, Container* _pContainer);
		static void LoadKeyframeTransform(fbxsdk::FbxNode* _pNode, fbxsdk::FbxCluster* _pCluster, const fbxsdk::FbxAMatrix& _matNodeTransform
			, int _iBoneIdx, Container* _pContainer);
		static int FindBoneIndex(std::string _strBoneName);
		static fbxsdk::FbxAMatrix GetTransform(fbxsdk::FbxNode* _pNode);

		static void Release();
		
		static int GetContainerCount() { return (int)mContainers.size(); }
		static const Container& GetContainer(int idx) { return mContainers[idx]; }
		static std::vector<Bone*>& GetBones() { return mBones; }
		static std::vector<AnimationClip*>& GetAnimClip() { return mAnimationClips; }


	private:
		static fbxsdk::FbxManager* mManager;
		static fbxsdk::FbxScene* mScene;
		static fbxsdk::FbxImporter* mImporter;
		static std::vector<Container> mContainers;

		//Animation
		static std::vector<Bone*> mBones;
		static fbxsdk::FbxArray<fbxsdk::FbxString*> mAnimationNames;
		static std::vector<AnimationClip*> mAnimationClips;
	};


}
