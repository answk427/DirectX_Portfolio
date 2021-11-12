#pragma once

#include "Interface.h"
#include <string>
#include <vector>
#include <algorithm>
#include "Transform.h"

//���� Ÿ������ �˻�
bool getComponentCompare(const Component* comp, const type_info& compType);


class GameObject : public Object
{

public:
	GameObject() : Object() {}
	GameObject(const gameObjectID& id) : Object(id) {}
	GameObject(const gameObjectID& id, const std::wstring& name) : Object(id, name) {}
			
	Transform transform;

public:
	template <typename componentType>
	Component* GetComponent();
	void AddComponent(Component* component);
	
public:
	
private:
	std::vector<Component*> components;
};


template<typename componentType>
inline Component* GameObject::GetComponent()
{
	//�����̳ʿ� �ִ��� Ȯ��
	if (!std::binary_search(components.begin(),
		components.end(), typeid(componentType),
		getComponentCompare))
	{
		return NULL;
	}
		
	//����Ÿ���� �̺�Ž��
	std::vector<Component*>::iterator it =  std::lower_bound(
		components.begin(), components.end(),
		typeid(componentType), getComponentCompare);

	return it;
}
