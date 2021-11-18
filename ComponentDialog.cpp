#include "ComponentDialog.h"

ComponentDialog* g_ComponentDialog = 0;


bool ComponentDialog::instantiated = false;

//-------------------*ChildProc Test *-----------------------
VOID WINAPI OnChildDialogInit(HWND hwndDlg)
{
	//부모 윈도우의 위치,크기
	HWND hwndParent = GetParent(hwndDlg);
	RECT parentWndRect;
	GetWindowRect(hwndParent, &parentWndRect);
	
	/*DLGHDR *pHdr = (DLGHDR *)GetWindowLongPtr(
		hwndParent, GWLP_USERDATA);*/
	float parentWidth = parentWndRect.right - parentWndRect.left;
	float parentHeight = (parentWndRect.bottom - parentWndRect.top);
	SetWindowPos(hwndDlg, HWND_TOP, parentWndRect.left,
		parentWndRect.top + parentHeight*0.03,//-2,
		parentWidth,
		parentHeight*0.9,
		SWP_SHOWWINDOW);
	
	return;
}

INT_PTR CALLBACK ChildProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_CLOSE:
		EndDialog(hDlg, 0);
		return (INT_PTR)TRUE;
	case WM_INITDIALOG:
	{
		OnChildDialogInit(hDlg);
		

		return (INT_PTR)TRUE;
	}

	case WM_COMMAND:
		
		return (INT_PTR)TRUE;
	case WM_KEYDOWN:
		
		return (INT_PTR)TRUE;
	case WM_MOUSEMOVE:
		
		return (INT_PTR)TRUE;
	case WM_NOTIFY: //리스트 컨트롤 클릭했을 때 발생
		
		return (INT_PTR)TRUE;
	case WM_MOVE: //
	{
		
		return (INT_PTR)TRUE;
	}

	}
	return (INT_PTR)FALSE;
}
//-------------------*ChildProc Test *-----------------------



bool ComponentDialog::OpenDialog()
{
	if (!IsWindow(m_hDlg))
	{
		m_hDlg = CreateDialog(0, MAKEINTRESOURCE(COMPONENTDIALOG), 0, ComponentDialog::DlgProc);

		ShowWindow(m_hDlg, SW_SHOW);
				
		return true;
	}

	return false;
}
bool ComponentDialog::OpenDialog(HWND hwnd)
{
	
	hWnd = hwnd;
	if (!IsWindow(m_hDlg))
	{
		m_hDlg = CreateDialog(m_hInstance, MAKEINTRESOURCE(COMPONENTDIALOG), 0, ComponentDialog::DlgProc);

		ShowWindow(m_hDlg, SW_SHOW);
		return true;
	}

	return false;
}

void ComponentDialog::Init(HWND hWnd_main)
{
	hWnd = hWnd_main;

	InitCommonControls();
}

ComponentDialog::ComponentDialog(HINSTANCE hInstance) : MsgProcedure(hInstance), m_pHdr(0)
{
	assert(!instantiated);
	instantiated = true;
	g_ComponentDialog = this;
}

INT_PTR CALLBACK ComponentDialog::DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_CLOSE:
		if (g_ComponentDialog->m_pHdr != nullptr)
			LocalFree(g_ComponentDialog->m_pHdr);
		EndDialog(hDlg, 0);
		return (INT_PTR)TRUE;
	case WM_INITDIALOG:
	{
		g_ComponentDialog->WindowSizing(hDlg);
		g_ComponentDialog->OnTabbedDialogInit(hDlg);

		return (INT_PTR)TRUE;
	}

	case WM_COMMAND:
		g_ComponentDialog->MenuProc(hDlg, wParam);
		return (INT_PTR)TRUE;
	case WM_KEYDOWN:
		g_ComponentDialog->KeyDownProc(wParam);
		return (INT_PTR)TRUE;
	case WM_MOUSEMOVE:
		g_ComponentDialog->MouseMoveProc(wParam, lParam);
		return (INT_PTR)TRUE;
	case WM_NOTIFY: //리스트 컨트롤 클릭했을 때 발생
		g_ComponentDialog->NotifyProc(hDlg,lParam);
		return (INT_PTR)TRUE;
	case WM_MOVE: //
	{
		
		return (INT_PTR)TRUE;
	}

	}
	return (INT_PTR)FALSE;
}

void ComponentDialog::MenuProc(HWND hDlg, WPARAM wParam)
{
	int wmId = LOWORD(wParam);

}





void ComponentDialog::KeyDownProc(WPARAM wParam)
{
	
}


