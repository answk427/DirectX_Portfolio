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
			   		
		//tree control�� �ڵ��� ���ͼ� ��������� ����
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
	//case WM_NOTIFY: //����Ʈ ��Ʈ�� Ŭ������ �� �߻�
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
	//��Ʈ���� ���õ�
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
	//��ǻ�� ��ü��ũ���� ũ��
	int width = GetSystemMetrics(SM_CXSCREEN);
	int height = (GetSystemMetrics(SM_CYSCREEN) - 30); //30�� �۾�ǥ���� ũ��
	

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

	//TVI_FIRST, TVI_LAST, TVI_ROOT, TVI_SORT ������ġ
	tvi.mask = TVIF_TEXT | TVIF_IMAGE
		| TVIF_SELECTEDIMAGE | TVIF_PARAM;

	//text ����
	tvi.pszText = itemName;
	tvi.cchTextMax = wcslen(tvi.pszText) + 1;


	//image ����
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

	//�Ű����� object�� root�� ���
	if (obj->parent == nullptr)
	{
		//root ������ ����
		parent = TreeViewInsertItem(obj);
	}

	//object�� root�� �ƴҰ�� parent�� �ڽ����� �߰�
	//parent�� ��� �߰��� item���� ����
	parent = TreeViewInsertItem(obj, parent);

	//object�� �ڽĵ鿡 ���ؼ� ���
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

	//cInitial = �̹��� ����, cGrow = �ִ� �̹��� ��(capacity)?
	if ((imageList =
		ImageList_Create(16, 16,
			ILC_COLOR24, 2, 10)) == NULL)
		return false;

	//���� ���� �̹���
	hbmp = LoadBitmap(m_hInstance, MAKEINTRESOURCE(IDB_BITMAP2));
	m_nOpen = ImageList_Add(imageList, hbmp, (HBITMAP)NULL);
	DeleteObject(hbmp);

	//���� ���� �̹���
	hbmp = LoadBitmap(m_hInstance, MAKEINTRESOURCE(IDB_BITMAP2));
	m_Closed = ImageList_Add(imageList, hbmp, (HBITMAP)NULL);
	DeleteObject(hbmp);

	//�̹��� �߰� ������ ���� ������
	if (ImageList_GetImageCount(imageList) < 2)
		return false;

	TreeView_SetImageList(m_hwndTV, imageList, TVSIL_NORMAL);

	return true;
}








