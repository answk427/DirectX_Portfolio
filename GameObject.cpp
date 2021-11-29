#include "GameObject.h"


void GameObject::AddComponent(Component * component)
{	
	bool search = std::binary_search(components.begin(), components.end(), 
		component, GameObjectComp);
	
	//중복된 컴포넌트는 추가하지 않음
	if (search)
		return;
	
	components.push_back(component);
	std::sort(components.begin(), components.end(), GameObjectComp);
}




bool getComponentCompare_binary(const type_info & compType, const Component* comp)
{	
	return typeid(*comp).raw_name()<compType.raw_name();
}

bool getComponentCompare_lowerBound(const Component* comp, const char* rawName)
{
	return typeid(*comp).raw_name() < rawName;
}

bool GameObjectComp(const Component * comA, const Component * comB)
{
	return typeid(*comA).raw_name() < typeid(*comB).raw_name();
}