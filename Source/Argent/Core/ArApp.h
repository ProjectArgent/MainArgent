#pragma once
#include <memory>

#include "../Graphic/ArGraphics.h"
#include "ArWindow.h"
#include "ArTimer.h"

namespace Argent
{
	namespace App
	{
		class ArgentApp final
		{
		public:
			ArgentApp(HINSTANCE hInstance, LONG width, LONG height, const char* appName, bool isFullScreen);
			~ArgentApp() = default;
			ArgentApp(const ArgentApp&) = delete;
			ArgentApp(const ArgentApp&&) = delete;
			ArgentApp operator=(const ArgentApp&) = delete;
			ArgentApp operator=(const ArgentApp&&) = delete;


			int Execute();	//���C���֐��ł͂��ꂳ���Ăׂ΂����悤�ɂ�����

		private:

			int Terminate();
			void Initialize();

		private:
			bool isExistOtherInstance{ FALSE };	//ArgentApp��Instance�����J�E���g�@> 1�Ȃ�΃A�T�[�g
			std::unique_ptr<Argent::Graphics::ArGraphics> arGfx{};
			std::unique_ptr<Argent::Window::ArWindow> arWindow{};
		};
	}

	inline bool MainLoop(HWND hWnd)
	{
		MSG msg{};

		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}


		Argent::Timer::ArTimer::Instance().Tick();
		Argent::Timer::ArTimer::Instance().CalcFrameTime(hWnd);

		if(WM_QUIT == msg.message) return false;

		return true;
	}
}
