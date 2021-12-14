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

	//��ġ�ϴ� ������Ʈ�� �ִ��� �˻�
	std::vector<ComponentOfObject*>::iterator it = std::lower_bound(
		components.begin(), components.end(),
		component->componentType, lowerBoundCompare);
	
	/*bool search = std::binary_search(components.begin(), components.end(), 
		component->componentType, lowerBoundCompare);*/
	
	//�ߺ��� ������Ʈ�� �߰����� ����
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
	//id�� ��ġ
	if (id == component->id)
		return;

	ComponentMgr componentMgr = ComponentMgr::Instance();
	component = componentMgr.GetComponent(id);
}
