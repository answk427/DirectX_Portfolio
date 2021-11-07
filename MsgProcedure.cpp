#include "MsgProcedure.h"



bool MsgProcedure::FileOpenDialog(HWND hwnd, WCHAR fileTitle[], WCHAR filePath[])
{
	OPENFILENAME ofn;

	//반드시 static으로 해줘야 한다고 하는데 안해도 작동중;
	WCHAR strFileTitle[MAX_PATH]=L""; //파일명
	WCHAR strFile[MAX_PATH]=L""; //파일경로까지포함
	WCHAR strFileExtension[100]=L""; //확장자

	char string[100];
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwnd;
	ofn.lpstrTitle = L"파일 선택";
	ofn.lpstrFileTitle = strFileTitle; //파일명
	ofn.lpstrFile = strFile; //전체 경로
	
	//필터 \0으로 구분 => 설명\0실제필터\0설명\0실제필터\0
	ofn.lpstrFilter = L"비트맵(*.bmp)\0*.bmp\0jpg 파일\0*.jpg\0";
	ofn.nMaxFile = MAX_PATH;
	ofn.nMaxFileTitle = MAX_PATH;
	if (GetOpenFileName(&ofn)!=0)
	{
		//필터는 1부터 시작. 지금필터에서 1.bmp 2.jpg
		switch (ofn.nFilterIndex)
		{
		case 1:
			MessageBox(hwnd, L"bmp", L"bmp파일선택", MB_OK);
			break;
		case2:
			MessageBox(hwnd, L"jpg", L"jpg파일선택", MB_OK);
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
	/*파일저장 모달형 다이얼로그
	- lpstrFilter 역할
	  저장할 파일의 형식을 구분
	- 저장할 때는 확장자의 유무를 파악하여 확장자 추가
	*/
	OPENFILENAME ofn;

	WCHAR strFileTitle[MAX_PATH] = L""; //파일명
	WCHAR strFile[MAX_PATH] = L""; //파일경로까지포함
	WCHAR strFileExtension[100] = L""; //확장자

	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwnd;
	ofn.lpstrTitle = L"파일 선택";
	ofn.lpstrFileTitle = strFileTitle; //파일명
	ofn.lpstrFile = strFile; //전체 경로

	//필터 \0으로 구분 => 설명\0실제필터\0설명\0실제필터\0
	ofn.lpstrFilter = L"비트맵(*.bmp)\0*.bmp\0jpg 파일\0*.jpg\0";
	ofn.nMaxFile = MAX_PATH;
	ofn.nMaxFileTitle = MAX_PATH;

	//실제 저장하는 코드는 만들어줘야 함.
	//GetSaveFileName을 통해 경로,  파일명 얻어올 수 있음.
	if (GetSaveFileName(&ofn) != 0)
	{
		switch (ofn.nFilterIndex)
		{
		case 1: //bmp파일
			//확장자가 입력되지 않은 경우(확장자 길이 0)
			if (ofn.nFileExtension == 0)
			{
				wcscat(strFileTitle, L".bmp");
				wcscat(strFile, L".bmp");
				wcscpy(filePath, strFile);
			}
			break;
		case 2: //jpg 파일
			//확장자가 입력되지 않은 경우(확장자 길이 0)
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


