#include "yaMaterial.h"
#include "yaResources.h"

namespace md::graphics
{
    Material::Material()
        : Resource(eResourceType::Material)
        , mMode(eRenderingMode::Opaque)
        , mbAnimaion(false)
		, mBoneCB{}
    {

    }

    Material::~Material()
    {

    }

    HRESULT Material::Save(const std::wstring& name, FILE* file)
    {
        std::filesystem::path parentPath = std::filesystem::current_path().parent_path();
        std::wstring fullPath = parentPath.wstring() + L"\\Resources\\Material\\" + name;

        std::wstring key = GetKey();

        UINT nameSize = static_cast<UINT>(key.size());
        fwrite(&nameSize, sizeof(UINT), 1, file);
        fwrite(key.c_str(), key.size() * sizeof(wchar_t), 1, file);

        UINT pathSize = static_cast<UINT>(fullPath.size());
        fwrite(&pathSize, sizeof(UINT), 1, file);
        fwrite(fullPath.c_str(), fullPath.size() * sizeof(wchar_t), 1, file);

        // 해당 정보들 저장 해주어야 한다.
        //std::shared_ptr<Shader> mShader;
        key = mShader->GetKey();
        fullPath = mShader->GetPath();
        //fullPath = L"..\\SHADER_SOURCE\\";


        nameSize = static_cast<UINT>(key.size());
        fwrite(&nameSize, sizeof(UINT), 1, file);
        fwrite(key.c_str(), key.size() * sizeof(wchar_t), 1, file);

        pathSize = static_cast<UINT>(fullPath.size());
        fwrite(&pathSize, sizeof(UINT), 1, file);
        fwrite(fullPath.c_str(), fullPath.size() * sizeof(wchar_t), 1, file);


        //std::shared_ptr<Texture> mTexture[(UINT)eTextureSlot::End];
        for (size_t i = 0; i < (UINT)eTextureSlot::End; i++)
        {
            if (mTexture[i] == nullptr)
            {
                int j = 0;
                fwrite(&j, sizeof(int), 1, file);

                continue;
            }
            else
            {
                int j = 1;
                fwrite(&j, sizeof(int), 1, file);
            }

            key = mTexture[i]->GetKey();
            fullPath = mTexture[i]->GetPath();

            nameSize = static_cast<UINT>(key.size());
            fwrite(&nameSize, sizeof(UINT), 1, file);
            fwrite(key.c_str(), key.size() * sizeof(wchar_t), 1, file);

            pathSize = static_cast<UINT>(fullPath.size());
            fwrite(&pathSize, sizeof(UINT), 1, file);
            fwrite(fullPath.c_str(), fullPath.size() * sizeof(wchar_t), 1, file);
        }

        //BoneAnimationCB mBoneCB;
        fwrite(&mBoneCB.boneCount, sizeof(UINT), 1, file);

        if (mBoneCB.boneCount <= 0)
            return S_OK;

        fwrite(&mBoneCB.frameIdx, sizeof(UINT), 1, file);
        fwrite(&mBoneCB.nextFrameIdx, sizeof(UINT), 1, file);
        fwrite(&mBoneCB.frameRatio, sizeof(float), 1, file);

        //eRenderingMode mMode;
        fwrite(&mMode, sizeof(UINT), 1, file);
        fwrite(&mDiffuseColor, sizeof(Vector4), 1, file);
        fwrite(&mSpecularColor, sizeof(Vector4), 1, file);
        fwrite(&mAmbientColor, sizeof(Vector4), 1, file);
        fwrite(&mEmissiveColor, sizeof(Vector4), 1, file);
        fwrite(&mbAnimaion, sizeof(bool), 1, file);

        return S_OK;
    }

