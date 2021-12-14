#include "GameObject.h"



bool GameObjectComp(const ComponentOfObject * comA, const ComponentOfObject * comB)
{
	int typeA = comA->GetComponentType();
	int typeB = comB->GetComponentType();
	if (typeA == typeB)
	{
		return comA->GetId() < comB->GetId();
	}

	return typeA < typeB;

	//return comA->GetComponentType() < comB->GetComponentType();
}


bool lowerBoundCompare(const ComponentOfObject * comA, ComponentType comB)
{
	return comA->GetComponentType() < comB;
}



bool GameObject::AddComponent(Component * component)
{	
	//componentT temp("temp");

	//일치하는 컴포넌트가 있는지 검색
	std::vector<ComponentOfObject*>::iterator it = std::lower_bound(
		components.begin(), components.end(),
		component->componentType, lowerBoundCompare);
	
	/*bool search = std::binary_search(components.begin(), components.end(), 
		component->componentType, lowerBoundCompare);*/
	
	//중복된 컴포넌트는 추가하지 않음
	/*if (search)
		return false;*/
	if (it != components.end())
		return false;
	
	ComponentOfObject* componentOfObject = new ComponentOfObject(component->id, component);
	components.push_back(componentOfObject);

	std::sort(components.begin(), components.end(), GameObjectComp);

	return true;
}


Component * ComponentOfObject::GetComponent() 
{
	UpdateComponent();
	return component;
}


void ComponentOfObject::UpdateComponent()
{
	//id가 일치
	if (id == component->id)
		return;

	ComponentMgr componentMgr = ComponentMgr::Instance();
	component = componentMgr.GetComponent(id);
}
