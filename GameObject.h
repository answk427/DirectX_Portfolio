#pragma once

#include "Interface.h"
#include <string>
#include <vector>
#include <algorithm>

//���� Ÿ������ �˻�
bool getComponentCompare(const Component* comp, const type_info& compType);


class GameObject : public Object
{
public:
	GameObject(const std::wstring& name) : Object(name) {}
	//parent�� �ڽ� ������Ʈ�� ����
	GameObject(const std::wstring& name, Object* parent) : Object(name, parent) {}
public:
	template <typename componentType>
	componentType* GetComponent();
	
	void AddComponent(Component* component);
	
private:
	std::vector<Component*> components;
};


template<typename componentType>
inline componentType * GameObject::GetComponent()
{
	//����Ÿ���� �̺�Ž��
	std::vector<Component*>::iterator it =  std::lower_bound(
		components.begin(), components.end(),
		typeid(componentType), getComponentCompare);

	if (it == components.end())
		return NULL;

	return (componentType*)*it;
}
