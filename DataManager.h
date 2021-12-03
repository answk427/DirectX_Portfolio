#pragma once
#include "EffectMgr.h"

//파일 저장, 불러오기 클래스
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

	//File의 경로, 이름을 얻어오는 함수
	bool FileOpen(HWND hwnd, WCHAR fileTitle[], WCHAR filePath[], std::vector<LPCWSTR> extension);
};
