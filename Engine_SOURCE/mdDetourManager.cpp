#include "mdDetourManager.h"

#include "DetourNavMesh.h"
#include "DetourNavMeshBuilder.h"
#include "DetourNavMeshQuery.h"

#include "yaSceneManager.h"
#include "yaTransform.h"

#include <fstream>

struct NavMeshSetHeader
{
    int magic; 
    int version;
    int numTiles;
    dtNavMeshParams params;
};

struct NavMeshTileHeader
{
    dtTileRef tileRef;
    int dataSize;
};

md::GameObject* DetourManager::mPlayer = nullptr;
dtNavMesh* DetourManager::mNaviMesh = nullptr;

DetourManager::DetourManager()
{

}
DetourManager::~DetourManager()
{

}

void DetourManager::Initalize()
{
  
}

void DetourManager::ChangeScene(eSceneType _sceneType)
{
    if (nullptr != mNaviMesh)
        delete mNaviMesh;
    mNaviMesh = nullptr;
    mPlayer = nullptr;

  
    switch (_sceneType)
    {
    case md::enums::eSceneType::TitleScene:
        break;
    case md::enums::eSceneType::PlayScene:
        break;
    case md::enums::eSceneType::UITestScene:
        break;
    case md::enums::eSceneType::PhysXTestScene:
        break;
    case md::enums::eSceneType::AnimationFunctionTestScene:
        break;
    case md::enums::eSceneType::NavTestScene:
        GetIndexAndVertex(L"Nav\\NavTest.bin");
        break;
    case md::enums::eSceneType::End:
        break;
    default:
        break;
    }
}

void DetourManager::LoadNavMeshFromBinFile(const std::wstring& _path)
{

    std::filesystem::path parentPath = std::filesystem::current_path().parent_path();
    std::wstring fullPath = parentPath.wstring() + L"\\Resources\\" + _path;

    FILE* fp = nullptr;
    errno_t err = _wfopen_s(&fp, fullPath.c_str(), L"rb");
    if (err != 0 || !fp)
    {
        return;
    }
    
    // Read header.
    NavMeshSetHeader header;
    fread(&header, sizeof(NavMeshSetHeader), 1, fp);

    if (header.magic != NAVMESHSET_MAGIC)
    {
        fclose(fp);
        return;
    }
    if (header.version != NAVMESHSET_VERSION)
    {
        fclose(fp);
        return;
    }

    mNaviMesh = dtAllocNavMesh();

    if (!mNaviMesh)
    {
        fclose(fp);
        return;
    }

    dtStatus status = mNaviMesh->init(&header.params);
    if (dtStatusFailed(status))
    {
        fclose(fp);
        return;
    }

    // Read tiles.
    for (int i = 0; i < header.numTiles; ++i)
    {
        NavMeshTileHeader tileHeader;
        fread(&tileHeader, sizeof(tileHeader), 1, fp);
        if (!tileHeader.tileRef || !tileHeader.dataSize)
            break;

        unsigned char* data = (unsigned char*)dtAlloc(tileHeader.dataSize, DT_ALLOC_PERM);
        if (!data) break;
        memset(data, 0, tileHeader.dataSize);
        fread(data, tileHeader.dataSize, 1, fp);

        mNaviMesh->addTile(data, tileHeader.dataSize, DT_TILE_FREE_DATA, tileHeader.tileRef, 0);
    }

    fclose(fp);
}

void DetourManager::GetIndexAndVertex(const std::wstring& _path)
{
    std::filesystem::path parentPath = std::filesystem::current_path().parent_path();
    std::wstring fullPath = parentPath.wstring() + L"\\Resources\\" + _path;

    FILE* fp = nullptr;
    errno_t err = _wfopen_s(&fp, fullPath.c_str(), L"rb");
    if (err != 0 || !fp) return;

    NavMeshSetHeader header;
    fread(&header, sizeof(NavMeshSetHeader), 1, fp);

    if (header.magic != NAVMESHSET_MAGIC || header.version != NAVMESHSET_VERSION)
    {
        fclose(fp);
        return;
    }

    mNaviMesh = dtAllocNavMesh();
    if (!mNaviMesh)
    {
        fclose(fp);
        return;
    }

    mNaviMesh->init(&header.params);

    // 파일을 읽어오고 NavMesh 정보를 추가한다 (addTile)
    for (int i = 0; i < header.numTiles; ++i)
    {
        NavMeshTileHeader tileHeader;
        fread(&tileHeader, sizeof(NavMeshTileHeader), 1, fp);

        if (!tileHeader.tileRef || !tileHeader.dataSize) break;

        unsigned char* data = (unsigned char*)dtAlloc(tileHeader.dataSize, DT_ALLOC_PERM);
        if (!data) break;
        fread(data, tileHeader.dataSize, 1, fp);

        mNaviMesh->addTile(data, tileHeader.dataSize, DT_TILE_FREE_DATA, tileHeader.tileRef, nullptr);
    }

    fclose(fp);

    // 타일 데이터를 접근해서 버텍스, 인덱스 정보를 추출한다.
    int maxTiles = mNaviMesh->getMaxTiles();
    for (int tileIndex = 0; tileIndex < maxTiles; ++tileIndex)
    {
        dtTileRef tileRef = mNaviMesh->getTileRefAt(tileIndex, 0, 0);
        if (!tileRef) continue;

        const dtMeshTile* tile = mNaviMesh->getTileByRef(tileRef);
        if (!tile || !tile->header) continue;

        int polyCount = tile->header->polyCount;
        for (int polyIndex = 0; polyIndex < polyCount; ++polyIndex)
        {
            const dtPoly* poly = &tile->polys[polyIndex];
            for (int vertIndex = 0; vertIndex < poly->vertCount; ++vertIndex)
            {
                int vertexRef = poly->verts[vertIndex];
                const float* vertexPos = &tile->verts[vertexRef * 3];
            }
        }
    }

}

