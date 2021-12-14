#include "LightDialog.h"


extern LightDialog* g_LightDialog = 0;
bool LightDialog::instantiated = false;

//-------------------*ChildProc Test *-----------------------
VOID WINAPI LightDialog::OnChildDialogInit(HWND hwndDlg)
{
	//부모 윈도우의 위치,크기
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


INT_PTR CALLBACK LightProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_CLOSE:
		EndDialog(hDlg, 0);
		return (INT_PTR)TRUE;
	case WM_INITDIALOG:
	{
		g_LightDialog->OnChildDialogInit(hDlg);
		g_LightDialog->Init(hDlg);


		return (INT_PTR)TRUE;
	}

	case WM_COMMAND:
		g_LightDialog->MenuProc(hDlg, wParam);
		return (INT_PTR)TRUE;
	case WM_KEYDOWN:

		return (INT_PTR)TRUE;
	case WM_MOUSEMOVE:

		return (INT_PTR)TRUE;
	case WM_NOTIFY: //리스트 컨트롤 클릭했을 때 발생
		g_LightDialog->NotifyProc(hDlg, lParam);
		return (INT_PTR)TRUE;
	case WM_MOVE: //
	{

		return (INT_PTR)TRUE;
	}

	}
	return (INT_PTR)FALSE;
}
//-------------------*ChildProc Test *-----------------------



bool LightDialog::OpenDialog()
{
	return false;
}

bool LightDialog::SetObject(GameObject* obj)
{
	Lighting* lighting = dynamic_cast<Lighting*>(obj->GetComponent<Lighting>());
	m_Lighting = lighting;
	
	if (!m_Lighting)
		return false;
	
}

bool LightDialog::UpdateView()
{
	//if (m_MeshRenderer == nullptr)
	//	return false;


	////ASCII -> UNICODE 변환
	//USES_CONVERSION;
	//LPCWSTR meshName = A2W(mesh->id.c_str());

	////mesh editText에 이름 설정
	//Edit_SetText(GetDlgItem(m_hDlg, IDC_EDIT11), meshName);

	//int listCount = ListBox_GetCount(m_hList);
	//for (int i = 0; i < listCount; i++)
	//	ListBox_DeleteString(m_hList, i);

	//for (GeneralMaterial& elem : (*materials))
	//{
	//	int pos = (int)SendMessage(m_hList, LB_ADDSTRING, 0,
	//		(LPARAM)elem.name.c_str());

	//	//GeneralMaterial의 주소값을 data로 설정
	//	SendMessage(m_hList, LB_SETITEMDATA, pos, (LPARAM)&elem);
	//}

	return true;

}


bool LightDialog::OpenDialog(HWND hwnd)
{

	m_hwndParent = hwnd;

	if (!IsWindow(m_hDlg))
	{
		m_hDlg = CreateDialog(m_hInstance, MAKEINTRESOURCE(TAB_LIGHT), hwnd, LightProc);

		ShowWindow(m_hDlg, SW_SHOW);
		return true;
	}

	return false;
}

void LightDialog::Init(HWND hDlg)
{
	m_hDlg = hDlg;

	m_hEdit_DiffuseR = GetDlgItem(hDlg, DIFFUSE_R);
	m_hEdit_DiffuseG = GetDlgItem(hDlg, DIFFUSE_G);
	m_hEdit_DiffuseB = GetDlgItem(hDlg, DIFFUSE_B);
	m_hEdit_DiffuseA = GetDlgItem(hDlg, DIFFUSE_A);

	m_hEdit_AmbientR = GetDlgItem(hDlg, AMBIENT_R);
	m_hEdit_AmbientG = GetDlgItem(hDlg, AMBIENT_G);
	m_hEdit_AmbientB = GetDlgItem(hDlg, AMBIENT_B);
	m_hEdit_AmbientA = GetDlgItem(hDlg, AMBIENT_A);

	m_hEdit_SpecularR = GetDlgItem(hDlg, SPECULAR_R);
	m_hEdit_SpecularG = GetDlgItem(hDlg, SPECULAR_G);
	m_hEdit_SpecularB = GetDlgItem(hDlg, SPECULAR_B);
	m_hEdit_SpecularA = GetDlgItem(hDlg, SPECULAR_A);
}


LightDialog::LightDialog(HINSTANCE hInstance) : ComponentDialog(hInstance), m_Lighting(0)

{
	assert(!instantiated);
	instantiated = true;
	g_LightDialog = this;
}



void LightDialog::MenuProc(HWND hDlg, WPARAM wParam)
{
	//LOWORD(wParam) = 컨트롤 식별
	int wmId = LOWORD(wParam);
	
}





void LightDialog::KeyDownProc(WPARAM wParam)
{

}


void LightDialog::MouseMoveProc(WPARAM btnState, LPARAM mousePos)
{

}



void LightDialog::CharProc(HWND hDlg, WPARAM wParam)
{

}

void LightDialog::NotifyProc(HWND hDlg, LPARAM lParam)
{
	UINT codeTest = ((LPNMHDR)lParam)->code;
	switch (((LPNMHDR)lParam)->code)
	{


	case TCN_SELCHANGE:

		return;

	}
}