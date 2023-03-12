#pragma once
#include <memory>

#include "../Graphic/ArGraphics.h"
#include "ArWindow.h"
#include "ArTimer.h"
#include "../EffectRenderer.h"
#include "../Resource/ArEffectManager.h"
namespace Argent
{
	namespace App
	{
		/**
		 * \brief アプリケーションクラス　こいつのExecuteを呼び出すだけで動く
		 */
		class ArgentApp final
		{
		public:
			ArgentApp(HINSTANCE hInstance, LONG width, LONG height, const char* appName, bool isFullScreen);
			~ArgentApp() = default;
			ArgentApp(const ArgentApp&) = delete;
			ArgentApp(const ArgentApp&&) = delete;
			ArgentApp operator=(const ArgentApp&) = delete;
			ArgentApp operator=(const ArgentApp&&) = delete;

			/**
			 * \brief　実行関数
			 * \return 終了コード
			 */
			int Execute() const;

		private:

			int Terminate() const;
			void Initialize() const;

		private:
			bool isExistOtherInstance{ FALSE };	//ArgentAppのInstance数をカウント　> 1ならばアサート
			std::unique_ptr<Argent::Graphics::ArGraphics> arGfx{};
			std::unique_ptr<Argent::Window::ArWindow> arWindow{};
			//std::unique_ptr< Argent::Effect::EffectRenderer > effectRenderer{};
			std::unique_ptr<Argent::Resource::Effect::ArEffectManager> effectManager{};
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
