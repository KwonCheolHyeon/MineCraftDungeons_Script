#pragma once
#include "yaComponent.h"

class GameObject;
class dtNavMesh;

class DetourManager
{
public:
	DetourManager();
	~DetourManager();

    static void Initalize();
    static void ChangeScene(eSceneType _sceneType);
    static void LoadNavMeshFromBinFile(const std::wstring& _path);
    static void GetIndexAndVertex(const std::wstring& _path);

    static std::vector<std::vector<physx::PxVec3>> GetVertexFromBinFile(const std::wstring& _path);
    static std::vector<std::vector<physx::PxU32>> GetIndexFromBinFile(const std::wstring& _path);

    static void Release();

    static md::GameObject* GetPlayer() { return mPlayer; }
    static void SetPlayer(md::GameObject* player) { mPlayer = player; }

    static md::math::Vector3* GetPathtoTarget(md::math::Vector3 _startPos, int* _actualPathCount);
    static md::math::Vector3* GetPathtoTarget(md::math::Vector3 _startPos, md::math::Vector3 _targetPos, int* _actualPathCount);

    static float GetDirtoTarget(md::math::Vector3 _startPos);
    static float GetSmoothDirtoTarget(GameObject* _startObj, float _degree);

private:
    static md::GameObject* mPlayer; // static으로 변경
    static dtNavMesh* mNaviMesh; // static으로 변경

    static const int NAVMESHSET_MAGIC = 'M' << 24 | 'S' << 16 | 'E' << 8 | 'T';
    static const int NAVMESHSET_VERSION = 1;

    std::vector<std::vector<physx::PxVec3>> mVertexVec;
    std::vector<std::vector<physx::PxU32>> mIndexVec;
};
