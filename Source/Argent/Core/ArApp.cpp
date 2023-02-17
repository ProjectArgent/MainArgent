#include "ArApp.h"
#include "../../../imgui/ImguiCtrl.h"
#include "../Other/ResourceManager.h"
#include "../Scene/SceneManager.h"

namespace Argent::App
{
	ArgentApp::ArgentApp(HINSTANCE hInstance, LONG width, LONG height, const char* appName, bool isFullScreen)
	{
		if(isExistOtherInstance) _ASSERT_EXPR(FALSE, L"Instance is already existed");
		isExistOtherInstance = true;

		arWindow = std::make_unique<Argent::Window::ArWindow>(hInstance, width, height);
		arGfx = std::make_unique<Argent::Graphics::ArGraphics>(arWindow->GetHandle());
	}

	void ArgentApp::Initialize()
	{
		arGfx->Initialize();
		ImguiCtrl::Initialize(arWindow->GetHandle(), arGfx->GetDevice(), arGfx->GetGUIHeap());
		ResourceManager::Instance().Initialize();
	}

	int ArgentApp::Execute()
	{
		Initialize();
			
		SceneManagement::ArSceneManager arSceneManager;
		arSceneManager.Initialize();
		while (MainLoop(arWindow->GetHandle()))
		{
			arGfx->Begin();

			ImguiCtrl::Begin("Main Window");

			arSceneManager.Execute();
			arGfx->DrawDebug();

			ImguiCtrl::End(arGfx->GetCommandList(), arGfx->GetGUIHeap());

			arGfx->End();
		}
		arSceneManager.Finalize();
		ImguiCtrl::Terminate();
		return Terminate();
	}

	int ArgentApp::Terminate()
	{
		arGfx->Terminate();
		return 0;
	}
}
