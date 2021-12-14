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
	// MyCommand��(��) ���� ��ӵ�
	virtual void Execute() override;
	virtual void Undo() override;
};
