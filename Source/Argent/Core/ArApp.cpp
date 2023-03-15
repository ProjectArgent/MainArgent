#include "ArApp.h"
#include "../../../imgui/ImguiCtrl.h"
#include "../Resource/ArResourceManager.h"
#include "../Scene/SceneManager.h"
#include "../Input/Keyboard.h"

namespace Argent::App
{
	ArgentApp::ArgentApp(HINSTANCE hInstance, LONG width, LONG height, const char* appName, bool isFullScreen)
	{
		if(isExistOtherInstance) _ASSERT_EXPR(FALSE, L"Instance is already existed");
		isExistOtherInstance = true;

		arWindow = std::make_unique<Window::ArWindow>(hInstance, width, height);
		arGfx = std::make_unique<Graphics::ArGraphics>(arWindow->GetHandle());
		//effectRenderer = std::make_unique<Argent::Effect::EffectRenderer>();
		effectManager = std::make_unique<Argent::Resource::Effect::ArEffectManager>(arGfx->GetDevice(), arGfx->GetCommandQueue(), arGfx->GetNumBackBuffers());
	}

	void ArgentApp::Initialize() const
	{
		arGfx->Initialize();
		ImguiCtrl::Initialize(arWindow->GetHandle(), arGfx->GetDevice(), arGfx->GetGUIHeap());
		Resource::ArResourceManager::Instance().Initialize();
	}

	int ArgentApp::Execute() const
	{
		Initialize();
			
		SceneManagement::ArSceneManager arSceneManager;
		arSceneManager.Initialize();
		while (MainLoop(arWindow->GetHandle()))
		{
			Argent::Input::Keyboard::Instance().Update();
			arGfx->Begin();

			ImguiCtrl::Begin("Main Window");

			effectManager->Update();
			arSceneManager.Execute();
			effectManager->Render();

			ImguiCtrl::End(arGfx->GetCommandList(), arGfx->GetGUIHeap());

			arGfx->End();
		}
		arSceneManager.Finalize();
		ImguiCtrl::Terminate();
		return Terminate();
	}

	int ArgentApp::Terminate() const
	{
		arGfx->Terminate();
		return 0;
	}
}
