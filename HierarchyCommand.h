#pragma once
#include "MyCommand.h"
#include "ObjectMgr.h"
#include "ComponentMgr.h"

class PasteObject :public MyCommand
{
private:
	static std::map<std::wstring, UINT> objectCount;
	GameObject* dstObj; //���� ���� ������Ʈ�� �ּҸ� ������ ����
	GameObject* srcObj; //������ ������Ʈ
	ObjectMgr& objectMgr;
		
public:
	PasteObject(GameObject* src,GameObject* dest) : srcObj(src), dstObj(dest),
		objectMgr(ObjectMgr::Instance()){}


public:
	// MyCommand��(��) ���� ��ӵ�
	virtual void Execute() override;
	virtual void Undo() override;
};



