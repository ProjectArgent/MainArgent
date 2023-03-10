#include "GameObject.h"
#include <sstream>
#include "../Graphic/ArGraphics.h"
#include "../Component/Camera.h"
#include "../Component/ArMeshRenderer.h"
#include "../Resource/ArResourceManager.h"


void GameObject::AddComponent(Argent::Component::ArComponent* com)
{
	com->SetOwner(this);
	components.emplace_back(com);
}

void GameObject::AddChild(GameObject* obj)
{
	obj->SetParent(this);
	childObjects.emplace_back(obj);
}

GameObject* GameObject::Cube(const std::string& name)
{
	return new GameObject({new Argent::Component::Renderer::ArMeshRenderer(Argent::Resource::ArResourceManager::Instance().GetMeshData("Cube")) }, name);
}

GameObject* GameObject::Sphere(const std::string& name)
{
	return new GameObject({ new Argent::Component::Renderer::ArMeshRenderer(Argent::Resource::ArResourceManager::Instance().GetMeshData("Sphere")) }, name);
}

GameObject* GameObject::Capsule(const std::string& name)
{
	return new GameObject({ new Argent::Component::Renderer::ArMeshRenderer(Argent::Resource::ArResourceManager::Instance().GetMeshData("Capsule")) }, name);
}

GameObject* GameObject::SceneCamera(const std::string& name, bool setSceneCamera)
{
	return new GameObject({
			new Camera(setSceneCamera, Argent::Graphics::ArGraphics::Instance()->GetWidth(), Argent::Graphics::ArGraphics::Instance()->GetHeight()),
			new CameraController
		},
		name);
}

GameObject::~GameObject()
{

}

void GameObject::Initialize()
{
	for(const auto& com : components)
	{
		com->Initialize();
	}
	for(const auto& obj : childObjects)
	{
		obj->Initialize();
	}
}

void GameObject::Finalize()
{
	for(const auto& com : components)
	{
		com->Finalize();
		delete com;
	}
	components.clear();
	for(const auto& obj : childObjects)
	{
		obj->Finalize();
		delete obj;
	}
	childObjects.clear();
}

void GameObject::Begin()
{
	for (const auto& com : components)
	{
		com->End();
	}
	for (const auto& obj : childObjects)
	{
		obj->Begin();
	}
}

void GameObject::End()
{
	for (const auto& com : components)
	{
		com->End();
	}
	for (const auto& obj : childObjects)
	{
		obj->Begin();
	}
}

void GameObject::Update()
{
	for(const auto& com : components)
	{
		com->Update();
	}
	for(const auto& obj : childObjects)
	{
		obj->Update();
	}
}

void GameObject::Render() const
{
	for(const auto& com : components)
	{
		com->Render();
	}
	for(const auto& obj : childObjects)
	{
		obj->Render();
	}
}

void GameObject::DrawDebug() 
{
#ifdef _DEBUG

	if(isSelected)
	{
		ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_::ImGuiCond_Once);
		ImGui::SetNextWindowPos(ImVec2( 900, 50), ImGuiCond_::ImGuiCond_Once);
		ImGui::Begin(name.c_str(),nullptr, ImGuiWindowFlags_MenuBar);
		if (ImGui::BeginMenuBar()) {
		    if (ImGui::BeginMenu("Menu"))
		    {
		        if (ImGui::MenuItem("Reset"))
				{
					Initialize();
		        }

				if(ImGui::MenuItem("Close"))
				{
					SetIsSelected(false);
				}
		        ImGui::EndMenu();
		    }
		    ImGui::EndMenuBar();
		}
		if(parent)
		{
			std::ostringstream oss;
			oss << "Parent Name : " << parent-> GetName();
			//ImGui::BulletText(oss.str().c_str());
			if(ImGui::Button(oss.str().c_str()))
			{
				parent->SetIsSelected(true);
				CloseWindow();
			}
		}
		for(const auto& com : components)
		{
			com->DrawDebug();
		}

		//AddComponent
		{
		//static bool addComponent;
		//if(ImGui::Button("Add CubeComponent", ImVec2(200, 30)))
		//{
		//	addComponent = true;
		//	//AddComponent(new Cube);
		//}

		//if(addComponent)
		//{
		//	ImGui::SetNextWindowPos(ImVec2( ImGui::GetWindowPos().x,
		//		ImGui::GetWindowPos().y + ImGui::GetWindowSize().y / 2.0f), ImGuiCond_::ImGuiCond_Once);
		//	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_::ImGuiCond_Once);
		//	ImGui::Begin("Component");

		//	if(ImGui::Button("Cube"))
		//	{
		//		AddComponent(new Cube);
		//		addComponent = false;
		//	}
		//	if(ImGui::Button("Sphere"))
		//	{
		//		AddComponent(new Sphere);
		//		addComponent = false;
		//	}

		//	ImGui::End();
		//}

		}
		ImGui::End();
	}
	
	for(const auto& obj : childObjects)
	{
		obj->DrawDebug();
	}

#endif
	
}


//if(isOpenNewDebugWindow)

	//if (ImGui::BeginMenuBar()) {
 //       if (ImGui::BeginMenu("File"))
 //       {
 //           if (ImGui::MenuItem("Save")) {

 //           }
 //           if (ImGui::MenuItem("Load")) {

 //           }

 //           ImGui::EndMenu();
 //       }
 //       ImGui::EndMenuBar();
	//}

	//ImGui::BeginChild(ImGui::GetID(static_cast<void*>(0)), ImVec2(250, 100), ImGuiWindowFlags_NoTitleBar);
	//ImGui::Text("Child");
 //   ImGui::EndChild();

	//if(!isOpenNewDebugWindow)
	//{
	//	if(ImGui::TreeNode(name.c_str()))
	//	{
	//		for(const auto& com : components)
	//		{
	//			com->DrawDebug();
	//		}

	//		if(!childObjects.empty())
	//		{
	//			if(ImGui::TreeNode("ChildObject"))
	//			{
	//				for(const auto& obj : childObjects)
	//				{
	//					obj->DrawDebug();
	//				}
	//				ImGui::TreePop();
	//			}
	//		}
	//		ImGui::TreePop();
	//	}
	//}

