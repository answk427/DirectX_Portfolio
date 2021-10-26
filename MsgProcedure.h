#pragma once
#include <windowsx.h>
#include <Windows.h>
#include <resource.h>
#include <string>
class MsgProcedure
{
protected:
	HINSTANCE m_hInstance;
	
public:
	virtual bool OpenDialog() = 0;
	
public:
	MsgProcedure(HINSTANCE hInstance) :m_hInstance(hInstance){}
	
	bool FileOpenDialog(HWND hwnd, WCHAR fileTitle[], WCHAR filePath[]);
	bool FileSaveDialog(HWND hwnd, WCHAR fileTitle[]);
public:	


	//메뉴눌렸을때 WM_COMMAND 
	virtual void MenuProc(HWND hDlg, WPARAM wParam) = 0;
	
	//문자 키 발생 메시지 WM_CHAR
	//대소문자 구분 방법 wParam : 아스키 코드 값
	virtual void CharProc(HWND hDlg, WPARAM wParam) = 0;
		
	virtual void KeyDownProc(WPARAM wParam) = 0;

	virtual void MouseMoveProc(WPARAM btnState, LPARAM mousePos) = 0;
	
	
};

/*Dialog
- 다이얼로그도 윈도우이다.

1. 모달형 다이얼로그
- 다이얼로그가 최우선 순위 ex)메시지박스
- 모달형 다이얼로그가 제어권을 독점함.(다른 창 선택 불가)

2. 모델리스형 다이얼로그



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


/*
ㅇ 컨트롤은 작은 윈도우 이다.
컨트롤의 윈도우 핸들을 얻는 함수
HWND GetDlgItem(HWND hDlg, int nIDDlgItem)

컨트롤 메시지 처리
- 사용자가 컨트롤에 대해 행위를 하면 
  폼이 컨트롤의 정보를WM_COMMAND 메시지 발생을 통해 전달
  - LOWORD(wParam) = 아이디 / HIWORD(wParam) = 컨트롤 상태, 통지 코드
- 통지코드(MSDN 참고)
	BN_CLICKED 버튼이 클릭된 상태 BN_DBCLICK 버튼이 더블클릭된 상태

ㅇ 컨트롤 속성
 - 기본 속성
	-Caption : 컨트롤에 표시되는 텍스트
	-ID : 컨트롤의 아이디
	-Visible : 컨트롤의 출력 여부를 결정
*/

/* 에디트 컨트롤
 - 사용자로부터 데이터 입력
 - 데이터 입력, 수정, 삭제를 위한 컨트롤
 - 다양한 속성 제공

 내용 읽기 함수
 UINT GetDlgItemText(HWND hDlg,
	int nIDDlgItem, LPTSTR lpStr,int nMaxCount)
 
 UINT GetDlgItemInt(HWND hDlg, int nIDDlgItem, 
	BOOL *lpTranslated, BOOL bSigned);
	lpTranslated는 보통 null
	bSigned가 true면 음수,양수 다 받음.
	bSigned가 false면 양수만 음수는 0으로 리턴
	부동소수점은 문자열로 받아서 변환해야함. ex) atof

 Edit Control에 데이터 설정
 BOOL SetDlgItemText(HWND hDlg, int nIDDlgItem, LPCTSTR lpString)
 BOOL SetDlgItemInt(HWND hDlg, int nIDDlgItem, UINT uValue, BOOL bSigned)

*/

/* 콤보박스
Simple, Drop Down, Drop List 형태 제공

 항목이 펼쳐지는 영역 설정(콤보박스 리스트가 너무 많을경우 조절?)
 - 리소스 뷰에서 콤보박스의 화살표 부분을 클릭하면 위아래로 크기를 조절할 수 있음.

 매크로 함수
 - Windowsx.h 포함
 - MSDN에서 combo box로 검색
 - 매크로 함수
 ComboBox_AddString() => 콤보박스에 새 항목을 추가
 ComboBox_DeleteString()
 ComboBox_InsertString()
 ComboBox_GetCurSel() => 선택된 항목의 인덱스를 반환(0부터 시작)
  선택 항목이 없으면 CB_ERR반환(-1)
*/

/* 리스트박스
 콤보박스와 유사함
 ListBox_AddString() => 리스트박스에 새 항목 추가
 int ListBox_GetCurSel(HWND hwnd) => 선택된 항목의 인덱스 반환
  선택 항목이 없으면 LB_ERR(-1) 반환
*/