void ComponentDialog::MouseMoveProc(WPARAM btnState, LPARAM mousePos)
{
	
}



void ComponentDialog::CharProc(HWND hDlg, WPARAM wParam)
{

}

void ComponentDialog::NotifyProc(HWND hDlg,LPARAM lParam)
{
	UINT codeTest = ((LPNMHDR)lParam)->code;
	switch (((LPNMHDR)lParam)->code)
	{
		
	//탭 컨트롤이 선택됨
	case TCN_SELCHANGE:
		return OnSelChanged(hDlg);
				
	}
}

void ComponentDialog::WindowSizing(HWND hwndDlg)
{
	//컴퓨터 전체스크린의 크기
	int width = GetSystemMetrics(SM_CXSCREEN);
	int height = (GetSystemMetrics(SM_CYSCREEN) - 30); //30은 작업표시줄 크기


	RECT mainWndRect;
	GetWindowRect(hWnd, &mainWndRect);

	//dialog 위치, 크기 조정
	MoveWindow(hwndDlg, width * MAINWINDOWWIDTHRATE + width * HIERARCHYWIDTHRATE,
		mainWndRect.top,
		width * COMPONENTWIDTHRATE,
		height * COMPONENTHEIGHTRATE, true);

	//tab control 위치, 크기 조정
	RECT dlgWndRect;
	GetWindowRect(hwndDlg, &dlgWndRect);
	HWND hwndTab = GetDlgItem(hwndDlg, IDC_TAB1);
	MoveWindow(hwndTab, 0,
		0,
		dlgWndRect.right - dlgWndRect.left,
		(dlgWndRect.bottom - dlgWndRect.top) * 0.8, true);

	HWND button = GetDlgItem(hwndDlg, IDC_BUTTON1);
	MoveWindow(button, 180, 690, 100, 30, true);

	HWND combo =GetDlgItem(hwndDlg, IDC_COMBO2);
	MoveWindow(combo , 70, 695, 100, 35,true);

}



