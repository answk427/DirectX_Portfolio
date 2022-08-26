#include "HierarchyDialog.h"

HierarchyDialog* g_HierarchyDialog = 0;


bool HierarchyDialog::instantiated = false;


bool HierarchyDialog::OpenDialog()
{
	if (!IsWindow(m_hDlg))
	{
		m_hDlg = CreateDialog(m_hInstance, MAKEINTRESOURCE(IDD_DIALOG2), 0, DlgProc);

		ShowWindow(m_hDlg, SW_SHOW);
		return true;
	}

	return false;
}
bool HierarchyDialog::OpenDialog(HWND hwnd)
{
	inspector.Init(hwnd);
	inspector.OpenDialog();
	
	hWnd = hwnd;
	if (!IsWindow(m_hDlg))
	{
		m_hDlg = CreateDialog(m_hInstance, MAKEINTRESOURCE(IDD_DIALOG2), 0, DlgProc);

		ShowWindow(m_hDlg, SW_SHOW);
		return true;
	}

	return false;
}

void HierarchyDialog::Init(HWND hWnd_main, BoundingBoxRenderer* bbr, MoveToolRenderer* moveToolRenderer)
{
	hWnd = hWnd_main;
	m_boundingBoxRenderer = bbr;
	m_moveToolRenderer = moveToolRenderer;

	InitCommonControls();
}

void HierarchyDialog::treeInit(HWND hDlg)
{
	
	m_hwndTV = GetDlgItem(hDlg, IDC_TREE2);
	TreeImageSet();
	SetWindowSubclass(m_hwndTV, myListBoxSubclassProc, 0, 0);
}

void HierarchyDialog::SelectObject(gameObjectID & objID)
{
	TreeView_SelectItem(m_hwndTV, Map_idItem[objID]);
	return;
}

bool HierarchyDialog::TreeViewDeleteItem(HTREEITEM item)
{
	return TreeView_DeleteItem(m_hwndTV, item);
}

bool HierarchyDialog::TreeViewDeleteItem()
{
	static int cnt = 0;
	cnt++;
	TVITEM tvi;
	
	//현재 선택된 아이템
	tvi.mask = TVIF_PARAM;
	tvi.hItem = TreeView_GetSelection(m_hwndTV);
	

	if (tvi.hItem == NULL)
	{
		MessageBox(m_hDlg, TEXT("TreeView_GetSelection Fail"), TEXT("Hierarchy Select"), MB_OK);
		return nullptr;
	}

	//아이템에 등록된 object를 가져온다.
	TreeView_GetItem(m_hwndTV, &tvi);
	Object* tempObj = (Object*)(tvi.lParam);

	if (tempObj == NULL)
	{
		MessageBox(m_hDlg, TEXT("TreeView_GetItem Fail"), TEXT("Hierarchy Select"), MB_OK);
		return nullptr;
	}
	
	bool result = TreeView_DeleteItem(m_hwndTV, tvi.hItem);

	//삭제가 제대로 안됐을 경우
	if (result == false)
		return nullptr;

	ObjectMgr& objectMgr = ObjectMgr::Instance();
	
			
	return objectMgr.DeleteObject(tempObj->GetID());
}

HierarchyDialog::HierarchyDialog(HINSTANCE hInstance) : MsgProcedure(hInstance), inspector(hInstance)
,m_boundingBoxRenderer(0), copyObj(0), m_moveToolRenderer(0)
{
	assert(!instantiated);
	instantiated = true;
	g_HierarchyDialog = this;
}

/////////////////////////////////////Procedure/////////////////////////
LRESULT CALLBACK myListBoxSubclassProc(HWND hWnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam,
	UINT_PTR uIdSubclass,
	DWORD_PTR dwRefData)
{
	switch (message)
	{
	case WM_KEYDOWN:
		g_HierarchyDialog->KeyDownProc(wParam);
		break;
	}
	return DefSubclassProc(hWnd, message, wParam, lParam);
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
	case WM_KEYDOWN:
		g_HierarchyDialog->KeyDownProc(wParam);
		return (INT_PTR)TRUE;
	//case WM_MOUSEMOVE:
	//	g_HierarchyDialog->MouseMoveProc(wParam, lParam);
	//	return (INT_PTR)TRUE;
	case WM_NOTIFY: //Tree View에 대한 조작
		g_HierarchyDialog->NotifyProc(lParam);
		return (INT_PTR)TRUE;
	case WM_MOVE: //
	{
		g_HierarchyDialog->WindowSizing(hDlg);
		return (INT_PTR)TRUE;
	}

	}
	return (INT_PTR)FALSE;
}
/////////////////////////////////////Procedure/////////////////////////

void HierarchyDialog::MenuProc(HWND hDlg, WPARAM wParam)
{
	int wmId = LOWORD(wParam);
}





void HierarchyDialog::KeyDownProc(WPARAM wParam)
{
	switch (wParam)
	{
	case VK_DELETE:
		TreeViewDeleteItem();
		break;
	case VK_F2:
		TVITEM tvi;

		//현재 선택된 아이템
		TCHAR buf[512];
		
		//TreeView_GetISearchString(m_hwndTV, TEXT("EmptyObject"));
		//HWND editTV = TreeView_EditLabel(m_hwndTV, TreeView_GetSelection(m_hwndTV));
		//SendMessage(editTV, WM_GETTEXT, sizeof(buf) / sizeof(TCHAR), (LPARAM)(void*)buf);
		//TreeView_EndEditLabelNow(m_hwndTV, true);
	
		int a = 0;
		break;
	}
}


