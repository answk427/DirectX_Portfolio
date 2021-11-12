#include "DialogPractice.h"

DialogPractice* g_HierarchyDialog = 0;


bool DialogPractice::instantiated = false;


bool DialogPractice::OpenDialog()
{
	if(!IsWindow(m_hDlg))
	{
		m_hDlg = CreateDialog(0, MAKEINTRESOURCE(IDD_DIALOG1), 0, DlgProc);

		ShowWindow(m_hDlg, SW_SHOW);
		return true;
	}

	return false;
}
bool DialogPractice::OpenDialog(HWND hwnd)
{
	hWnd = hwnd;
	if (!IsWindow(m_hDlg))
	{
		m_hDlg = CreateDialog(m_hInstance, MAKEINTRESOURCE(IDD_DIALOG1), 0, DlgProc);
		
		ShowWindow(m_hDlg, SW_SHOW);
		return true;
	}
	
	return false;
}

void DialogPractice::Init(HWND hWnd_main)
{
	hWnd = hWnd_main;
	
	InitCommonControls();
}

void DialogPractice::treeInit(HWND hDlg)
{
	m_hwndTV = GetDlgItem(hDlg, IDC_TREE1);
	TreeImageSet();
}

DialogPractice::DialogPractice(HINSTANCE hInstance) : MsgProcedure(hInstance) 
{
	assert(!instantiated);
	instantiated = true;
	g_HierarchyDialog = this;
}
INT_PTR CALLBACK DialogPractice::DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{	
	case WM_CLOSE:
		EndDialog(hDlg, 0);
		return (INT_PTR)TRUE;
	case WM_INITDIALOG:
	{
		g_HierarchyDialog->testFunc();

		
		//�޺��ڽ��� �׸� �߰�
		HWND comboBox1 = GetDlgItem(hDlg, IDC_COMBO1);
		HWND listBox1 = GetDlgItem(hDlg, IDC_LIST1);
		//tree control�� �ڵ��� ���ͼ� ��������� ����
		g_HierarchyDialog->treeInit(hDlg);
		WCHAR* strMenu[] = { L"hi", TEXT("hello"), L"bonjour" };
		for (int i = 0; i < 3; i++)
		{
			//�޺��ڽ��� �߰��ϴ� ��ũ�� �Լ�
			ComboBox_AddString(comboBox1, strMenu[i]);
			//����Ʈ�ڽ��� �߰��ϴ� ��ũ�� �Լ�
			ListBox_AddString(listBox1, strMenu[i]);
		}
			
		ComboBox_AddString(comboBox1, L"���ͷ����ڷ� �߰�");
		ComboBox_InsertString(comboBox1, 3, L"�μ�Ʈ����");
		
		// ************ ����Ʈ ��Ʈ�� *************//
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
	case WM_NOTIFY: //����Ʈ ��Ʈ�� Ŭ������ �� �߻�
		g_HierarchyDialog->NotifyProc(lParam);
		return (INT_PTR)TRUE;
	case WM_MOVE: //
	{
		g_HierarchyDialog->testFunc();
		return (INT_PTR)TRUE;
	}

	}
	return (INT_PTR)FALSE;
}

void DialogPractice::MenuProc(HWND hDlg,WPARAM wParam)
{
	int wmId = LOWORD(wParam);
	
	WCHAR title[MAX_PATH];
	WCHAR full_path[MAX_PATH];

	

	switch (wmId)
	{
	case ID_40001: //����
		FileOpenDialog(hDlg, title, full_path);
		MessageBox(0, L"����", L"����", MB_OK);
		break;
	case ID_40002: //����
		FileSaveDialog(hDlg, full_path);
		MessageBox(0, L"����", L"����", MB_OK);
		break;
	case IDC_BUTTON1: //��ư1
	{
		//������ �߰�
		LVITEM lvItem;
		HWND hLV = GetDlgItem(hDlg, IDC_LIST2);
		lvItem.iItem = ListView_GetItemCount(hLV); //�߰��� �ε��� ��ġ
		lvItem.iSubItem = 0;
		lvItem.mask = LVIF_TEXT;
		lvItem.pszText = L"test Item";
		ListView_InsertItem(hLV, &lvItem);

		//��������� �߰�
		
		ListView_SetItemText(hLV, lvItem.iItem, 0, L"0�� test Sub Item"); 
		ListView_SetItemText(hLV, lvItem.iItem, 1, L"1�� test Sub Item");
		ListView_SetItemText(hLV, lvItem.iItem, 2, L"2�� test Sub Item");
		ListView_SetItemText(hLV, lvItem.iItem, 3, L"3�� test Sub Item");
		MessageBox(hDlg, L"IDC_BUTTON1", L"dialog button", MB_OK);

				
		break;
	}
		
	case IDC_BUTTON2: //��ư2
		MessageBox(0, L"IDC_BUTTON2", L"dialog button", MB_OK);
		break;

	/*
		���� ��ư ���� ����
		UINT IsDlgButtonChecked(HWND hDlg, int nIDButton);
		���� �ڵ� = BST_CHECKED , BST_INDETERMINATE, BST_UNCHECKED
		��ü���� �Ҷ� �迭�̿� int nRadioID[2] = {IDC_RADIO01, IDC_RADIO02}

		--------

		�׷� ������(�׷캰�� ����)
		���� ��ư �Ӽ��� Group �Ӽ��� ����.
		True�� ������ ���� ��ư���� ���� True�� ������ ���� ��ư���� �� �׷����� ����.
		
		���º����Լ�
		BOOL CheckDlgButton(HWND hDlg, int nIDButton, UINT uCheck) ��ư�� ���������� ���� �� ����. ���� �׷��� ������ư���� 2���� ���� �� ���� ����.(�Ҽ������� ����)
		=> uCheck = BST_CHECKED / BST_UNCHECKED
		BOOL CheckRadioButton(HWND hDlg, int nIDFirstButton, int nIDLastButton, int nIDCheckButton) �׷� �ȿ��� �ϳ��� ����
		������ ����. ������ ���õ��ִٸ� �������� ����. nIDCheckButton�� ����
		nIDFirstButton = �׷��� ���� , nIDLastButton = �׷��� ������
	*/
	case IDC_RADIO1:
		MessageBox(0, L"IDC_RADIO1", L"dialog button", MB_OK);
		break;
	case IDC_RADIO2:
		MessageBox(0, L"IDC_RADIO2", L"dialog button", MB_OK);
		break;

	/* üũ ��ư 
		���� ���� �Լ�
		UINT IsDlgButtonChecked(HWND hDlg, int nIDButton);
		���� �ڵ� BST_CHECKED, BST_INDETERMINATE, BST_UNCHECKED

		���� �����Լ� CheckDlgButton 
	*/
	case IDC_CHECK1:
		if (IsDlgButtonChecked(hDlg, IDC_CHECK2)==BST_CHECKED)
			MessageBox(0, L"üũ2 üũ��", L"checkcheck", MB_OK);
		else
			MessageBox(0, L"üũ2 üũ�ȵ�", L"checkcheck", MB_OK);
		break;
	case IDC_CHECK2:
		CheckDlgButton(hDlg, IDC_CHECK1, BST_CHECKED);
		break;


	}
}





