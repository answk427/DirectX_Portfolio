#pragma once

#include "Interface.h"
#include <string>
#include <vector>
#include <algorithm>
#include "Transform.h"
#include "ComponentMgr.h"

class ComponentOfObject
{
private:
	componentID id;
	ComponentType componentType;
	Component* component;
public:
	ComponentOfObject(componentID id, Component* component) : id(id), component(component) { componentType = component->componentType; }

public:
	bool operator<(const ComponentOfObject& other);
	bool operator<(const ComponentOfObject* other);
	friend bool operator<(const ComponentOfObject& comp, const ComponentType& other);
	friend bool operator<(const ComponentType& other, const ComponentOfObject& comp);
	
public:
	Component* GetComponent();
	gameObjectID GetId() const {return id;}
	ComponentType GetComponentType() const{ return component->componentType; }
	
	//id와 Component가 일치하지 않을경우 Component매니저에서 Component를 다시 받아옴
	void UpdateComponent();
};


class GameObject : public Object
{
	 
public:
	GameObject() : Object() {}
	GameObject(const gameObjectID& id) : Object(id) {}
	GameObject(const gameObjectID& id, const std::wstring& name) : Object(id, name) {}
	~GameObject();
			
	Transform transform;

public:
	template <typename componentT>
	componentT* GetComponent();
	bool AddComponent(Component* component);
	bool DeleteComponent(componentID& id);
	
	//현재 오브젝트에 해당타입 컴포넌트가 있는지 확인하는 함수
	bool SearchComponent(ComponentType compType);
	bool SearchComponent(Component* component);
	std::vector<ComponentOfObject>& GetComponents() { return components; }

private:
	std::vector<ComponentOfObject> components;
	
};


template<typename componentT>
inline componentT* GameObject::GetComponent()
{
	componentT temp("temp");
	
	//일치하는 컴포넌트가 있는지 검색
	std::vector<ComponentOfObject>::iterator it = std::lower_bound(
		components.begin(), components.end(),
		temp.componentType);

	if (it == components.end() || it._Ptr==NULL)
		return nullptr;
	if ((*it).GetComponentType() != temp.componentType)
		return nullptr;

	return dynamic_cast<componentT*>((*it).GetComponent());
}
  