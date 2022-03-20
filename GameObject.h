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
	
	//id�� Component�� ��ġ���� ������� Component�Ŵ������� Component�� �ٽ� �޾ƿ�
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
	
	//���� ������Ʈ�� �ش�Ÿ�� ������Ʈ�� �ִ��� Ȯ���ϴ� �Լ�
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
	
	//��ġ�ϴ� ������Ʈ�� �ִ��� �˻�
	std::vector<ComponentOfObject>::iterator it = std::lower_bound(
		components.begin(), components.end(),
		temp.componentType);

	if (it == components.end() || it._Ptr==NULL)
		return nullptr;
	if ((*it).GetComponentType() != temp.componentType)
		return nullptr;

	return dynamic_cast<componentT*>((*it).GetComponent());
}
  