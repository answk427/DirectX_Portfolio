#include "GameObject.h"


GameObject::GameObject(const GameObject & other) : Object(other), transform(this)
{
	transform = other.transform;
	transform.m_owner_obj = this;
	components = other.components;
}

GameObject::~GameObject()
{
	ComponentMgr& componentMgr = ComponentMgr::Instance();
	for (auto& elem : components)
	{
		componentMgr.OnOffComponent(elem.GetComponent(), Command::DISABLE);
	}

}

bool GameObject::AddComponent(Component * component)
{		 
	bool test = SearchComponent(component);

	ComponentOfObject* componentOfObject = new ComponentOfObject(component->id, component);
	components.push_back(*componentOfObject);

	std::sort(components.begin(), components.end());

	return true;
}

bool GameObject::DeleteComponent(componentID& id)
{
	for (auto it = components.begin(); it != components.end(); ++it)
	{
		if ((*it).GetId() == id)
		{
			components.erase(it);
			return true;
		}
	}
	return false;
}

bool GameObject::SearchComponent(ComponentType compType)
{
	return std::binary_search(components.begin(), components.end(), compType);
}

bool GameObject::SearchComponent(Component * component)
{
	return std::binary_search(components.begin(), components.end(), component->componentType);
	
}

bool operator<(const ComponentOfObject & comp, const ComponentType & other)
{
	return comp.componentType < other;
}

bool operator<(const ComponentType & other, const ComponentOfObject & comp)
{
	return  other< comp.componentType;
}



bool ComponentOfObject::operator<(const ComponentOfObject & other)
{
	if (componentType == other.componentType)
		return id < other.id;
	
	return componentType < other.componentType;
}

bool ComponentOfObject::operator<(const ComponentOfObject * other)
{
	if (componentType == other->componentType)
		return id < other->id;

	return componentType < other->componentType;
}

//bool ComponentOfObject::operator<(const ComponentType & other)
//{
//	return componentType < other;
//}



Component * ComponentOfObject::GetComponent()
{
	UpdateComponent();
	return component;
}


void ComponentOfObject::UpdateComponent()
{
	//id°¡ ÀÏÄ¡
	if (id == component->id)
		return;

	ComponentMgr& componentMgr = ComponentMgr::Instance();
	component = componentMgr.GetComponent(id);
}

