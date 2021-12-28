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

	//Object ����
	ObjectMgr& objectMgr = ObjectMgr::Instance();
	resultObject = dynamic_cast<Object*>(&objectMgr.CreateGameObject(basicName));
	//Hierarchy�� ����
	resultItem = hierarchy.TreeInsertObject(resultObject);
}

void CreateEmptyObject::Undo()
{
	//Object ����
	ObjectMgr& objectMgr = ObjectMgr::Instance();
	objectMgr.DeleteObject(resultObject->GetID());
	//Hierarchy���� ����
	hierarchy.TreeViewDeleteItem(resultItem);

	emptyCount--;
}
//
//DeleteObjectFromHierarchy::DeleteObjectFromHierarchy(HierarchyDialog & hierarchy) : hierarchy(hierarchy)
//{
//}
//
//void DeleteObjectFromHierarchy::Execute()
//{
//	//hierarchy���� ���� ���õ� ������ ����
//	Object* obj = hierarchy.TreeViewDeleteItem();
//	//object ����
//	ObjectMgr& objectMgr = ObjectMgr::Instance();
//	objectMgr.DeleteObject(obj->GetID());
//}
//
//void DeleteObjectFromHierarchy::Undo()
//{//����
//}
