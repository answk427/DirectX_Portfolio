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
	gameObjectID id;
	ComponentType componentType;
	Component* component;
public:
	ComponentOfObject(gameObjectID id, Component* component) : id(id), component(component) { componentType = component->componentType; }

public:
	bool operator<(const ComponentOfObject& other);
public:
	Component* GetComponent();
	gameObjectID GetId() const {return id;}
	ComponentType GetComponentType() const{ return component->componentType; }
	
	//id와 Component가 일치하지 않을경우 Component매니저에서 Component를 다시 받아옴
	void UpdateComponent();
};

//type순, id순 정렬
bool GameObjectComp(const ComponentOfObject* comA,const ComponentOfObject* comB);
bool lowerBoundCompare(const ComponentOfObject* comA, ComponentType comB);


class GameObject : public Object
{
	 
public:
	GameObject() : Object() {}
	GameObject(const gameObjectID& id) : Object(id) {}
	GameObject(const gameObjectID& id, const std::wstring& name) : Object(id, name) {}
			
	Transform transform;

public:
	template <typename componentT>
	componentT* GetComponent();
	bool AddComponent(Component* component);
	std::vector<ComponentOfObject*>& GetComponents() { return components; }

private:
	std::vector<ComponentOfObject*> components;
	
};


template<typename componentT>
inline componentT* GameObject::GetComponent()
{
	componentT temp("temp");
		
	//일치하는 컴포넌트가 있는지 검색
	std::vector<ComponentOfObject*>::iterator it = std::lower_bound(
		components.begin(), components.end(),
		temp.componentType, lowerBoundCompare);

	if (it == components.end())
		return nullptr;

	return dynamic_cast<componentT*>((*it)->GetComponent());
}
