#pragma once
#include <unordered_map>
#include <memory>

#include "Scene.h"

namespace Argent::SceneManagement
{
	class ArSceneManager
	{
	public:
		ArSceneManager();
		~ArSceneManager() = default;
		
		void Execute();
		void Initialize();
		void Finalize();
		
		
		static void SetNextScene(const std::string& s) { nextScene = s; }
		
		
	private:
		void ChangeScene();
		
	private:
		static std::string nextScene;
		Scene* currentScene;
		std::unordered_map<std::string, std::unique_ptr<Scene>> scenes;

	};
}
