#include "GameObject.h"


GameObject::GameObject(const GameObject & other) : Object(other), transform(std::make_shared<Transform>(this))
{
	transform = other.transform;
	transform->m_owner_obj = this;
	components = other.components;
	nodeHierarchy = other.nodeHierarchy;
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

void GameObject::AddNodeHierarchy()
{
	std::string* parentId;
	//최상위 루트 일때
	if (parent == nullptr)
	{
		nodeHierarchy = std::make_shared<NodeHierarchy>();
		parentId = nullptr;
	}
	else
		parentId = &parent->GetID();
	 
	nodeHierarchy->AddNode(parentId, &GetID(), &GetName(),transform); 
	nodeHierarchy->m_animator->boneDatas.SetBoneTransform(GetName(), transform);

	MeshRenderer* meshRender = (MeshRenderer*)GetComponent(ComponentType::MESHRENDERER);
	SkinnedMeshRenderer* skinRender = (SkinnedMeshRenderer*)GetComponent(ComponentType::SKINNEDMESHRENDERER);
	if (meshRender != nullptr)
		meshRender->SetNodeHierarchy(nodeHierarchy);
	if (skinRender != nullptr)
		skinRender->SetNodeHierarchy(nodeHierarchy);
}

void GameObject::AddBoneOffset(const XMFLOAT4X4& offsetMat)
{
	nodeHierarchy->AddOffset(GetID(), offsetMat);
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
	//id가 일치
	if (id == component->id)
		return;

	ComponentMgr& componentMgr = ComponentMgr::Instance();
	component = componentMgr.GetComponent(id);
}

