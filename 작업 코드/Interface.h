#pragma once
#include <string>
#include <vector>


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
	Object* parent;
	std::vector<Object*> childs;
public:
	std::wstring name;
public:
	Object(const std::wstring& name) : name(name), parent(0) {}
	//자식 오브젝트로 생성할 때 사용
	Object(const std::wstring& name, Object* parent) : name(name), parent(parent)
	{
		parent->childs.push_back(this);
	}
	virtual ~Object() {}

};