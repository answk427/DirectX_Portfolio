#include "Inspector.h"

Inspector* g_Inspector = 0;
bool Inspector::instantiated = false;
extern std::vector<std::string> idFront;


bool Inspector::OpenDialog()
{
	if (!IsWindow(m_hDlg))
	{
		m_hDlg = CreateDialog(m_hInstance, MAKEINTRESOURCE(INSPECTOR), 0, Inspector::DlgProc);
		
		ShowWindow(m_hDlg, SW_SHOW);
				
		return true;
	}

	return false;
}
bool Inspector::OpenDialog(HWND hwnd)
{
	
	hWnd = hwnd;
	if (!IsWindow(m_hDlg))
	{
		m_hDlg = CreateDialog(m_hInstance, MAKEINTRESOURCE(INSPECTOR), 0, Inspector::DlgProc);

		ShowWindow(m_hDlg, SW_SHOW);
		return true;
	}

	return false;
}

void Inspector::Init(HWND hWnd_main)
{
	hWnd = hWnd_main;

	InitCommonControls();
}

Inspector::Inspector(HINSTANCE hInstance) : MsgProcedure(hInstance), m_pHdr(0), m_currObject(0),
				m_MeshRendererDialog(hInstance), m_TransformDialog(hInstance), m_LightDialog(hInstance)
{
	assert(!instantiated);
	instantiated = true;
	g_Inspector = this;

	dialogs.reserve(20);
}

INT_PTR CALLBACK Inspector::DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_CLOSE:
		if (g_Inspector->m_pHdr != nullptr)
			LocalFree(g_Inspector->m_pHdr);
		EndDialog(hDlg, 0);
		return (INT_PTR)TRUE;
	case WM_INITDIALOG:
	{
		g_Inspector->WindowSizing(hDlg);
		g_Inspector->OnTabbedDialogInit(hDlg);

		return (INT_PTR)TRUE;
	}

	case WM_COMMAND:
		g_Inspector->MenuProc(hDlg, wParam);
		return (INT_PTR)TRUE;
	case WM_KEYDOWN:
		MessageBox(hDlg, L"Inspector KeyDown", L"Inspector", MB_OK);
		g_Inspector->KeyDownProc(wParam);
		return (INT_PTR)TRUE;
	case WM_MOUSEMOVE:
		g_Inspector->MouseMoveProc(wParam, lParam);
		return (INT_PTR)TRUE;
	case WM_NOTIFY: //리스트 컨트롤 클릭했을 때 발생
		g_Inspector->NotifyProc(hDlg,lParam);
		return (INT_PTR)TRUE;
	case WM_MOVE: 
	{
		
		return (INT_PTR)TRUE;
	}

	}
	return (INT_PTR)FALSE;
}

void Inspector::MenuProc(HWND hDlg, WPARAM wParam)
{
	//AddComponentCommand 로부터의 호출인지 검사
	if (wParam == (UINT)(&m_hDlg))
	{
		AddTab(hDlg);
		return;
	}
		

	//LOWORD(wParam) = 컨트롤 식별
	int wmId = LOWORD(wParam);
	



	switch (wmId)
	{
	case ADDCOMPONENTBTN:
		if (HIWORD(wParam) == BN_CLICKED)
		{
			//콤보박스의 현재 index 확인
			int idx = ComboBox_GetCurSel(m_hCombo);
			if (idx == -1)
				break;
			//Component 추가
			if(m_currObject != nullptr)
				CommandQueue::AddCommand(new AddComponentCommand(m_currObject, (ComponentType)idx, &m_hDlg));
			
		}
		
	}

	

}





void Inspector::KeyDownProc(WPARAM wParam)
{
	
}


void Inspector::MouseMoveProc(WPARAM btnState, LPARAM mousePos)
{
	
}



void Inspector::CharProc(HWND hDlg, WPARAM wParam)
{

}

void Inspector::NotifyProc(HWND hDlg,LPARAM lParam)
{
	UINT codeTest = ((LPNMHDR)lParam)->code;
	switch (((LPNMHDR)lParam)->code)
	{
		
	//탭 컨트롤이 선택됨
	case TCN_SELCHANGE:
		return OnSelChanged(hDlg);
				
	}
}

void Inspector::WindowSizing(HWND hwndDlg)
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

	HWND button = GetDlgItem(hwndDlg, ADDCOMPONENTBTN);
	MoveWindow(button, 180, 690, 100, 30, true);

	HWND combo =GetDlgItem(hwndDlg, COMPONENTCOMBO);
	MoveWindow(combo , 70, 695, 100, 150,true);

}