    HRESULT Material::Load(const std::wstring& name, FILE* file)
    {
        //read 로 바꿔준다.
        std::filesystem::path parentPath = std::filesystem::current_path().parent_path();
        std::wstring fullPath = parentPath.wstring() + L"\\Resources\\Material\\" + name;


        wchar_t buff[256] = {};
        UINT nameSize = 0;
        fread(&nameSize, sizeof(UINT), 1, file);
        fread(buff, sizeof(wchar_t), nameSize, file);
        std::wstring key = buff;
        SetKey(key);

        UINT pathSize = 0;
        fread(&pathSize, sizeof(UINT), 1, file);
        fread(buff, sizeof(wchar_t), pathSize, file);
        std::wstring path = buff;
        SetPath(buff);

        // 해당 정보들 저장 해주어야 한다.
        //std::shared_ptr<Shader> mShader;
        key = L"";
        path = L"";
        ZeroMemory(buff, 256);

        nameSize = 0;
        fread(&nameSize, sizeof(UINT), 1, file);
        fread(buff, sizeof(wchar_t), nameSize, file);
        key = buff;

        pathSize = 0;
        fread(&pathSize, sizeof(UINT), 1, file);
        fread(buff, sizeof(wchar_t), pathSize, file);
        path = buff;

        // 구조 바뀌어야 한다.
        path = L"..\\SHADER_SOURCE\\Defferd";

        mShader = Resources::Find<Shader>(key);

        //std::shared_ptr<Texture> mTexture[(UINT)eTextureSlot::End];
        for (size_t i = 0; i < (UINT)eTextureSlot::End; i++)
        {
            int j = -1;
            fread(&j, sizeof(int), 1, file);

            if (j == 0)
                continue;

            key = L"";
            path = L"";

            nameSize = 0;
            fread(&nameSize, sizeof(UINT), 1, file);
            fread(buff, sizeof(wchar_t), nameSize, file);
            key = buff;

            pathSize = 0;
            fread(&pathSize, sizeof(UINT), 1, file);
            fread(buff, sizeof(wchar_t), pathSize, file);
            path = buff;

            std::filesystem::path parentPath = std::filesystem::current_path().parent_path();
            std::wstring fullPath = parentPath.wstring() + L"\\Resources\\" + path;

            mTexture[i] = Resources::Load<Texture>(key, path);
        }

        //BoneAnimationCB mBoneCB;
        fread(&mBoneCB.boneCount, sizeof(UINT), 1, file);

        if (mBoneCB.boneCount <= 0)
            return S_OK;

        fread(&mBoneCB.frameIdx, sizeof(UINT), 1, file);
        fread(&mBoneCB.nextFrameIdx, sizeof(UINT), 1, file);
        fread(&mBoneCB.frameRatio, sizeof(float), 1, file);

        //eRenderingMode mMode;
        fread(&mMode, sizeof(UINT), 1, file);
        fread(&mDiffuseColor, sizeof(Vector4), 1, file);
        fread(&mSpecularColor, sizeof(Vector4), 1, file);
        fread(&mAmbientColor, sizeof(Vector4), 1, file);
        fread(&mEmissiveColor, sizeof(Vector4), 1, file);
        fread(&mbAnimaion, sizeof(bool), 1, file);

        
        return E_NOTIMPL;
    }

