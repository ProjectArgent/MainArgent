#include "SceneManager.h"

//todo ‰½‚Æ‚©‚·‚é‚±‚Æ
#include "../../Title.h"
#include "../../Game.h"
#include "../Input/Keyboard.h"

namespace Argent::SceneManagement
{
		
	std::string ArSceneManager::nextScene;
		
	ArSceneManager::ArSceneManager():
		currentScene(nullptr)
	{
		std::unique_ptr<Scene> s = std::make_unique<Title>("Title");
		scenes[s->GetName()] = std::move(s);
		
		s = std::make_unique<Game>("Game");
		scenes[s->GetName()] = std::move(s);
		
	}
		
	void ArSceneManager::Execute()
	{
		ChangeScene();
		
		if(Input::Keyboard::Instance().IsKeyPressEnter(Input::Keyboard::A))
		{
			if(currentScene->GetName() != "Game")
			{
				SetNextScene("Game");
			}
		}
		
		if(currentScene)
		{
			currentScene->Update();
			currentScene->Render();
			currentScene->DrawDebug();
		}
	}
		
	void ArSceneManager::Initialize()
	{
		nextScene = "Title";
	}
		
	void ArSceneManager::Finalize()
	{
		if(currentScene)
			currentScene->Finalize();
	}
		
	void ArSceneManager::ChangeScene()
	{
		if(!nextScene.empty())
		{
			if(currentScene)
				currentScene->Finalize();
		
			currentScene = scenes[nextScene].get();
			nextScene.clear();
		
			if(currentScene)
				currentScene->Initialize();
		}
	}
}