HRESULT Inspector::OnTabbedDialogInit(HWND hwndDlg)
{
	INITCOMMONCONTROLSEX iccex;
	
	m_hTab = GetDlgItem(hwndDlg, IDC_TAB1);
	m_hBtn_addComponent = GetDlgItem(hwndDlg, ADDCOMPONENTBTN);
	m_hCombo = GetDlgItem(hwndDlg, COMPONENTCOMBO);

	USES_CONVERSION;
	wchar_t itemName[100];
	
	//combo박스에 item 추가
	for (auto& elem : idFront)
	{	
		swprintf(itemName, L"%s", A2W(elem.c_str()));
		ComboBox_AddString(m_hCombo, itemName);
		
	}
	

	
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

	m_pHdr->hwndTab = GetDlgItem(hwndDlg, TABCONTROLID);

	if (m_pHdr->hwndTab == NULL)
	{
		return HRESULT_FROM_WIN32(GetLastError());
	}



	// Lock the resources for the three child dialog boxes. 
	m_pHdr->apRes[0] = DoLockDlgRes(MAKEINTRESOURCE(TAB_TRANSFORM));
	m_pHdr->apRes[1] = DoLockDlgRes(MAKEINTRESOURCE(TAB_MESHRENDERER));
	m_pHdr->apRes[2] = DoLockDlgRes(MAKEINTRESOURCE(TAB_TERRAIN));
	
	
	// Calculate how large to make the tab control, so 
	// the display area can accommodate all the child dialog boxes. 
	TabCtrl_AdjustRect(m_pHdr->hwndTab, TRUE, &rcTab);
	GetClientRect(m_pHdr->hwndTab, &rcTab);
	


	// Calculate the display rectangle. 
	CopyRect(&m_pHdr->rcDisplay, &rcTab);
	TabCtrl_AdjustRect(m_pHdr->hwndTab, FALSE, &m_pHdr->rcDisplay);

	// Simulate selection of the first item. 
	OnSelChanged(hwndDlg);

	return S_OK;
}

DLGTEMPLATEEX * Inspector::DoLockDlgRes(LPCTSTR lpszResName)
{
	HRSRC hrsrc = FindResource(NULL, lpszResName, RT_DIALOG);

	// Note that m_hInstance is the global instance handle
	HGLOBAL hglb = LoadResource(m_hInstance, hrsrc);
	return (DLGTEMPLATEEX *)LockResource(hglb);
}

VOID Inspector::OnSelChanged(HWND hwndDlg)
{
	if (m_currObject == nullptr)
		return;

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
	dialogs[iSel]->OpenDialog(hwndDlg);
	pHdr->hwndDisplay = dialogs[iSel]->m_hDlg;
	
	if (m_currObject != nullptr)
		dialogs[iSel]->UpdateView();
			
	return;
}

void Inspector::AddTab(HWND hTab)
{
	
	if (m_currObject == nullptr)
		return;
	
	TabCtrl_DeleteAllItems(m_pHdr->hwndTab);
		
	dialogs.clear();

	// 오브젝트의 컴포넌트를 읽고 해당하는 탭을 추가
	TCITEM tie;
	tie.mask = TCIF_TEXT | TCIF_IMAGE;
	tie.iImage = -1;

	const std::vector<ComponentOfObject> components = m_currObject->GetComponents();

	//transform은 무조건 있으므로 첫번째로 추가
	tie.pszText = L"Transform";
	dialogs.push_back(&m_TransformDialog);
	TabCtrl_InsertItem(m_pHdr->hwndTab, 0, &tie);
	
	for (int i=0; i<components.size(); i++)
	{
		switch (components[i].GetComponentType())
		{
		case ComponentType::MESHRENDERER:
			tie.pszText = L"MeshRenderer";
			//i번째 탭은 dialogs vector의 i번째 원소를 사용함
			dialogs.push_back(&m_MeshRendererDialog);
			break;
		case ComponentType::SKINNEDMESHRENDERER:
			tie.pszText = L"SkinnedMeshRenderer";
			//미구현
			break;
		case ComponentType::LIGHT:
			tie.pszText = L"LIGHT";
			dialogs.push_back(&m_LightDialog);
			break;
		default:
			break;
		}
		TabCtrl_InsertItem(m_pHdr->hwndTab, i+1, &tie);
	}
	
}

void Inspector::SetObject(GameObject * obj)
{
	m_currObject = obj; 
	AddTab(m_hTab);
	for (auto& elem : dialogs)
	{
		elem->SetObject(obj);
	}
	
	OnSelChanged(m_hDlg);
}

bool Inspector::AddComponent(ComponentType componentType)
{
	if (m_currObject == nullptr)
		return false;

	ComponentMgr& componentMgr = ComponentMgr::Instance();
	Component* component = componentMgr.CreateComponent(componentType);

	if (component == nullptr)
		return false;


	return m_currObject->AddComponent(component);
}

void Inspector::Update()
{
	if (m_pHdr != nullptr)
	{
		int idx = TabCtrl_GetCurSel(m_pHdr->hwndTab);
		if(idx!=-1)
			dialogs[idx]->UpdateView();
	}
}