void HierarchyDialog::MouseMoveProc(WPARAM btnState, LPARAM mousePos)
{
}



void HierarchyDialog::CharProc(HWND hDlg, WPARAM wParam)
{
}

void HierarchyDialog::NotifyProc(LPARAM lParam)
{
			

	switch (((LPNMHDR)lParam)->code)
	{
	//트리뷰에서 선택항목이 변경되었을 경우
	case TVN_SELCHANGED:
		int n = TreeView_GetCount(m_hwndTV);
		SelectItem();

		return;
	}
}

void HierarchyDialog::WindowSizing(HWND hDlg)
{
	
	//컴퓨터 전체스크린의 크기
	int width = GetSystemMetrics(SM_CXSCREEN);
	int height = (GetSystemMetrics(SM_CYSCREEN) - 30); //30은 작업표시줄 크기
	

	RECT mainWndRect;
	GetWindowRect(hWnd, &mainWndRect);

	//dialog 위치, 크기 조정
	MoveWindow(hDlg, width * MAINWINDOWWIDTHRATE,
		mainWndRect.top,
		width * HIERARCHYWIDTHRATE,
		height * HIERARCHYHEIGHTRATE, true);

	//tree control 위치, 크기 조정
	RECT dlgWndRect;
	GetWindowRect(hDlg, &dlgWndRect);
	HWND hwndTV = GetDlgItem(hDlg, IDC_TREE2);
	MoveWindow(hwndTV, 0,
		0,
		dlgWndRect.right - dlgWndRect.left,
		dlgWndRect.bottom - dlgWndRect.top, true);
}

void HierarchyDialog::SelectItem()
{
	TVITEM tvi;
	//현재 선택된 아이템
	tvi.hItem = TreeView_GetSelection(m_hwndTV);
	tvi.mask = TVIF_PARAM;

	if (tvi.hItem == NULL) 
	{
		MessageBox(m_hDlg, TEXT("TreeView_GetSelection Fail"), TEXT("Hierarchy Select"), MB_OK);
		return;
	}
		

	TreeView_GetItem(m_hwndTV, &tvi);
	Object* tempObj = (Object*)(tvi.lParam);

	if (tempObj == NULL)
	{
		MessageBox(m_hDlg, TEXT("TreeView_GetItem Fail"), TEXT("Hierarchy Select"), MB_OK);
		return;
	}
		

	inspector.SetObject(dynamic_cast<GameObject*>(tempObj));
	m_boundingBoxRenderer->SetObject(dynamic_cast<GameObject*>(tempObj));
	m_moveToolRenderer->SetObject(dynamic_cast<GameObject*>(tempObj));

	return;
	
}

Object * HierarchyDialog::GetSelectObject()
{
	TVITEM tvi;
	//현재 선택된 아이템
	tvi.hItem = TreeView_GetSelection(m_hwndTV);
	tvi.mask = TVIF_PARAM;

	if (tvi.hItem == NULL)
	{
		MessageBox(m_hDlg, TEXT("TreeView_GetSelection Fail"), TEXT("Hierarchy Select"), MB_OK);
		return nullptr;
	}


	TreeView_GetItem(m_hwndTV, &tvi);
	
	return (Object*)(tvi.lParam);
}

void HierarchyDialog::CtrlC_copy()
{
	if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
	{
		if (GetAsyncKeyState('C') & 0x8000)
		{
			copyObj = dynamic_cast<GameObject*>(GetSelectObject());
			if (copyObj != nullptr)
			{
				if (m_hDlg != nullptr)
					MessageBox(m_hDlg, copyObj->GetName().c_str(), TEXT("control"), MB_OK);
			}

		}
	}
}

void HierarchyDialog::CtclV_paste()
{
	if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
	{
		if (GetAsyncKeyState('V') & 0x8000)
		{
			copyObj = dynamic_cast<GameObject*>(GetSelectObject());
			if (copyObj != nullptr)
			{
				if (m_hDlg != nullptr)
					MessageBox(m_hDlg, copyObj->GetName().c_str(), TEXT("control"), MB_OK);
			}

		}
	}
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

	tvi.lParam = (LPARAM)item;
	tvins.hParent = parent;
	tvins.item = tvi;
	tvins.hInsertAfter = TVI_FIRST;
	

	return TreeView_InsertItem(m_hwndTV, &tvins);
}

HTREEITEM HierarchyDialog::TreeInsertObject(Object* obj, HTREEITEM parent)
{
	if (obj == nullptr)
		return parent;
	//매개변수 object가 root일 경우
	if (obj->parent == nullptr)
	{
		//root 아이템 삽입
		parent = TreeViewInsertItem(obj);
		Map_idItem.insert({ obj->GetID(), parent });
	}
	else
	{
		//object가 root가 아닐경우 parent의 자식으로 추가
		//parent를 방금 추가한 item으로 변경
		parent = TreeViewInsertItem(obj, parent);
		Map_idItem.insert({ obj->GetID(), parent });
	}

	

	//object의 자식들에 대해서 재귀
	for (Object* child : obj->childs)
	{
		TreeInsertObject(child, parent);
	}

	return parent;
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








