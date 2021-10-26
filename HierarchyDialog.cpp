#include "HierarchyDialog.h"

HierarchyDialog* g_HierarchyDialog = 0;

bool HierarchyDialog::instantiated = false;

bool HierarchyDialog::OpenDialog()
{
	if (DialogBox(m_hInstance, MAKEINTRESOURCE(IDD_DIALOG1), 0, DlgProc))
		return true;
	//CreateDialog(m_hInstance, MAKEINTRESOURCE(IDD_DIALOG1), 0, DlgProc);

	return false;
}
bool HierarchyDialog::OpenDialog(HWND hwnd)
{
	//DialogBox(m_hInstance, MAKEINTRESOURCE(IDD_DIALOG1), hwnd, DlgProc);
	if (!IsWindow(hDlg))
	{
		hDlg = CreateDialog(m_hInstance, MAKEINTRESOURCE(IDD_DIALOG1), 0, DlgProc);
		ShowWindow(hDlg, SW_SHOW);
		return true;
	}
	
	return false;
}
HierarchyDialog::HierarchyDialog(HINSTANCE hInstance) : MsgProcedure(hInstance) 
{
	assert(!instantiated);
	instantiated = true;
	g_HierarchyDialog = this;
}
INT_PTR CALLBACK HierarchyDialog::DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_CLOSE:
		EndDialog(hDlg, 0);
		return (INT_PTR)TRUE;
	case WM_INITDIALOG:
	{
		//콤보박스에 항목 추가
		HWND comboBox1 = GetDlgItem(hDlg, IDC_COMBO1);
		HWND listBox1 = GetDlgItem(hDlg, IDC_LIST1);
		WCHAR* strMenu[] = { L"hi", L"hello", L"bonjour" };
		for (int i = 0; i < 3; i++)
		{
			//콤보박스에 추가하는 매크로 함수
			ComboBox_AddString(comboBox1, strMenu[i]);
			//리스트박스에 추가하는 매크로 함수
			ListBox_AddString(listBox1, strMenu[i]);
		}
			
		ComboBox_AddString(comboBox1, L"리터럴문자로 추가");
		ComboBox_InsertString(comboBox1, 3, L"인서트삽입");
		
		// ************ 리스트 컨트롤 *************//
		LVCOLUMN lvColumn;
		HWND h_listControl = GetDlgItem(hDlg, IDC_LIST2);
		ListView_SetExtendedListViewStyle(h_listControl,
			LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
		lvColumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_WIDTH | LVCF_TEXT;
		lvColumn.fmt = LVCFMT_CENTER;
		
		for (int i = 0; i < 3; i++)
		{
			lvColumn.cx = wcslen(strMenu[i]) * 10;
			lvColumn.pszText = strMenu[i];
			ListView_InsertColumn(h_listControl, i, &lvColumn);
		}
		
			
		
		return (INT_PTR)TRUE;
	}

	case WM_COMMAND:
		g_HierarchyDialog->MenuProc(hDlg, wParam);
		return (INT_PTR)TRUE;
	case WM_KEYDOWN:
		g_HierarchyDialog->KeyDownProc(wParam);
		return (INT_PTR)TRUE;
	case WM_MOUSEMOVE:
		g_HierarchyDialog->MouseMoveProc(wParam, lParam);
		return (INT_PTR)TRUE;
	case WM_NOTIFY: //리스트 컨트롤 클릭했을 때 발생
		g_HierarchyDialog->NotifyProc(lParam);
	}
	return (INT_PTR)FALSE;
}

