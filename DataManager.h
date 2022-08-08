#pragma once
#include "FileReaderWriter.h"
#include "EffectMgr.h"
#include <map>

//���� ����, �ҷ����� Ŭ����
class DataManager
{
private:
	EffectMgr& effectMgr;
	std::map<fileTypeStr, FileReaderWriter*> typeOnfrw;
public:
	DataManager();
	~DataManager() {}

public:
	static DataManager& Instance();
	
	
public:
	void Init();

	bool LoadEffectData();
	//File�� ���, �̸��� ������ �Լ�
	bool FileOpen(HWND hwnd, WCHAR fileTitle[], WCHAR filePath[], std::vector<LPCWSTR> extension);
};
 