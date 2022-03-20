#include "DataManager.h"



DataManager::DataManager() : effectMgr(EffectMgr::Instance()) {}

DataManager & DataManager::Instance()
{
	static DataManager* instance = new DataManager();
	return *instance;
}

void DataManager::Init()
{
	for (auto& frw : FileReaderWriter::FRWs)
	{
		typeOnfrw.insert({ frw->getFileType(), frw });
	}
}

bool DataManager::LoadEffectData()
{
	if (effectMgr.CreateEffect(L"FX/Basic.fxo", EffectType::BasicEffectType) == nullptr)
		return false;
	else
		return true;
}

bool DataManager::FileOpen(HWND hwnd, WCHAR fileTitle[], WCHAR filePath[], std::vector<LPCWSTR> extension)
{
	OPENFILENAME ofn;
	
	
	//반드시 static으로 해줘야 한다고 하는데 안해도 작동중;
	WCHAR strFileTitle[MAX_PATH] = L""; //파일명
	WCHAR strFile[MAX_PATH] = L""; //파일경로까지포함
	WCHAR strFileExtension[100] = L""; //확장자

	char string[100];
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwnd;
	ofn.lpstrTitle = L"파일 선택";
	ofn.lpstrFileTitle = strFileTitle; //파일명
	ofn.lpstrFile = strFile; //전체 경로

	//확장자 필터 
	std::wstring file_extension;
	for (LPCWSTR& elem : extension)
	{
		//"(*.확장자)\0*.확장자\0"
		file_extension += L"(*.";
		file_extension += elem;
		file_extension += L")";
		file_extension += (TCHAR)'\0';
		file_extension += L"*.";
		file_extension += elem;
		file_extension += (TCHAR)'\0';
	}


	//필터 \0으로 구분 => 설명\0실제필터\0설명\0실제필터\0
	//ofn.lpstrFilter = L"(*.dds)\0*.dds\0";
	ofn.lpstrFilter = file_extension.c_str();
	ofn.nMaxFile = MAX_PATH;
	ofn.nMaxFileTitle = MAX_PATH;
	if (GetOpenFileName(&ofn) != 0)
	{
		////필터는 1부터 시작. 지금필터에서 1.bmp 2.jpg
		//switch (ofn.nFilterIndex)
		//{
		//case 1:
		//	MessageBox(hwnd, L"bmp", L"bmp파일선택", MB_OK);
		//	break;
		//case2:
		//	MessageBox(hwnd, L"jpg", L"jpg파일선택", MB_OK);
		//	break;
		//}
		wcscpy(fileTitle, strFileTitle);
		wcscpy(filePath, strFile);
		return true;
	}
	else
	{
		DWORD dw = CommDlgExtendedError();
		return false;
	}
	return true;
}