HRESULT ComponentDialog::OnTabbedDialogInit(HWND hwndDlg)
{
	INITCOMMONCONTROLSEX iccex;

	/*DWORD dwDlgBase = GetDialogBaseUnits();
	int cxMargin = LOWORD(dwDlgBase) / 4;
	int cyMargin = HIWORD(dwDlgBase) / 8;*/

	TCITEM tie;
	RECT rcTab;
	HWND hwndButton;
	RECT rcButton;
	int i;

	// Initialize common controls.
	iccex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	iccex.dwICC = ICC_TAB_CLASSES;
	InitCommonControlsEx(&iccex);

	// Allocate memory for the DLGHDR structure. Remember to 
	// free this memory before the dialog box is destroyed.
	m_pHdr = (DLGHDR *)LocalAlloc(LPTR, sizeof(DLGHDR));
	
	// Save a pointer to the DLGHDR structure in the window
	// data of the dialog box. 
	SetWindowLongPtr(hwndDlg, GWLP_USERDATA, (LONG_PTR)m_pHdr);

	// Create the tab control. Note that m_hInstance is a global 
	// instance handle. 
	/*m_pHdr->hwndTab = CreateWindow(
		WC_TABCONTROL, L"",
		WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
		0, 0, 100, 100,
		hwndDlg, NULL, m_hInstance, NULL
	);*/
	m_pHdr->hwndTab = GetDlgItem(hwndDlg, TABCONTROLID);

	if (m_pHdr->hwndTab == NULL)
	{
		return HRESULT_FROM_WIN32(GetLastError());
	}

	// Add a tab for each of the three child dialog boxes. 
	tie.mask = TCIF_TEXT | TCIF_IMAGE;
	tie.iImage = -1;
	tie.pszText = L"Transform";
	TabCtrl_InsertItem(m_pHdr->hwndTab, 0, &tie);
	tie.pszText = L"MeshRenderer";
	TabCtrl_InsertItem(m_pHdr->hwndTab, 1, &tie);
	tie.pszText = L"Terrain";
	TabCtrl_InsertItem(m_pHdr->hwndTab, 2, &tie);

	// Lock the resources for the three child dialog boxes. 
	m_pHdr->apRes[0] = DoLockDlgRes(MAKEINTRESOURCE(TAB_TRANSFORM));
	m_pHdr->apRes[1] = DoLockDlgRes(MAKEINTRESOURCE(TAB_MESHRENDERER));
	m_pHdr->apRes[2] = DoLockDlgRes(MAKEINTRESOURCE(TAB_TERRAIN));

	// Determine a bounding rectangle that is large enough to 
	// contain the largest child dialog box. 
	//SetRectEmpty(&rcTab);
	//for (i = 0; i < C_PAGES; i++)
	//{
	//	if (m_pHdr->apRes[i]->cx > rcTab.right)
	//		rcTab.right = m_pHdr->apRes[i]->cx;
	//	if (m_pHdr->apRes[i]->cy > rcTab.bottom)
	//		rcTab.bottom = m_pHdr->apRes[i]->cy;
	//}

	//// Map the rectangle from dialog box units to pixels.
	//MapDialogRect(hwndDlg, &rcTab);

	// Calculate how large to make the tab control, so 
	// the display area can accommodate all the child dialog boxes. 
	TabCtrl_AdjustRect(m_pHdr->hwndTab, TRUE, &rcTab);
	GetClientRect(m_pHdr->hwndTab, &rcTab);
	
	//OffsetRect(&rcTab, cxMargin - rcTab.left, cyMargin - rcTab.top);

	// Calculate the display rectangle. 
	CopyRect(&m_pHdr->rcDisplay, &rcTab);
	TabCtrl_AdjustRect(m_pHdr->hwndTab, FALSE, &m_pHdr->rcDisplay);

	// Set the size and position of the tab control, buttons, 
	// and dialog box. 
	/*SetWindowPos(m_pHdr->hwndTab, NULL, rcTab.left, rcTab.top,
		rcTab.right - rcTab.left, rcTab.bottom - rcTab.top,
		SWP_NOZORDER);*/

	// Move the first button below the tab control. 
	//hwndButton = GetDlgItem(hwndDlg, IDC_BUTTON1);
	//SetWindowPos(hwndButton, NULL,
	//	rcTab.left, rcTab.bottom + cyMargin, 0, 0,
	//	SWP_NOSIZE | SWP_NOZORDER);

	//// Determine the size of the button. 
	//GetWindowRect(hwndButton, &rcButton);
	//rcButton.right -= rcButton.left;
	//rcButton.bottom -= rcButton.top;

	//// Move the second button to the right of the first. 
	//hwndButton = GetDlgItem(hwndDlg, IDC_BUTTON1);
	//SetWindowPos(hwndButton, NULL,
	//	rcTab.left + rcButton.right + cxMargin,
	//	rcTab.bottom + cyMargin, 0, 0,
	//	SWP_NOSIZE | SWP_NOZORDER);

	//// Size the dialog box. 
	//SetWindowPos(hwndDlg, NULL, 0, 0,
	//	rcTab.right + cyMargin + (2 * GetSystemMetrics(SM_CXDLGFRAME)),
	//	rcTab.bottom + rcButton.bottom + (2 * cyMargin)
	//	+ (2 * GetSystemMetrics(SM_CYDLGFRAME))
	//	+ GetSystemMetrics(SM_CYCAPTION),
	//	SWP_NOMOVE | SWP_NOZORDER);

	// Simulate selection of the first item. 
	OnSelChanged(hwndDlg);

	return S_OK;
}

DLGTEMPLATEEX * ComponentDialog::DoLockDlgRes(LPCTSTR lpszResName)
{
	HRSRC hrsrc = FindResource(NULL, lpszResName, RT_DIALOG);

	// Note that m_hInstance is the global instance handle
	HGLOBAL hglb = LoadResource(m_hInstance, hrsrc);
	return (DLGTEMPLATEEX *)LockResource(hglb);
}

VOID ComponentDialog::OnSelChanged(HWND hwndDlg)
{
	// Get the dialog header data.
	DLGHDR *pHdr = (DLGHDR *)GetWindowLongPtr(
		hwndDlg, GWLP_USERDATA);

	// Get the index of the selected tab.
	int iSel = TabCtrl_GetCurSel(pHdr->hwndTab);

	// Destroy the current child dialog box, if any. 
	if (pHdr->hwndDisplay != NULL)
		DestroyWindow(pHdr->hwndDisplay);

	// Create the new child dialog box. Note that g_hInst is the
	// global instance handle.
	/*pHdr->hwndDisplay = CreateDialogIndirect(m_hInstance,
		(DLGTEMPLATE *)pHdr->apRes[iSel], hwndDlg, ChildProc);*/
	pHdr->hwndDisplay = CreateDialog(m_hInstance, MAKEINTRESOURCE(IDD_DIALOG4), hwndDlg, ChildProc);


	
	return;
}

