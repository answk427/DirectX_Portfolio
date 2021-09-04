#pragma once
#include <vector>

//������Ʈ�� �߰��� ������Ʈ�� �������̽�
class Component
{
public:
	virtual void Init() = 0;
	virtual void FixedUpdate() = 0;
	virtual void Update() = 0;
	virtual void LateUpdate() = 0;
	virtual ~Component() {}
};

class Object
{
private:
	
	
};