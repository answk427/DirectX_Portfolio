#pragma once
#include "MyCommand.h"
#include "ObjectMgr.h"
#include "ComponentMgr.h"

class PasteObject :public MyCommand
{
private:
	static std::map<std::wstring, UINT> objectCount;
	GameObject* dstObj; //새로 만든 오브젝트의 주소를 저장할 변수
	GameObject* srcObj; //복사할 오브젝트
	ObjectMgr& objectMgr;
		
public:
	PasteObject(GameObject* src,GameObject* dest) : srcObj(src), dstObj(dest),
		objectMgr(ObjectMgr::Instance()){}


public:
	// MyCommand을(를) 통해 상속됨
	virtual void Execute() override;
	virtual void Undo() override;
};



