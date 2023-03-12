#include "Title.h"
#include <sstream>

#include "Argent/Component/ArSpriteRenderer.h"
#include "Argent/Input/Keyboard.h"
#include "Argent/Component/ArSkinnedMeshRenderer.h"
#include "Argent/Component/NoneBoneMeshRenderer.h"
#include "Argent/Graphic/ArGraphics.h"


void Title::Initialize()
{
	AddObject(new GameObject("Demo"));
	//AddObject(new GameObject("Demo2"));
	GetGameObject("Demo")->AddComponent(new Argent::Component::Renderer::ArSpriteRenderer);
	//GetGameObject("Demo2")->AddComponent(new Argent::Component::Renderer::ArSpriteRenderer);
	//AddObject(GameObject::Cube());

	//AddObject(GameObject::Sphere());
	//AddObject(GameObject::Capsule());
	//AddObject(GameObject::SceneCamera());
	//AddObject( new GameObject("obj",new ArSkinnedMeshRenderer(Argent::Graphics::ArGraphics::Instance()->GetDevice(), "./Resource/DragonBinary.fbx")));
	//AddObject( new GameObject("obj",new Argent::Resource::FBX::ArSkinnedMeshRenderer(Argent::Graphics::ArGraphics::Instance()->GetDevice(), "./Resources/fbxkakidashi.fbx")));
	//AddObject( new GameObject("obj",new Argent::Resource::FBX::ArSkinnedMeshRenderer(Argent::Graphics::ArGraphics::Instance()->GetDevice(), "./Resources/RenderingEngine/nico.fbx")));

	AddObject(new GameObject("noneBone", new Argent::Component::Renderer::NoneBoneMeshRenderer(Argent::Graphics::ArGraphics::Instance()->GetDevice(), "./Resources/DemoGun/DemoGun.fbx", true)));


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

