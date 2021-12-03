#pragma once
#include "EffectMgr.h"

//���� ����, �ҷ����� Ŭ����
class DataManager
{
private:
	EffectMgr& effectMgr;
public:
	DataManager():effectMgr(EffectMgr::Instance()){}
	~DataManager() {}

public:
	static DataManager& Instance();
	

public:
	bool LoadEffectData();

	//File�� ���, �̸��� ������ �Լ�
	bool FileOpen(HWND hwnd, WCHAR fileTitle[], WCHAR filePath[], std::vector<LPCWSTR> extension);
};
