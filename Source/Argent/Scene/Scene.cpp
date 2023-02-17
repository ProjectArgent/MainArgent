#include "Scene.h"
#include "../Graphic/ArGraphics.h"
#include "../GameObject/Component/Core/Camera.h"

void Scene::Initialize()
{
	Camera* c{};
	Light* l{};
	for(const auto& object : gameObject)
	{
		object->Initialize();
		if(!c)
		{
			c = object->GetComponent<Camera>();
			if(!c)
			{
				if(object->GetChild<GameObject>())
					c = object->GetChild<GameObject>()->GetComponent<Camera>();
			}
		}
		if(!l)
		{
			l = object->GetComponent<Light>();
			if(!l)
			{
				if(object->GetChild<GameObject>())
					l = object->GetChild<GameObject>()->GetComponent<Light>();
			}
		}
	}
	Argent::Graphics::ArGraphics::Instance()->SetCamera(c);
	Argent::Graphics::ArGraphics::Instance()->SetLight(l);
}

void Scene::Finalize()
{
	for(const auto& object : gameObject)
	{
		object->Finalize();
		delete object;
	}
	gameObject.clear();
}

void Scene::Begin()
{
	for(const auto& object : gameObject)
	{
		
	}
}

void Scene::End()
{
	for (const auto& object : gameObject)
	{

	}
}

void Scene::Update()
{
	for(const auto& object : gameObject)
	{
		object->Update();
	}
}

void Scene::Render()
{
	for(const auto& object : gameObject)
	{
		object->Render();
	}
}

void Scene::DrawDebug()
{
	if(ImGui::TreeNode("Object"))
	{
		for(const auto & object : gameObject)
		{
			ImGuiCheckBox(object);
		}
		ImGui::TreePop();
	}
	for(const auto& object : gameObject)
	{
		object->DrawDebug();
	}
}

void Scene::CloseAllDebugWindow() const
{
	for(const auto& obj : gameObject)
	{
		obj->CloseAllWindow();
	}
}

void Scene::ImGuiCheckBox(GameObject* obj)
{
#ifdef _DEBUG

	if(ImGui::Button(obj->GetName().c_str()))
	{
		CloseAllDebugWindow();
		obj->SetIsSelected(true);
	}

	if(obj->GetChildCount() == 0) return;
	if(ImGui::TreeNode(obj->GetName().c_str()))
	{
		for(const auto& child : *obj)
		{
			ImGuiCheckBox(child);
		}
		ImGui::TreePop();		
	}
#endif
}
