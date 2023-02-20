#include "Title.h"
#include <sstream>

#include "Argent/GameObject/Component/Renderer/ArSpriteRenderer.h"
#include "Argent/Input/Keyboard.h"


#include "Argent/JustChanging/ArSkinnedMeshRenderer.h"
#include "Argent/Graphic/ArGraphics.h"


void Title::Initialize()
{
	AddObject(new GameObject("Demo"));
	AddObject(new GameObject("Demo2"));
	GetGameObject("Demo")->AddComponent(new Argent::Component::Renderer::ArSpriteRenderer);
	GetGameObject("Demo2")->AddComponent(new Argent::Component::Renderer::ArSpriteRenderer);
	AddObject(GameObject::Cube());
	//AddObject(GameObject::SceneCamera());
	AddObject( new GameObject("obj",new ArSkinnedMeshRenderer(Argent::Graphics::ArGraphics::Instance()->GetDevice(), "./Resource/RenderingEngine/plantune.fbx")));

	//GetGameObject("Main Camera")->GetComponent<Camera>()->SetTarget(GetGameObject("Player")->GetTransform());
	//GetGameObject("Player")->AddComponent<Transform>();
	Scene::Initialize();
}

void Title::Finalize()
{
	Scene::Finalize();
}

void Title::Update()
{
	Scene::Update();

	if(Argent::Input::Mouse::Instance().IsButtonPress(Argent::Input::Mouse::Mouses::mLeftButton))
	{
		//SceneManager::SetNextScene("Game");
	}
}

void Title::Render()
{
	Scene::Render();
}

void Title::DrawDebug()
{
#ifdef _DEBUG
	Scene::DrawDebug();
#endif
}