    void Material::SetData(eGPUParam param, void* data)
    {
        
        
        switch (param)
        {
        case md::graphics::eGPUParam::Int_0:
            mCB.int_0 = *static_cast<int*>(data);
            break;
        case md::graphics::eGPUParam::Int_1:
            mCB.int_1 = *static_cast<int*>(data);
            break;
        case md::graphics::eGPUParam::Int_2:
            mCB.int_2 = *static_cast<int*>(data);
            break;
        case md::graphics::eGPUParam::Int_3:
            mCB.int_3 = *static_cast<int*>(data);
            break;
        case md::graphics::eGPUParam::Float_0:
            mCB.float_0 = *static_cast<float*>(data);
            break;
        case md::graphics::eGPUParam::Float_1:
            mCB.float_1 = *static_cast<float*>(data);
            break;
        case md::graphics::eGPUParam::Float_2:
            mCB.float_2 = *static_cast<float*>(data);
            break;
        case md::graphics::eGPUParam::Float_3:
            mCB.float_3 = *static_cast<float*>(data);
            break;
        case md::graphics::eGPUParam::Vector2_0:
            mCB.float2_0 = *static_cast<Vector2*>(data);
            break;
        case md::graphics::eGPUParam::Vector2_1:
            mCB.float2_1 = *static_cast<Vector2*>(data);
            break;
        case md::graphics::eGPUParam::Vector2_2:
            mCB.float2_2 = *static_cast<Vector2*>(data);
            break;
        case md::graphics::eGPUParam::Vector2_3:
            mCB.float2_3 = *static_cast<Vector2*>(data);
            break;
        case md::graphics::eGPUParam::Vector3_0:
            mCB.float3_0 = *static_cast<Vector3*>(data);
            break;
        case md::graphics::eGPUParam::Vector3_1:
            mCB.float3_1 = *static_cast<Vector3*>(data);
            break;
        case md::graphics::eGPUParam::Vector3_2:
            mCB.float3_2 = *static_cast<Vector3*>(data);
            break;
        case md::graphics::eGPUParam::Vector3_3:
            mCB.float3_3 = *static_cast<Vector3*>(data);
            break;
        case md::graphics::eGPUParam::Vector4_0:
            mCB.float4_0 = *static_cast<Vector4*>(data);
            break;
        case md::graphics::eGPUParam::Vector4_1:
            mCB.float4_1 = *static_cast<Vector4*>(data);
            break;
        case md::graphics::eGPUParam::Vector4_2:
            mCB.float4_2 = *static_cast<Vector4*>(data);
            break;
        case md::graphics::eGPUParam::Vector4_3:
            mCB.float4_3 = *static_cast<Vector4*>(data);
            break;
        case md::graphics::eGPUParam::Matrix_0:
            mCB.mat_0 = *static_cast<Matrix*>(data);
            break;
        case md::graphics::eGPUParam::Matrix_1:
            mCB.mat_1 = *static_cast<Matrix*>(data);
            break;
        case md::graphics::eGPUParam::Matrix_2:
            mCB.mat_2 = *static_cast<Matrix*>(data);
            break;
        case md::graphics::eGPUParam::Matrix_3:
            mCB.mat_3 = *static_cast<Matrix*>(data);
            break;
        default:
            break;
        }

    }

    void Material::Bind()
    {
        for (size_t i = 0; i < (UINT)eTextureSlot::End; i++)
        {
            if (mTexture[i] == nullptr)
                continue;

            UINT castedIndex = static_cast<UINT>(i);

			mTexture[i]->BindShaderResource(eShaderStage::VS, castedIndex);
			mTexture[i]->BindShaderResource(eShaderStage::HS, castedIndex);
			mTexture[i]->BindShaderResource(eShaderStage::DS, castedIndex);
			mTexture[i]->BindShaderResource(eShaderStage::GS, castedIndex);
			mTexture[i]->BindShaderResource(eShaderStage::PS, castedIndex);
			mTexture[i]->BindShaderResource(eShaderStage::CS, castedIndex);
        }

        if(mTexture[(UINT)eTextureSlot::Albedo])
            mCB.usedAlbedo = 1;

        if (mTexture[(UINT)eTextureSlot::Normal])
            mCB.usedNormal = 1;

        if (mTexture[(UINT)eTextureSlot::Specular])
            mCB.usedSpecular = 1;

        if (mbAnimaion)
            mCB.usedAnimation = 1;

        ConstantBuffer* pCB = renderer::constantBuffers[(UINT)eCBType::Material];
        pCB->SetData(&mCB);
        pCB->Bind(eShaderStage::VS);
        pCB->Bind(eShaderStage::GS);
        pCB->Bind(eShaderStage::PS);



        mShader->Binds();
    }

    void Material::Clear()
    {
        for (size_t i = 0; i < (UINT)eTextureSlot::End; i++)
        {
            if (mTexture[i] == nullptr)
                continue;

            mTexture[i]->Clear();
        }
    }
}
