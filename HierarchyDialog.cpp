#include "HierarchyDialog.h"

HierarchyDialog* g_HierarchyDialog = 0;


bool HierarchyDialog::instantiated = false;


bool HierarchyDialog::OpenDialog()
{
	if (!IsWindow(m_hDlg))
	{
		m_hDlg = CreateDialog(0, MAKEINTRESOURCE(IDD_DIALOG2), 0, DlgProc);

		ShowWindow(m_hDlg, SW_SHOW);
		return true;
	}

	return false;
}
bool HierarchyDialog::OpenDialog(HWND hwnd)
{
	hWnd = hwnd;
	if (!IsWindow(m_hDlg))
	{
		m_hDlg = CreateDialog(m_hInstance, MAKEINTRESOURCE(IDD_DIALOG2), 0, DlgProc);

		ShowWindow(m_hDlg, SW_SHOW);
		return true;
	}

	return false;
}

void HierarchyDialog::Init(HWND hWnd_main)
{
	hWnd = hWnd_main;

	InitCommonControls();
}

void HierarchyDialog::treeInit(HWND hDlg)
{
	m_hwndTV = GetDlgItem(hDlg, IDC_TREE2);
	TreeImageSet();
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
		g_HierarchyDialog->WindowSizing(hDlg);
			   		
		//tree control의 핸들을 얻어와서 멤버변수에 저장
		g_HierarchyDialog->treeInit(hDlg);
		
		return (INT_PTR)TRUE;
	}

	//case WM_COMMAND:
	//	g_HierarchyDialog->MenuProc(hDlg, wParam);
	//	return (INT_PTR)TRUE;
	//case WM_KEYDOWN:
	//	g_HierarchyDialog->KeyDownProc(wParam);
	//	return (INT_PTR)TRUE;
	//case WM_MOUSEMOVE:
	//	g_HierarchyDialog->MouseMoveProc(wParam, lParam);
	//	return (INT_PTR)TRUE;
	//case WM_NOTIFY: //리스트 컨트롤 클릭했을 때 발생
	//	g_HierarchyDialog->NotifyProc(lParam);
	case WM_MOVE: //
	{
		g_HierarchyDialog->WindowSizing(hDlg);
		return (INT_PTR)TRUE;
	}

	}
	return (INT_PTR)FALSE;
}

void HierarchyDialog::MenuProc(HWND hDlg, WPARAM wParam)
{
	int wmId = LOWORD(wParam);
}





void HierarchyDialog::KeyDownProc(WPARAM wParam)
{
}


void HierarchyDialog::MouseMoveProc(WPARAM btnState, LPARAM mousePos)
{
}



void HierarchyDialog::CharProc(HWND hDlg, WPARAM wParam)
{
}

void HierarchyDialog::NotifyProc(LPARAM lParam)
{
	//컨트롤이 선택됨
	//switch (((LPNMHDR)lParam)->code)
	//{
	//case NM_CLICK:
	//	int n = TreeView_GetCount(m_hwndTV);
	//	TCHAR str[100];
	//	wsprintf(str, TEXT("tree count : %d"), n);
	//	//MessageBox(m_hDlg, str, L"test", MB_OK);

	//	return;
	//}
}

void HierarchyDialog::WindowSizing(HWND hDlg)
{
	//컴퓨터 전체스크린의 크기
	int width = GetSystemMetrics(SM_CXSCREEN);
	int height = (GetSystemMetrics(SM_CYSCREEN) - 30); //30은 작업표시줄 크기
	

	RECT mainWndRect;
	GetWindowRect(hWnd, &mainWndRect);

	MoveWindow(hDlg, mainWndRect.right-20,
		mainWndRect.top,
		width - (mainWndRect.right - mainWndRect.left),
		(mainWndRect.bottom - mainWndRect.top), true);
}

HTREEITEM HierarchyDialog::TreeViewInsertItem(Object* item, HTREEITEM parent)
{
	TVITEM tvi;
	TVINSERTSTRUCT tvins;

	WCHAR itemName[100];
	wsprintf(itemName, L"%s", item->GetName().c_str());

	//TVI_FIRST, TVI_LAST, TVI_ROOT, TVI_SORT 삽입위치
	tvi.mask = TVIF_TEXT | TVIF_IMAGE
		| TVIF_SELECTEDIMAGE | TVIF_PARAM;

	//text 설정
	tvi.pszText = itemName;
	tvi.cchTextMax = wcslen(tvi.pszText) + 1;


	//image 설정
	tvi.iImage = m_Closed;
	tvi.iSelectedImage = m_nOpen;

	tvi.lParam = 1;
	tvins.hParent = parent;
	tvins.item = tvi;
	tvins.hInsertAfter = TVI_FIRST;


	return TreeView_InsertItem(m_hwndTV, &tvins);
}

void HierarchyDialog::TreeInsertObject(Object* obj, HTREEITEM parent)
{

	//매개변수 object가 root일 경우
	if (obj->parent == nullptr)
	{
		//root 아이템 삽입
		parent = TreeViewInsertItem(obj);
	}

	//object가 root가 아닐경우 parent의 자식으로 추가
	//parent를 방금 추가한 item으로 변경
	parent = TreeViewInsertItem(obj, parent);

	//object의 자식들에 대해서 재귀
	for (Object* child : obj->childs)
	{
		TreeInsertObject(child, parent);
	}

	return;
}

bool HierarchyDialog::TreeImageSet()
{
	HIMAGELIST imageList;
	HBITMAP hbmp;

	//cInitial = 이미지 개수, cGrow = 최대 이미지 수(capacity)?
	if ((imageList =
		ImageList_Create(16, 16,
			ILC_COLOR24, 2, 10)) == NULL)
		return false;

	//열린 상자 이미지
	hbmp = LoadBitmap(m_hInstance, MAKEINTRESOURCE(IDB_BITMAP2));
	m_nOpen = ImageList_Add(imageList, hbmp, (HBITMAP)NULL);
	DeleteObject(hbmp);

	//닫힌 상자 이미지
	hbmp = LoadBitmap(m_hInstance, MAKEINTRESOURCE(IDB_BITMAP2));
	m_Closed = ImageList_Add(imageList, hbmp, (HBITMAP)NULL);
	DeleteObject(hbmp);

	//이미지 추가 실패한 것이 있으면
	if (ImageList_GetImageCount(imageList) < 2)
		return false;

	TreeView_SetImageList(m_hwndTV, imageList, TVSIL_NORMAL);

	return true;
}








