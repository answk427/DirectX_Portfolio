#pragma once
#include <string>
#include <vector>

typedef std::string gameObjectID;

//오브젝트에 추가될 컴포넌트의 인터페이스
class Component
{
public:
	std::string id;

public:
	Component(const std::string& id) : id(id) {}
	virtual ~Component() {}
public:
	virtual void Init() = 0;
	virtual void FixedUpdate() = 0;
	virtual void Update() = 0;
	virtual void LateUpdate() = 0;
	
};

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
	
	virtual ~Object() {}

public:
	std::wstring GetName() { return name; }
	void SetName(std::wstring& name) { this->name = name; }
	void SetParent(Object* parent) { this->parent = parent; }
	void SetChild(Object* child) { childs.push_back(child); }
};