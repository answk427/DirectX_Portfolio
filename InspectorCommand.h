#pragma once
#include "MyCommand.h"
#include "GameObject.h"
class AddComponentCommand :public MyCommand
{
private:
	GameObject* gameObj;
	ComponentType componentType;
public:
	AddComponentCommand(GameObject* obj, ComponentType compType) : gameObj(obj), componentType(compType) {}


public:
	// MyCommand을(를) 통해 상속됨
	virtual void Execute() override;
	virtual void Undo() override;
};
