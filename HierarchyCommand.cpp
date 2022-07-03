#include "HierarchyCommand.h"

std::map<std::wstring, UINT> PasteObject::objectCount;

void PasteObject::Execute()
{
	//���� �����ؼ� ������ ������Ʈ �̸� �ڿ� �ѹ���
	UINT cnt = ++objectCount[srcObj->GetName()];
	dstObj = &objectMgr.CreateGameObject((srcObj->GetName() + L"_" + std::to_wstring(cnt)));
}

void PasteObject::Undo()
{
}
