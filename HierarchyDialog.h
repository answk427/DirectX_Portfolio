#pragma once
#include "stdafx.h"
#include <CommCtrl.h>
#include <resource.h>
#include <cassert>
#include "MsgProcedure.h"
#include "GameObject.h"

#pragma comment(lib,"comctl32.lib")


class HierarchyDialog : public MsgProcedure
{
public:	
	static bool instantiated;

	int m_nOpen; //오픈이미지 인덱스
	int m_Closed; //클로즈이미지 인덱스

	HWND m_hDlg; //dialog
	HWND m_hwndTV; //tree view
	HWND hWnd; //main window
	
	
	HierarchyDialog(HINSTANCE hInstance);
	
	static INT_PTR CALLBACK DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
	
	// MsgProcedure을(를) 통해 상속됨
	virtual bool OpenDialog() override;
	bool OpenDialog(HWND hwnd);
	void Init(HWND hWnd_main);
	void treeInit(HWND hDlg);
	

public:
	//TreeView 함수
	HTREEITEM TreeViewInsertItem(LPWSTR itemName, HTREEITEM parent = TVI_ROOT);
	void TreeInsertObject(Object* obj, HTREEITEM parent = TVI_ROOT);

	//메뉴눌렸을때 WM_COMMAND 
	virtual void MenuProc(HWND hDlg, WPARAM wParam) override;
	
	virtual void KeyDownProc(WPARAM wParam) override;
	
	virtual void MouseMoveProc(WPARAM btnState, LPARAM mousePos) override;
		   
	virtual void CharProc(HWND hDlg, WPARAM wParam) override;

	

	bool TreeImageSet();

	void NotifyProc(LPARAM lParam);
	
	void testFunc();
	

	
};

/*Dialog
- 다이얼로그도 윈도우이다.

1. 모달형 다이얼로그
- 다이얼로그가 최우선 순위 ex)메시지박스
- 모달형 다이얼로그가 제어권을 독점함.(다른 창 선택 불가)
- DialogBox()로 생성 EndDialog()로 해제

2. 모델리스형 다이얼로그
- CreateDialog()로 생성
- DestroyWindow()로 해제


ㅇ 다이얼로그 생성함수
- DialogBox(HINSTANCE hInstance,
	LPCTSTR lpTemplate, <- Form을 연결
	HWND hWndParent, <- 메뉴에 의해서 Dialog가 떳으면 메뉴를 가진 윈도우가 부모
	DLGPROC lpDialogFunc); <- 프로시져

- makeintresource() -> lpTemplate에 지정
- WndProc에서 처리되지 않은 메시지는 DefWindowProc가 처리
	- DialogProc는 TRUE 또는 FALSE를 반환
- SetWindowText() 로 다이얼로그의 타이틀 변경

**********************
파일열기 모달형 다이얼로그
#include <commdlg.h>
BOOL GetOpenFileName(LOPENFILENAME lpofn);
- OPENFILENAME 멤버 변수
 - lpstrFilter
   파일형식 설명 ex) "비트맵(*.bmp)\0*.bmp\0
 - nFilterIndex
	선택한 파일 인덱스, 1부터 시작
 - lpstrFileTitle
	파일명을 저장할 배열 설정
	nMaxFileTitle에 길이 지정
	static 변수 또는 전역 변수 설정(필수, 로컬로하면 에러)
 - lpstrFile
	'전체 경로'를 저장할 배열 설정
	nMaxFile에 길이 지정
	static 변수 또는 전역 변수 설정
 - lpstrTitle
	다이얼로그 타이틀 문자열 출력

*/


/* 다이얼로그 프로시저의 형태

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam)); //2번째 인수는 다이얼로그부터 리턴값을 받고 싶을때 넣음. 보통 0으로 사용
			return (INT_PTR)TRUE;
		}
		break;
	case WM_CLOSE:
		EndDialog(hDlg, 0);
		return (INT_PTR)TRUE;
	
	}
	return (INT_PTR)FALSE;
}
*/