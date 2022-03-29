#pragma once

#include "GameObject.h"
#include "HierarchyDialog.h"
#include "ObjectMgr.h"

class CreateEmptyObject : public MyCommand
{
private:
	static int emptyCount;
		
	HierarchyDialog& hierarchy;
	Object* resultObject;
	HTREEITEM resultItem;

public:
	CreateEmptyObject(HierarchyDialog& hierarchy);

	// MyCommand을(를) 통해 상속됨
	virtual void Execute() override;
	virtual void Undo() override;
};