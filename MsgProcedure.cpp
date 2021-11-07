#include "MsgProcedure.h"



bool MsgProcedure::FileOpenDialog(HWND hwnd, WCHAR fileTitle[], WCHAR filePath[])
{
	OPENFILENAME ofn;

	//�ݵ�� static���� ����� �Ѵٰ� �ϴµ� ���ص� �۵���;
	WCHAR strFileTitle[MAX_PATH]=L""; //���ϸ�
	WCHAR strFile[MAX_PATH]=L""; //���ϰ�α�������
	WCHAR strFileExtension[100]=L""; //Ȯ����

	char string[100];
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwnd;
	ofn.lpstrTitle = L"���� ����";
	ofn.lpstrFileTitle = strFileTitle; //���ϸ�
	ofn.lpstrFile = strFile; //��ü ���
	
	//���� \0���� ���� => ����\0��������\0����\0��������\0
	ofn.lpstrFilter = L"��Ʈ��(*.bmp)\0*.bmp\0jpg ����\0*.jpg\0";
	ofn.nMaxFile = MAX_PATH;
	ofn.nMaxFileTitle = MAX_PATH;
	if (GetOpenFileName(&ofn)!=0)
	{
		//���ʹ� 1���� ����. �������Ϳ��� 1.bmp 2.jpg
		switch (ofn.nFilterIndex)
		{
		case 1:
			MessageBox(hwnd, L"bmp", L"bmp���ϼ���", MB_OK);
			break;
		case2:
			MessageBox(hwnd, L"jpg", L"jpg���ϼ���", MB_OK);
			break;
		}
		wcscpy(fileTitle, strFileTitle);
		wcscpy(filePath, strFile);
		return true;
	}
	else
	{
		DWORD dw = CommDlgExtendedError();
		return false;
	}
	return false;
}

bool MsgProcedure::FileSaveDialog(HWND hwnd, WCHAR filePath[])
{
	/*�������� ����� ���̾�α�
	- lpstrFilter ����
	  ������ ������ ������ ����
	- ������ ���� Ȯ������ ������ �ľ��Ͽ� Ȯ���� �߰�
	*/
	OPENFILENAME ofn;

	WCHAR strFileTitle[MAX_PATH] = L""; //���ϸ�
	WCHAR strFile[MAX_PATH] = L""; //���ϰ�α�������
	WCHAR strFileExtension[100] = L""; //Ȯ����

	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwnd;
	ofn.lpstrTitle = L"���� ����";
	ofn.lpstrFileTitle = strFileTitle; //���ϸ�
	ofn.lpstrFile = strFile; //��ü ���

	//���� \0���� ���� => ����\0��������\0����\0��������\0
	ofn.lpstrFilter = L"��Ʈ��(*.bmp)\0*.bmp\0jpg ����\0*.jpg\0";
	ofn.nMaxFile = MAX_PATH;
	ofn.nMaxFileTitle = MAX_PATH;

	//���� �����ϴ� �ڵ�� �������� ��.
	//GetSaveFileName�� ���� ���,  ���ϸ� ���� �� ����.
	if (GetSaveFileName(&ofn) != 0)
	{
		switch (ofn.nFilterIndex)
		{
		case 1: //bmp����
			//Ȯ���ڰ� �Էµ��� ���� ���(Ȯ���� ���� 0)
			if (ofn.nFileExtension == 0)
			{
				wcscat(strFileTitle, L".bmp");
				wcscat(strFile, L".bmp");
				wcscpy(filePath, strFile);
			}
			break;
		case 2: //jpg ����
			//Ȯ���ڰ� �Էµ��� ���� ���(Ȯ���� ���� 0)
			if (ofn.nFileExtension == 0)
			{
				wcscat(strFileTitle, L".jpg");
				wcscat(strFile, L".jpg");
			}
			break;
		}
		return true;
			
	}

	return false;
}