/* 리스트컨트롤 (리스트박스와 다름) <- 파일탐색기 만들때 쓰면될듯?
포함 헤더 Commctrl.h , Windowsx.h

 ㅇ 리스트박스와의 차이
  - 리스트박스는 단일항목을 나열
  - 리스트컨트롤은 리스트 박스를 여러 줄 가질 수 있음.
  - 다양한 뷰 스타일 제공, 매크로 함수 제공
 ㅇ 뷰스타일
  - Icon 스타일, Small Icon 스타일, List 스타일, Report 스타일
 
 - 컬럼(상단의 항목)
 - 행을 아이템 이라고 함.
 - 열에 있는 내용을 SubItem 이라고 함.

 순서 1. 컬럼 생성
 int ListView_InsertColumn(HWND hwnd, int iCol,
	const LPLVCOLUMN pcol)
	hwnd : 윈도우 핸들 GetDlgItem() 사용
	iCol : 컬럼 인덱스
	pCol : LVCOLUMN의 메모리 주소

 ㅇ LVCOLUMN 구조체
	{UINT mask = 컬럼에 적용할 속성을 결정(.fmt, .cx, .pszText, .isublItem)
	OR연산 => 일반적으로 4개 다 사용함
	- LVCF_FMT : FMT의 컬럼과 서브 아이템 정렬
	- LVCF_WIDTH : 길이 적용
	- LVCF_TEXT : 문자열 적용
	- LVCF_SUBITEM : 서브 아이템 적용}

	{ fmt : 컬럼과 서브아이템의 데이터 정렬 형식 지정
	  정렬 방식
	  - LVCFMT_LEFT
	  - LVCFMT_RIGHT
	  - LVCFMT_CENTER
	  * 0 인덱스의 컬럼과 서브아이템은 무조건 왼쪽 정렬}
	{ cx : 가로 길이 pszText : 문자열 지정 iSubItem : 서브 아이템 인덱스}

	* 아이템 관련 매크로 함수
	int ListView_GetItemCount(HWND hwnd)
	-아이템 개수 조사

	int ListView_InsertItem(HWND hwnd, const LPLVITEM pitem)
	ㅇ LPLVITEM 구조체
	UINT mask -> LVIF_TEXT 지정
	int iItem; -> 삽입하고자 하는 인덱스
	int iSubItem; -> 0(고정)
	LPTSTR pszText; -> 문자열 설정

	서브 아이템을 넣는 함수
	int ListView_SsetItemText(HWND hwnd,
	int i, int iSubItem, LPCTSTR pszText);

	서브 아이템 데이터 가져오기
	void ListView_GetItemText(
		HWND hwnd, int iItem,
		int iSubItem,
		LPTSTR pszText,
		int cchTextMax)

	* 아이템 선택에 대한 메시지 알아내기
	발생 메시지
	- WM_NOTIFY
	- lParam
	  통지 코드(클릭인지 더블클릭인지 등),
	  컨트롤 핸들, 아이디가 저장된 메모리 주소

	* lParam 다루는 방법
	   NMHDR 구조체 사용
	   typedef struct tagNMHDR{
			HWND hwndFrom; //윈도우 핸들
			UINT_PTR idFrom; // 컨트롤 아이디
			UINT id; // 통지코드

	* 아이템 인덱스 알아내기
	int ListView_GetNextItem(
		HWND hwnd,
		int iStart, //임의의 인덱스, -1
		UINT flags // LVNI_SELECTED)

		LVNI_SELECTED -> 선택된 아이템의 인덱스를 찾겠다.
		선택이 없을 때 : -1 리턴

	* 리스트 컨트롤 뷰 설정
	void ListView_SetExtendedListViewStyle(
		HWND hwndLV, DWORD dwExStyle)
	
	dwExStyle 값
	LVS_EX_FULLROWSELECT(한행 전체 선택) |
	LVS_EX_GRIDLINEES(항목별 선택)

	아이템 수정
	ListView_GetItemText()
	ListView_SetItemText()

	아이템 삽입
	ListView_InsertItemText()
	
	아이템 삭제
	ListView_DeleteItem(HWND hwnd, int iItem)
	ListView_DeleteAllItem(HWND hwnd)
*/

/* 유용한 함수 정리


*/