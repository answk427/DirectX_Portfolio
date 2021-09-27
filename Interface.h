#pragma once
#include <string>


//������Ʈ�� �߰��� ������Ʈ�� �������̽�
class Component
{
public:
	std::string id;

public:
	virtual void Init() = 0;
	virtual void FixedUpdate() = 0;
	virtual void Update() = 0;
	virtual void LateUpdate() = 0;
	virtual ~Component() {}
};

class Object
{
public:
	Object(const std::wstring& name) : name(name), parent(0), child(0) {}
	//�ڽ� ������Ʈ�� ������ �� ���
	Object(const std::wstring& name, Object* parent) : name(name), parent(parent) 
	{
		parent->child = this;
	}
	virtual ~Object() {}
private:
	std::wstring name;
	Object* parent;
	Object* child;
};