std::vector<std::vector<physx::PxVec3>> DetourManager::GetVertexFromBinFile(const std::wstring& _path)
{
    std::filesystem::path parentPath = std::filesystem::current_path().parent_path();
    std::wstring fullPath = parentPath.wstring() + L"\\Resources\\" + _path;

    FILE* fp = nullptr;
    errno_t err = _wfopen_s(&fp, fullPath.c_str(), L"rb");
    if (err != 0 || !fp) return {};

    NavMeshSetHeader header;
    fread(&header, sizeof(NavMeshSetHeader), 1, fp);
    if (header.magic != NAVMESHSET_MAGIC || header.version != NAVMESHSET_VERSION)
    {
        fclose(fp);
        return {};
    }

    mNaviMesh = dtAllocNavMesh();
    if (!mNaviMesh)
    {
        fclose(fp);
        return {};
    }

    mNaviMesh->init(&header.params);
    for (int i = 0; i < header.numTiles; ++i)
    {
        NavMeshTileHeader tileHeader;
        fread(&tileHeader, sizeof(NavMeshTileHeader), 1, fp);
        if (!tileHeader.tileRef || !tileHeader.dataSize) break;

        unsigned char* data = (unsigned char*)dtAlloc(tileHeader.dataSize, DT_ALLOC_PERM);
        if (!data) break;
        fread(data, tileHeader.dataSize, 1, fp);

        mNaviMesh->addTile(data, tileHeader.dataSize, DT_TILE_FREE_DATA, tileHeader.tileRef, nullptr);
    }
    fclose(fp);

    std::vector<std::vector<physx::PxVec3>> allVertexes;
    int maxTiles = mNaviMesh->getMaxTiles();
    for (int tileIndex = 0; tileIndex < maxTiles; ++tileIndex)
    {
        dtTileRef tileRef = mNaviMesh->getTileRefAt(tileIndex, 0, 0);
        if (!tileRef) continue;

        const dtMeshTile* tile = mNaviMesh->getTileByRef(tileRef);
        if (!tile || !tile->header) continue;

        int polyCount = tile->header->polyCount;
        for (int polyIndex = 0; polyIndex < polyCount; ++polyIndex)
        {
            const dtPoly* poly = &tile->polys[polyIndex];
            std::vector<physx::PxVec3> polygonVertices;

            for (int vertIndex = 0; vertIndex < poly->vertCount; ++vertIndex)
            {
                int vertexRef = poly->verts[vertIndex];
                const float* vertexPos = &tile->verts[vertexRef * 3];
                polygonVertices.push_back(physx::PxVec3(vertexPos[0], vertexPos[1], vertexPos[2]));
            }

            allVertexes.push_back(polygonVertices);
        }
    }
    return allVertexes;
}

std::vector<std::vector<physx::PxU32>> DetourManager::GetIndexFromBinFile(const std::wstring& _path)
{
    std::filesystem::path parentPath = std::filesystem::current_path().parent_path();
    std::wstring fullPath = parentPath.wstring() + L"\\Resources\\" + _path;

    FILE* fp = nullptr;
    errno_t err = _wfopen_s(&fp, fullPath.c_str(), L"rb");
    if (err != 0 || !fp) return {};

    NavMeshSetHeader header;
    fread(&header, sizeof(NavMeshSetHeader), 1, fp);
    if (header.magic != NAVMESHSET_MAGIC || header.version != NAVMESHSET_VERSION)
    {
        fclose(fp);
        return {};
    }

    mNaviMesh = dtAllocNavMesh();
    if (!mNaviMesh)
    {
        fclose(fp);
        return {};
    }

    mNaviMesh->init(&header.params);
    for (int i = 0; i < header.numTiles; ++i)
    {
        NavMeshTileHeader tileHeader;
        fread(&tileHeader, sizeof(NavMeshTileHeader), 1, fp);
        if (!tileHeader.tileRef || !tileHeader.dataSize) break;

        unsigned char* data = (unsigned char*)dtAlloc(tileHeader.dataSize, DT_ALLOC_PERM);
        if (!data) break;
        fread(data, tileHeader.dataSize, 1, fp);

        mNaviMesh->addTile(data, tileHeader.dataSize, DT_TILE_FREE_DATA, tileHeader.tileRef, nullptr);
    }
    fclose(fp);


    std::vector<std::vector<physx::PxU32>> allIndexes;
    int maxTiles = mNaviMesh->getMaxTiles();
    for (int tileIndex = 0; tileIndex < maxTiles; ++tileIndex)
    {
        dtTileRef tileRef = mNaviMesh->getTileRefAt(tileIndex, 0, 0);
        if (!tileRef) continue;

        const dtMeshTile* tile = mNaviMesh->getTileByRef(tileRef);
        if (!tile || !tile->header) continue;

        int polyCount = tile->header->polyCount;
        for (int polyIndex = 0; polyIndex < polyCount; ++polyIndex)
        {
            const dtPoly* poly = &tile->polys[polyIndex];
            std::vector<physx::PxU32> polygonIndices;

            if (poly->vertCount == 3) // Assuming triangles for simplicity
            {
                polygonIndices.push_back(poly->verts[2]);
                polygonIndices.push_back(poly->verts[0]); // Swapped
                polygonIndices.push_back(poly->verts[1]); // Swapped
            }
            else
            {
                for (int vertIndex = 0; vertIndex < poly->vertCount; ++vertIndex)
                {
                    polygonIndices.push_back(poly->verts[vertIndex]);
                }
            }

            allIndexes.push_back(polygonIndices);
        }
    }
    return allIndexes;

}

