#pragma once
#include <string>
#include <vector>


//������Ʈ�� �߰��� ������Ʈ�� �������̽�
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
	//�ڽ� ������Ʈ�� ������ �� ���
	Object(const std::wstring& name, Object* parent) : name(name), parent(parent)
	{
		parent->childs.push_back(this);
	}
	virtual ~Object() {}

};