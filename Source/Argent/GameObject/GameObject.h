#pragma once
#include <vector>
#include <string>
#include <typeinfo>
#include <memory>

#include "Component/ArComponent.h"
#include "Component/Core/Transform.h"


class GameObject
{
public:
	GameObject(std::string name = "gameObject", Argent::Component::ArComponent* c = nullptr):
		isSelected(false)
	,	name(std::move(name))
	{
		AddComponent(new Transform());
		if(c)
		{
			AddComponent(c);
		}
	}

	GameObject(std::initializer_list<Argent::Component::ArComponent*> components, std::string name = "gameObject"):
		isSelected(false)
	,	name(name)
	{
		AddComponent(new Transform());
		for(auto&& com : components)
		{
			AddComponent(com);
		}
	}

	template <typename T>
	void AddComponent()
	{
		demoCom.emplace_back(std::make_unique<T>());
	}

	void AddComponent(std::unique_ptr<Argent::Component::ArComponent> com)
	{
		demoCom.emplace_back(std::move(com));
	}

	virtual ~GameObject();
	GameObject(const GameObject&) = delete;
	GameObject(const GameObject&&) = delete;
	GameObject& operator=(const GameObject&) = delete;
	GameObject& operator=(const GameObject&&) = delete;

	virtual void Initialize();
	virtual void Finalize();
	virtual void Begin();
	virtual void End();
	virtual void Update();
	virtual void Render() const;
	virtual void DrawDebug();

	void AddComponent(Argent::Component::ArComponent* com);
	void AddChild(GameObject* obj);

	template <typename T> T* GetComponent();
	template <typename T> T* GetChild();

	Transform* GetTransform() { return GetComponent<Transform>(); }

	void SetParent(GameObject* p) { parent = p; }
	GameObject* GetParent() const { return parent; }

	const std::string& GetName() const { return name; }
	void SetName(std::string s) { name = s; }

	std::vector<GameObject*>::iterator begin() { return childObjects.begin(); }
	std::vector<GameObject*>::iterator end() { return childObjects.end(); }

	bool GetIsSelected() const { return isSelected; }
	bool* GetpIsSelected() { return &isSelected; }
	void SetIsSelected(bool b) { isSelected = b; }
	void ReverseIsSelected() { isSelected = !isSelected; }

	int GetChildCount() const { return static_cast<int>(childObjects.size()); }

	void CloseWindow() { isSelected = false; }
	void CloseAllWindow()
	{
		CloseWindow();
		for(const auto& child : childObjects)
		{
			child->CloseAllWindow();
		}
	}

	static GameObject* Cube(const std::string& name = "Cube");
	static GameObject* Sphere(const std::string& name = "Sphere");
	static GameObject* Capsule(const std::string& name = "Capsule");
	static GameObject* SceneCamera(const std::string& name = "Sub Camera", bool setSceneCamera = false);

protected:
	bool isSelected;
	std::vector<Argent::Component::ArComponent*> components;
	std::vector<std::unique_ptr<Argent::Component::ArComponent>> demoCom;
	std::vector<GameObject*> childObjects;
	std::string name;
	GameObject* parent;

};

template <typename T>
T* GameObject::GetComponent()
{
	for(auto& com : components)
	{
		if(typeid(*com) != typeid(T)) continue;

		return static_cast<T*>(com);
	}
	return nullptr;
}

template <typename T>
T* GameObject::GetChild()
{
	for(auto& obj : childObjects)
	{
		if(typeid(*obj) != typeid(T)) continue;

		return static_cast<T*>(obj);
	}
	return nullptr;
}
