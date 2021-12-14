#include "MeshRendererDialog.h"


extern MeshRendererDialog* g_MeshRendererDialog = 0;
bool MeshRendererDialog::instantiated = false;

//-------------------*ChildProc Test *-----------------------
VOID WINAPI MeshRendererDialog::OnChildDialogInit(HWND hwndDlg)
{
	//�θ� �������� ��ġ,ũ��
	HWND hwndParent = GetParent(hwndDlg);
	RECT parentWndRect;
	GetWindowRect(hwndParent, &parentWndRect);
	
	
	float parentWidth = parentWndRect.right - parentWndRect.left;
	float parentHeight = (parentWndRect.bottom - parentWndRect.top);
	SetWindowPos(hwndDlg, HWND_TOP, parentWndRect.left,
		parentWndRect.top + parentHeight * 0.03,//-2,
		parentWidth,
		parentHeight*0.9,
		SWP_SHOWWINDOW);

	return;
}


INT_PTR CALLBACK MeshRendererProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_CLOSE:
		EndDialog(hDlg, 0);
		return (INT_PTR)TRUE;
	case WM_INITDIALOG:
	{
		g_MeshRendererDialog->OnChildDialogInit(hDlg);
		g_MeshRendererDialog->Init(hDlg);

		
		return (INT_PTR)TRUE;
	}

	case WM_COMMAND:
		g_MeshRendererDialog->MenuProc(hDlg, wParam);
		return (INT_PTR)TRUE;
	case WM_KEYDOWN:

		return (INT_PTR)TRUE;
	case WM_MOUSEMOVE:

		return (INT_PTR)TRUE;
	case WM_NOTIFY: //����Ʈ ��Ʈ�� Ŭ������ �� �߻�
		g_MeshRendererDialog->NotifyProc(hDlg, lParam);
		return (INT_PTR)TRUE;
	case WM_MOVE: //
	{

		return (INT_PTR)TRUE;
	}

	}
	return (INT_PTR)FALSE;
}
//-------------------*ChildProc Test *-----------------------



bool MeshRendererDialog::OpenDialog()
{
	return false;
}

bool MeshRendererDialog::SetObject(GameObject* obj)
{
	MeshRenderer* meshRenderer = obj->GetComponent<MeshRenderer>();
	m_MeshRenderer = meshRenderer;
	if (!m_MeshRenderer)
		return false;

	materials = &(m_MeshRenderer->GetMaterials());
	mesh = m_MeshRenderer->GetMesh();

}

bool MeshRendererDialog::UpdateView()
{	
	if (m_MeshRenderer == nullptr)
		return false;

	
	//ASCII -> UNICODE ��ȯ
	USES_CONVERSION;
	LPCWSTR meshName = A2W(mesh->id.c_str());

	//mesh editText�� �̸� ����
	Edit_SetText(GetDlgItem(m_hDlg, IDC_EDIT11), meshName);
			
	int listCount = ListBox_GetCount(m_hList);
	for (int i = 0; i < listCount; i++)
		ListBox_DeleteString(m_hList, i);
	
	for (GeneralMaterial& elem : (*materials))
	{
		int pos = (int)SendMessage(m_hList, LB_ADDSTRING, 0,
			(LPARAM)elem.name.c_str());

		//GeneralMaterial�� �ּҰ��� data�� ����
		SendMessage(m_hList, LB_SETITEMDATA, pos, (LPARAM)&elem);
	}

	return true;

}


bool MeshRendererDialog::OpenDialog(HWND hwnd)
{

	m_hwndParent = hwnd;

	if (!IsWindow(m_hDlg))
	{
		m_hDlg = CreateDialog(m_hInstance, MAKEINTRESOURCE(TAB_MESHRENDERER), hwnd, MeshRendererProc);
		
		ShowWindow(m_hDlg, SW_SHOW);
		return true;
	}

	return false;
}

void MeshRendererDialog::Init(HWND hDlg)
{
	m_hDlg = hDlg;
	m_hList = GetDlgItem(hDlg, IDC_LIST6);
}


MeshRendererDialog::MeshRendererDialog(HINSTANCE hInstance) : ComponentDialog(hInstance), m_MeshRenderer(0)
															,mesh(0), materials(0)
{
	assert(!instantiated);
	instantiated = true;
	g_MeshRendererDialog = this;
}



void MeshRendererDialog::MenuProc(HWND hDlg, WPARAM wParam)
{
	//LOWORD(wParam) = ��Ʈ�� �ĺ�
	int wmId = LOWORD(wParam);

	switch (wmId)
	{
		
	case DIFFUSEMAPBUTTON:
		if (HIWORD(wParam) == BN_CLICKED)
		{
			WCHAR fileTitle[100];
			WCHAR filePath[100];
			std::vector<LPCWSTR> extensions = { L"dds"};
			if (FileOpenDialog(m_hDlg, fileTitle, filePath, extensions))
			{
				int idx = ListBox_GetCurSel(m_hList);
				if (idx == -1)
					break;
				CommandQueue::AddCommand(new SetMaterialMap(&(*materials)[idx], filePath, mapType::Type_DiffuseMap));
				
				//diffuseMapText�� �̸� ����
				Edit_SetText(GetDlgItem(m_hDlg, DIFFUSEMAPCONTROL), fileTitle);
			}
			else
				MessageBox(m_hDlg, L"DiffuseMap Load Fail!", L"DiffuseMap Load", MB_OK);
			
		}
		break;
	case NORMALMAPBUTTON:
		if (HIWORD(wParam) == BN_CLICKED)
		{

		}

		break;
	}


}





void MeshRendererDialog::KeyDownProc(WPARAM wParam)
{

}


void MeshRendererDialog::MouseMoveProc(WPARAM btnState, LPARAM mousePos)
{

}



void MeshRendererDialog::CharProc(HWND hDlg, WPARAM wParam)
{

}

void MeshRendererDialog::NotifyProc(HWND hDlg, LPARAM lParam)
{
	UINT codeTest = ((LPNMHDR)lParam)->code;
	switch (((LPNMHDR)lParam)->code)
	{

		
	case TCN_SELCHANGE:
		
		return;

	}
}