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
	
	//id�� Component�� ��ġ���� ������� Component�Ŵ������� Component�� �ٽ� �޾ƿ�
	void UpdateComponent();
};


class GameObject : public Object
{
	 
public:
	GameObject() : Object(), transform(std::make_shared<Transform>(this)) { AddNodeHierarchy(); }
	GameObject(const gameObjectID& id) : Object(id), transform(std::make_shared<Transform>(this)) { AddNodeHierarchy(); }
	GameObject(const gameObjectID& id, const std::wstring& name) : Object(id, name), transform(std::make_shared<Transform>(this)) { AddNodeHierarchy(); }
	//���� ������
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
	
	//���� ������Ʈ�� �ش�Ÿ�� ������Ʈ�� �ִ��� Ȯ���ϴ� �Լ�
	bool SearchComponent(ComponentType compType);
	bool SearchComponent(Component* component);
	std::vector<ComponentOfObject>& GetComponents() { return components; }
	
	//�������� ���� �Լ���
	void AddNodeHierarchy();
	void AddBoneOffset(const XMFLOAT4X4& offsetMat);

private:
	std::vector<ComponentOfObject> components;
	
};


inline Component* GameObject::GetComponent(ComponentType compType)
{
	if (components.empty())
		return nullptr;
	
		
	//��ġ�ϴ� ������Ʈ�� �ִ��� �˻�
	std::vector<ComponentOfObject>::iterator it = std::lower_bound(
		components.begin(), components.end(),
		compType);

	if (it == components.end() || it._Ptr==NULL)
		return nullptr;
	if ((*it).GetComponentType() != compType)
		return nullptr;

	return (*it).GetComponent();
}
  