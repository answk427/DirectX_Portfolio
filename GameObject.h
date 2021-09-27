#pragma once

#include "Interface.h"
#include <string>
#include <vector>
#include <algorithm>

//같은 타입인지 검사
bool getComponentCompare(const Component* comp, const type_info& compType);


class GameObject : public Object
{
public:
	GameObject(const std::wstring& name) : Object(name) {}
	//parent의 자식 오브젝트로 생성
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
	//같은타입을 이분탐색
	std::vector<Component*>::iterator it =  std::lower_bound(
		components.begin(), components.end(),
		typeid(componentType), getComponentCompare);

	if (it == components.end())
		return NULL;

	return (componentType*)*it;
}