void DetourManager::Release()
{
    if (nullptr != mNaviMesh)
    {
        delete mNaviMesh;
    }
}

md::math::Vector3* DetourManager::GetPathtoTarget(md::math::Vector3 _startPos, int* _actualPathCount)
{
    if (nullptr == mPlayer)
    {
        //mPlayer = CLevelMgr::GetInst()->GetCurLevel()->FindObjectByName(L"Player");
        return nullptr;
    }

    auto transform = mPlayer->GetTransform();
    if (nullptr == transform)
    {
        return nullptr; 
    }

    return GetPathtoTarget(_startPos, transform->GetPosition(), _actualPathCount);
}

md::math::Vector3* DetourManager::GetPathtoTarget(md::math::Vector3 _startPos, md::math::Vector3 _targetPos, int* _actualPathCount)
{
    if (nullptr == mNaviMesh)
        assert(nullptr);

    float actualPath[256 * 3] = { 0.f, };
    for (int i = 0; i < 256 * 3; i++)
        actualPath[i] = 0.0f;

    // NavMesh와 함께 경로 계획을 수행하는 Query 객체 생성
    dtNavMeshQuery* navQuery = dtAllocNavMeshQuery();
    navQuery->init(mNaviMesh, 4866);

    // 시작, 도착 위치 설정
    float startpos[3] = {};
    startpos[0] = _startPos.x;
    startpos[1] = _startPos.y;
    startpos[2] = -_startPos.z;

    float endpos[3] = {};
    md::math::Vector3 vEndPos = _targetPos;
    endpos[0] = vEndPos.x;
    endpos[1] = vEndPos.y;
    endpos[2] = -vEndPos.z;

    dtPolyRef startRef, endRef;
    float polyPickExt[3] = { 30000,30000,30000 }; // 범위를 제한하기 위한 벡터

    dtQueryFilter filter;
    filter.setIncludeFlags(0xFFFF); // Include all polygons in pathfinding.
    filter.setExcludeFlags(0); // Exclude none of the polygons.

    navQuery->findNearestPoly(startpos, polyPickExt, &filter, &startRef, 0);
    navQuery->findNearestPoly(endpos, polyPickExt, &filter, &endRef, 0);

    // 시작과 도착 위치까지 경로 탐색
    float nearestStartPos[3], nearestEndPos[3];
    navQuery->closestPointOnPoly(startRef, startpos, nearestStartPos, 0);
    navQuery->closestPointOnPoly(endRef, endpos, nearestEndPos, 0);

    // 경로를 계획.
    dtPolyRef path[256];
    int pathCount;
    navQuery->findPath(startRef, endRef, nearestStartPos, nearestEndPos, &filter, path, &pathCount, 256);

    // 경로를 따라 실제 이동 경로를 생성
    navQuery->findStraightPath(nearestStartPos, nearestEndPos, path, pathCount, actualPath, 0, 0, _actualPathCount, 256);

    // Query 객체 할당 해제
    dtFreeNavMeshQuery(navQuery);

    md::math::Vector3 Path[256] = {};
    for (int i = 0; i < 256; ++i)
    {
        Path[i] = md::math::Vector3(actualPath[i * 3], actualPath[i * 3 + 1], actualPath[i * 3 + 2]);
        if (md::math::Vector3(0.f, 0.f, 0.f) == Path[i])
            break;
    }

    return Path;
}

//float DetourManager::GetDirtoTarget(md::math::Vector3 _startPos)
//{
//    md::math::Vector3 vPlayerPos = mPlayer->GetTransform()->GetPosition();
//
//    return GetDir(vPlayerPos, _startPos);;
//}
//
//float DetourManager::GetSmoothDirtoTarget(GameObject* _startObj, float _degree)
//{
//    return GetSmoothDir(_startObj, mPlayer, _degree);
//}
