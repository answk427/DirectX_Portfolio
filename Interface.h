#pragma once
#include <vector>

//오브젝트에 추가될 컴포넌트의 인터페이스
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