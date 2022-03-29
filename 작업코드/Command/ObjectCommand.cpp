#include "ObjectCommand.h"

int CreateEmptyObject::emptyCount = -1;

CreateEmptyObject::CreateEmptyObject(HierarchyDialog & hierarchy) : hierarchy(hierarchy), resultObject(0)
{
}

void CreateEmptyObject::Execute()
{
	std::wstring basicName(L"EmptyObject");
	emptyCount++;
	if (emptyCount != 0)
		basicName += std::to_wstring(emptyCount);

	//Object 생성
	ObjectMgr& objectMgr = ObjectMgr::Instance();
	resultObject = dynamic_cast<Object*>(&objectMgr.CreateGameObject(basicName));
	//Hierarchy에 삽입
	resultItem = hierarchy.TreeInsertObject(resultObject);
}

void CreateEmptyObject::Undo()
{
	//Object 제거
	ObjectMgr& objectMgr = ObjectMgr::Instance();
	objectMgr.DeleteObject(resultObject->GetID());
	//Hierarchy에서 제거
	hierarchy.TreeViewDeleteItem(resultItem);

	emptyCount--;
}
