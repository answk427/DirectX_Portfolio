#include "GameObject.h"

//type �̸��� ����
bool comp(const Component* comA, const Component* comB)
{
	return typeid(*comA).raw_name() < typeid(*comB).raw_name();
}

void GameObject::AddComponent(Component * component)
{	
	bool search = std::binary_search(components.begin(), components.end(), 
		component, comp);
	
	//�ߺ��� ������Ʈ�� �߰����� ����
	if (search)
		return;
	
	components.push_back(component);
	std::sort(components.begin(), components.end(), comp);
}

bool getComponentCompare(const Component * comp, const type_info & compType)
{
	std::string a = typeid(*comp).name();
	std::string b = compType.name();
	bool what = typeid(*comp) == compType;
	//return typeid(*comp) == compType;
	return typeid(*comp).raw_name()<compType.raw_name();
}
