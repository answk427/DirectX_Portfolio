#include "HierarchyCommand.h"

std::map<std::wstring, UINT> PasteObject::objectCount;

void PasteObject::Execute()
{
	//새로 복사해서 생성한 오브젝트 이름 뒤에 넘버링
	UINT cnt = ++objectCount[srcObj->GetName()];
	dstObj = &objectMgr.CreateGameObject((srcObj->GetName() + L"_" + std::to_wstring(cnt)));
}

void PasteObject::Undo()
{
}
