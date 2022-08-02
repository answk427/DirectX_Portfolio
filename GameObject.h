#pragma once

#include "Interface.h"
#include <string>
#include <vector>
#include <algorithm>
#include "Transform.h"
#include "ComponentMgr.h"
#include "NodeBoneDatas.h"
#include <memory>

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
	GameObject() : Object(), transform(std::make_shared<Transform>(this)) { AddNodeHierarchy(); }
	GameObject(const gameObjectID& id) : Object(id), transform(std::make_shared<Transform>(this)) { AddNodeHierarchy(); }
	GameObject(const gameObjectID& id, const std::wstring& name) : Object(id, name), transform(std::make_shared<Transform>(this)) { AddNodeHierarchy(); }
	//복사 생성자
	GameObject(const GameObject& other);
	~GameObject();
			
	std::shared_ptr<Transform> transform;
	std::shared_ptr<NodeHierarchy> nodeHierarchy;
	
	virtual void SetParent(GameObject* parent) 
	{
		Object::SetParent(parent); 
		nodeHierarchy = parent->nodeHierarchy;
		AddNodeHierarchy();
	}
public:
	Component* GetComponent(ComponentType compType); 
	bool AddComponent(Component* component);
	bool DeleteComponent(componentID& id);
	
	//현재 오브젝트에 해당타입 컴포넌트가 있는지 확인하는 함수
	bool SearchComponent(ComponentType compType);
	bool SearchComponent(Component* component);
	std::vector<ComponentOfObject>& GetComponents() { return components; }
	
	//계층구조 관련 함수들
	void AddNodeHierarchy();
	void AddBoneOffset(const XMFLOAT4X4& offsetMat);

private:
	std::vector<ComponentOfObject> components;
	
};


inline Component* GameObject::GetComponent(ComponentType compType)
{
	if (components.empty())
		return nullptr;
	
		
	//일치하는 컴포넌트가 있는지 검색
	std::vector<ComponentOfObject>::iterator it = std::lower_bound(
		components.begin(), components.end(),
		compType);

	if (it == components.end() || it._Ptr==NULL)
		return nullptr;
	if ((*it).GetComponentType() != compType)
		return nullptr;

	return (*it).GetComponent();
}
  