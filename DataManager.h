#pragma once
#include "FileReaderWriter.h"
#include "EffectMgr.h"
#include <map>

//파일 저장, 불러오기 클래스
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
	//File의 경로, 이름을 얻어오는 함수
	bool FileOpen(HWND hwnd, WCHAR fileTitle[], WCHAR filePath[], std::vector<LPCWSTR> extension);
};
 