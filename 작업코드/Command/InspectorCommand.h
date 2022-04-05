#pragma once
#include "MyCommand.h"
#include "GameObject.h"
#include "ComponentMgr.h"

class AddComponentCommand :public MyCommand
{
private:
	GameObject* gameObj;
	ComponentType componentType;
	ComponentMgr& componentMgr;
	
	Component* addedComponent;
	HWND* hInspector;
public:
	AddComponentCommand(GameObject* obj, ComponentType compType, HWND* inspector) : gameObj(obj), componentType(compType), 
		componentMgr(ComponentMgr::Instance()), hInspector(inspector) {}


public:
	// MyCommand을(를) 통해 상속됨
	virtual void Execute() override;
	virtual void Undo() override;
};