void HierarchyDialog::MenuProc(HWND hDlg,WPARAM wParam)
{
	int wmId = LOWORD(wParam);
	
	WCHAR title[MAX_PATH];
	WCHAR full_path[MAX_PATH];

	

	switch (wmId)
	{
	case ID_40001: //열기
		FileOpenDialog(hDlg, title, full_path);
		MessageBox(0, L"열기", L"연습", MB_OK);
		break;
	case ID_40002: //저장
		FileSaveDialog(hDlg, full_path);
		MessageBox(0, L"저장", L"연습", MB_OK);
		break;
	case IDC_BUTTON1: //버튼1
	{
		//아이템 추가
		LVITEM lvItem;
		HWND hLV = GetDlgItem(hDlg, IDC_LIST2);
		lvItem.iItem = ListView_GetItemCount(hLV); //추가할 인덱스 위치
		lvItem.iSubItem = 0;
		lvItem.mask = LVIF_TEXT;
		lvItem.pszText = L"test Item";
		ListView_InsertItem(hLV, &lvItem);

		//서브아이템 추가
		
		ListView_SetItemText(hLV, lvItem.iItem, 0, L"0에 test Sub Item"); 
		ListView_SetItemText(hLV, lvItem.iItem, 1, L"1에 test Sub Item");
		ListView_SetItemText(hLV, lvItem.iItem, 2, L"2에 test Sub Item");
		ListView_SetItemText(hLV, lvItem.iItem, 3, L"3에 test Sub Item");
		MessageBox(hDlg, L"IDC_BUTTON1", L"dialog button", MB_OK);
		break;
	}
		
	case IDC_BUTTON2: //버튼2
		MessageBox(0, L"IDC_BUTTON2", L"dialog button", MB_OK);
		break;

	/*
		라디오 버튼 상태 조사
		UINT IsDlgButtonChecked(HWND hDlg, int nIDButton);
		리턴 코드 = BST_CHECKED , BST_INDETERMINATE, BST_UNCHECKED
		전체조사 할때 배열이용 int nRadioID[2] = {IDC_RADIO01, IDC_RADIO02}

		--------

		그룹 나누기(그룹별로 선택)
		라디오 버튼 속성에 Group 속성이 있음.
		True로 설정된 라디오 버튼부터 다음 True로 설정된 라디오 버튼까지 한 그룹으로 묶임.
		
		상태변경함수
		BOOL CheckDlgButton(HWND hDlg, int nIDButton, UINT uCheck) 버튼을 개별적으로 선택 및 비선택. 같은 그룹의 라디오버튼에서 2개를 선택 할 수도 있음.(할수있지만 안함)
		=> uCheck = BST_CHECKED / BST_UNCHECKED
		BOOL CheckRadioButton(HWND hDlg, int nIDFirstButton, int nIDLastButton, int nIDCheckButton) 그룹 안에서 하나만 선택
		오로지 선택. 기존에 선택되있다면 비선택으로 만듬. nIDCheckButton을 선택
		nIDFirstButton = 그룹의 시작 , nIDLastButton = 그룹의 마지막
	*/
	case IDC_RADIO1:
		MessageBox(0, L"IDC_RADIO1", L"dialog button", MB_OK);
		break;
	case IDC_RADIO2:
		MessageBox(0, L"IDC_RADIO2", L"dialog button", MB_OK);
		break;

	/* 체크 버튼 
		상태 조사 함수
		UINT IsDlgButtonChecked(HWND hDlg, int nIDButton);
		리턴 코드 BST_CHECKED, BST_INDETERMINATE, BST_UNCHECKED

		상태 변경함수 CheckDlgButton 
	*/
	case IDC_CHECK1:
		if (IsDlgButtonChecked(hDlg, IDC_CHECK2)==BST_CHECKED)
			MessageBox(0, L"체크2 체크됨", L"checkcheck", MB_OK);
		else
			MessageBox(0, L"체크2 체크안됨", L"checkcheck", MB_OK);
		break;
	case IDC_CHECK2:
		CheckDlgButton(hDlg, IDC_CHECK1, BST_CHECKED);
		break;


	}
}





void HierarchyDialog::KeyDownProc(WPARAM wParam)
{
	{
		//키가 눌렷는지 안눌렷는지 확인?
		//모든 키에 대해 발생하는 메시지 WM_KEYDOWN
		//WPARAM : 가상 키 코드, 대문자로만 인식
		//가상 키 코드 VK_LEFT, VK_HOME 등

		switch (wParam) //MSDN에서 VK변수 참고
		{
		case VK_LEFT:

			MessageBox(0, L"VK_LEFT", L"실험", MB_OK);
			return;
		case VK_RIGHT:
			MessageBox(0, L"VK_RIGHT", L"실험", MB_OK);
			return;
		}

		//SHORT GetAsyncKeyState(int vKey)
		//실시간으로 키 입력을 체크
		//메시지 큐에 저장되는 키 메시지의 단점을 보완. 게임에서 사용
		//키 눌림이 있으면 음수값 리턴
		//if(GetAsyncKeyState(VK_LEFT) <0) 눌린것

	}
}


void HierarchyDialog::MouseMoveProc(WPARAM btnState, LPARAM mousePos)
{
	//WM_MOUSEMOVE 마우스 이동시 발생
		//마우스 위치 정보
		//LOWORD(LPARAM) : X좌표 HIWORD(LPARAM) : Y좌표


		//마우스 버튼 코드
		//wParam으로 전달되는 코드
		//MK_LBUTTON, MK_RBUTTON <-드래그 확인가능
		//MK_MBUTTON (중간버튼), MK_SHIFT, MK_CONTROL



		//그 외 마우스 메시지
		//WM_LBUTTONDOWN 마우스 왼쪽 버튼 눌림
		//WM_LBUTTONUP 마우스 왼쪽 버튼 놓임
}



void HierarchyDialog::CharProc(HWND hDlg, WPARAM wParam)
{
}

void HierarchyDialog::NotifyProc(LPARAM lParam)
{
	//컨트롤이 선택됨
	switch (((LPNMHDR)lParam)->code)
	{
		case NM_CLICK :
			MessageBox(hDlg, L"NotifProc!", L"test", MB_OK);
			return;
	}
}