void DialogPractice::KeyDownProc(WPARAM wParam)
{
	{
		//Ű�� ���Ǵ��� �ȴ��Ǵ��� Ȯ��?
		//��� Ű�� ���� �߻��ϴ� �޽��� WM_KEYDOWN
		//WPARAM : ���� Ű �ڵ�, �빮�ڷθ� �ν�
		//���� Ű �ڵ� VK_LEFT, VK_HOME ��

		switch (wParam) //MSDN���� VK���� ����
		{
		case VK_LEFT:

			MessageBox(0, L"VK_LEFT", L"����", MB_OK);
			return;
		case VK_RIGHT:
			MessageBox(0, L"VK_RIGHT", L"����", MB_OK);
			return;
		}

		//SHORT GetAsyncKeyState(int vKey)
		//�ǽð����� Ű �Է��� üũ
		//�޽��� ť�� ����Ǵ� Ű �޽����� ������ ����. ���ӿ��� ���
		//Ű ������ ������ ������ ����
		//if(GetAsyncKeyState(VK_LEFT) <0) ������

	}
}


void DialogPractice::MouseMoveProc(WPARAM btnState, LPARAM mousePos)
{
	//WM_MOUSEMOVE ���콺 �̵��� �߻�
		//���콺 ��ġ ����
		//LOWORD(LPARAM) : X��ǥ HIWORD(LPARAM) : Y��ǥ


		//���콺 ��ư �ڵ�
		//wParam���� ���޵Ǵ� �ڵ�
		//MK_LBUTTON, MK_RBUTTON <-�巡�� Ȯ�ΰ���
		//MK_MBUTTON (�߰���ư), MK_SHIFT, MK_CONTROL



		//�� �� ���콺 �޽���
		//WM_LBUTTONDOWN ���콺 ���� ��ư ����
		//WM_LBUTTONUP ���콺 ���� ��ư ����
}



void DialogPractice::CharProc(HWND hDlg, WPARAM wParam)
{
}

void DialogPractice::NotifyProc(LPARAM lParam)
{
	//��Ʈ���� ���õ�
	switch (((LPNMHDR)lParam)->code)
	{
		case NM_CLICK :
			int n = TreeView_GetCount(m_hwndTV);
			TCHAR str[100];
			wsprintf(str, TEXT("tree count : %d"), n);
			//MessageBox(m_hDlg, str, L"test", MB_OK);
			
			return;
	}
}

void DialogPractice::testFunc()
{
	RECT mainWndRect;
	GetWindowRect(hWnd, &mainWndRect);

	MoveWindow(m_hDlg, mainWndRect.right-25,
		mainWndRect.top,
		(mainWndRect.right - mainWndRect.left) / 2,
		(mainWndRect.bottom - mainWndRect.top), true);
}

HTREEITEM DialogPractice::TreeViewInsertItem(Object* item, HTREEITEM parent)
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
	tvi.cchTextMax = wcslen(tvi.pszText)+1;
	
	
	//image ����
	tvi.iImage = m_Closed;
	tvi.iSelectedImage = m_nOpen;
	
	tvi.lParam = 1;
	tvins.hParent = parent;
	tvins.item = tvi;
	tvins.hInsertAfter = TVI_FIRST;
	
	
	return TreeView_InsertItem(m_hwndTV, &tvins);
}

void DialogPractice::TreeInsertObject(Object* obj, HTREEITEM parent)
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

bool DialogPractice::TreeImageSet()
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








