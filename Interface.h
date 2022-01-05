#pragma once
#include <d3dUtil.h>
#include <string>
#include <vector>



enum ComponentType
{
	MESHRENDERER,
	SKINNEDMESHRENDERER,
	LIGHT,
	UNDEFINED
};

typedef std::string componentID;

//������Ʈ�� �߰��� ������Ʈ�� �������̽�
class Component
{
public:
	std::string id;
	ComponentType componentType;

public:
	Component(const componentID& id, ComponentType type) : id(id), componentType(type) {}
	virtual ~Component() 
	{
	}
public:
	virtual void Init() = 0;
	virtual void FixedUpdate() = 0;
	virtual void Update() = 0;
	virtual void LateUpdate() = 0;
	virtual void Enable() = 0;
	virtual void Disable() = 0;
	
};

typedef std::string gameObjectID;


class Object
{
private:
	gameObjectID id;
	std::wstring name;
public:
	Object* parent;
	std::vector<Object*> childs;
	
public:
	Object() :id(""), name(L"") {}
	Object(const gameObjectID& id) : id(id), name(L"Object"), parent(0) {}
	Object(const gameObjectID& id, const std::wstring& name) : id(id), name(name), parent(0) {}
	
	virtual ~Object() {	}

public:
	std::wstring GetName() { return name; }
	gameObjectID& GetID() { return id; }
	void SetName(std::wstring& name) { this->name = name; }
	void SetParent(Object* parent) { this->parent = parent; }
	void SetChild(Object* child) { childs.push_back(child); }
	void DeleteChild(Object* child)
	{
		std::vector<Object*>::iterator it = childs.begin();
		for (it; it != childs.end(); ++it)
		{
			//���� ���̵����� �˻� �� ����
			if ( ((*it)->id) == child->id)
			{
				childs.erase(it);
				break;
			}
				
		}
	}
};