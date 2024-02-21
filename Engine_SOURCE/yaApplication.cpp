#include "yaApplication.h"
#include "yaRenderer.h"
#include "yaTime.h"
#include "yaInput.h"
#include "yaSceneManager.h"
#include "yaResources.h"
#include "yaCollisionManager.h"
#include "yaFmod.h"
#include "yaFontWrapper.h"
#include "yaFbxLoader.h"
#include "yaPhysics.h"
#include "mdDetourManager.h"

namespace md
{
	using namespace graphics;

	Physics* Application::mPhysics{};

	Application::Application()
	{

	}

	Application::~Application()
	{
		
	}

	void Application::Initalize()
	{
		mPhysics = new Physics;
		mPhysics->Initialize();
		Time::Initialize();
		Input::Initialize();
		//FbxLoader::Initialize();
		Fmod::Initialize();
		FontWrapper::Initialize();

		CollisionManager::Init();
		renderer::Initialize();
		SceneManager::Initalize();
		DetourManager::Initalize();
	}

	// 게임 로직 캐릭터 이동 등등 
	// CPU UPDATE
	void Application::Update()
	{
		// 시간 매니저의 업데이트가 처리된다.
		Time::Update();
		// 입력 관련 처리
		Input::Update();
		//CollisionManager::Update();
		// 씬 업데이트 처리
		SceneManager::Update();
	}

	// GPU update
	void Application::FixedUpdate()
	{
		//CollisionManager::FixedUpdate();
		// 씬 매니저 업데이트
		SceneManager::FixedUpdate();
		// 피직스 업데이트 처리
		mPhysics->Update();
	}

	void Application::Render()
	{
		// 타임 매니저에서 렌더
		Time::Render(mHdc);

		//graphicDevice->Clear();
		// 뷰포트 관련 조정
		graphicDevice->AdjustViewPorts();
		// 렌더러의 렌더 타겟들을 정리
		renderer::ClearRenderTargets();
		// 렌더러의 렌더 호출
		renderer::Render();
	}

	void Application::Destroy()
	{

	}

	// Running main engine loop
	void Application::Run()
	{
		Update();
		FixedUpdate();
		Render();

		// 현재는 수행하는 일이 없다.
		Destroy();
	}

	void Application::Present()
	{
		graphicDevice->Present();
	}

	void Application::Release()
	{
		delete mPhysics;
		Resources::deleteTest();
		FbxLoader::Release();
		Fmod::Release();
		FontWrapper::Release();
		DetourManager::Release();
	}

	void Application::SetWindow(HWND hwnd, UINT width, UINT height)
	{
		if (graphicDevice == nullptr)
		{
			mHwnd = hwnd;
			mHdc = GetDC(mHwnd);
			mWidth = width;
			mHeight = height;

			ValidationMode vaildationMode = ValidationMode::Disabled;
			graphicDevice = std::make_unique<GraphicDevice_DX11>();
		}

		RECT rt = { 0, 0, (LONG)width , (LONG)height };
		AdjustWindowRect(&rt, WS_OVERLAPPEDWINDOW, false);
		SetWindowPos(mHwnd, nullptr, 0, 0, rt.right - rt.left, rt.bottom - rt.top, 0);
		ShowWindow(mHwnd, true);
		UpdateWindow(mHwnd);
	}

	float Application::GetScreenRatio()
	{
		if (mWidth == 0)
		{
			return 1.f;
		}

		return static_cast<float>(mHeight) / static_cast<float>(mWidth);
	}
}
