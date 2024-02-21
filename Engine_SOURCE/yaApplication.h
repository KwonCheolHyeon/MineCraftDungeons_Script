#pragma once
#include "yaEngine.h"
#include "yaGraphics.h"
#include "yaGraphicDevice_DX11.h"

namespace md
{
	class Physics;
	class Application
	{
	public:
		Application();
		~Application();

		virtual void Initalize();
		virtual void Update();
		virtual void FixedUpdate();
		virtual void Render();
		virtual void Destroy();

		// Running main engine loop
		void Run();
		void Present();
		void Release();


		void SetWindow(HWND hwnd, UINT width, UINT height);
		void SetHwnd(HWND hwnd) { mHwnd = hwnd; }
		HWND GetHwnd() { return mHwnd; }
		UINT GetWidth() { return mWidth; }
		UINT GetHeight() { return mHeight; }
		math::Vector2 GetSize() { return math::Vector2(static_cast<float>(mWidth), static_cast<float>(mHeight)); }
		float GetScreenRatio();

		static Physics* GetPhysics() { return mPhysics; }

	private:
		bool initialized = false;
		std::unique_ptr<graphics::GraphicDevice_DX11> graphicDevice;

		HWND mHwnd;
		HDC mHdc;
		UINT mHeight;
		UINT mWidth;

		static Physics* mPhysics;
	};
}
