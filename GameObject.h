#pragma once

#include "Interface.h"
#include <string>
#include <vector>
#include <algorithm>
#include "Transform.h"

//같은 타입인지 검사
bool getComponentCompare_binary(const type_info& compType, const Component* comp);
bool getComponentCompare_lowerBound(const Component* comp, const char* rawName);

//type 이름순 정렬
bool GameObjectComp(const Component* comA, const Component* comB);


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
	const std::vector<Component*>& GetComponents() { return components; }

private:
	std::vector<Component*> components;
	
};


template<typename componentType>
inline Component* GameObject::GetComponent()
{
	////컨테이너에 있는지 확인
	//if (!std::binary_search(components.begin(),
	//	components.end(), typeid(componentType),
	//	getComponentCompare_lowerBound))
	//{
	//	return NULL;
	//}
	const char* str = typeid(componentType).raw_name();

	//같은타입을 이분탐색
	std::vector<Component*>::iterator it =  std::lower_bound(
		components.begin(), components.end(),
		&componentType("test"), GameObjectComp);

	//bool(const Component *, const Component *): 인수2을(를) 'const_Ty'에서 const Component*(으)로 변환할 수 없습니다.
	//std::vector<Component*>::iterator it = std::lower_bound(
	//	components.begin(), components.end(),
	//	typeid(componentType).raw_name(), GameObjectComp);

	if (it == components.end())
		return nullptr;

	return *it;
}
