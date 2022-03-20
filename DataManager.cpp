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
	
	
	//�ݵ�� static���� ����� �Ѵٰ� �ϴµ� ���ص� �۵���;
	WCHAR strFileTitle[MAX_PATH] = L""; //���ϸ�
	WCHAR strFile[MAX_PATH] = L""; //���ϰ�α�������
	WCHAR strFileExtension[100] = L""; //Ȯ����

	char string[100];
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwnd;
	ofn.lpstrTitle = L"���� ����";
	ofn.lpstrFileTitle = strFileTitle; //���ϸ�
	ofn.lpstrFile = strFile; //��ü ���

	//Ȯ���� ���� 
	std::wstring file_extension;
	for (LPCWSTR& elem : extension)
	{
		//"(*.Ȯ����)\0*.Ȯ����\0"
		file_extension += L"(*.";
		file_extension += elem;
		file_extension += L")";
		file_extension += (TCHAR)'\0';
		file_extension += L"*.";
		file_extension += elem;
		file_extension += (TCHAR)'\0';
	}


	//���� \0���� ���� => ����\0��������\0����\0��������\0
	//ofn.lpstrFilter = L"(*.dds)\0*.dds\0";
	ofn.lpstrFilter = file_extension.c_str();
	ofn.nMaxFile = MAX_PATH;
	ofn.nMaxFileTitle = MAX_PATH;
	if (GetOpenFileName(&ofn) != 0)
	{
		////���ʹ� 1���� ����. �������Ϳ��� 1.bmp 2.jpg
		//switch (ofn.nFilterIndex)
		//{
		//case 1:
		//	MessageBox(hwnd, L"bmp", L"bmp���ϼ���", MB_OK);
		//	break;
		//case2:
		//	MessageBox(hwnd, L"jpg", L"jpg���ϼ���", MB_OK);
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


