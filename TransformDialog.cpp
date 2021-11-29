#include "TransformDialog.h"


TransformDialog* g_TransformDialog = 0;
bool TransformDialog::instantiated = false;

//-------------------*ChildProc Test *-----------------------
VOID WINAPI TransformDialog::OnChildDialogInit(HWND hwndDlg)
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


INT_PTR CALLBACK TransformProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_CLOSE:
		EndDialog(hDlg, 0);
		return (INT_PTR)TRUE;
	case WM_INITDIALOG:
	{
		g_TransformDialog->OnChildDialogInit(hDlg);
		g_TransformDialog->Init(hDlg);

		return (INT_PTR)TRUE;
	}

	case WM_COMMAND:

		return (INT_PTR)TRUE;
	case WM_KEYDOWN:

		return (INT_PTR)TRUE;
	case WM_MOUSEMOVE:

		return (INT_PTR)TRUE;
	case WM_NOTIFY: //리스트 컨트롤 클릭했을 때 발생
		g_TransformDialog->NotifyProc(hDlg, lParam);
		return (INT_PTR)TRUE;
	case WM_MOVE: //
	{

		return (INT_PTR)TRUE;
	}

	}
	return (INT_PTR)FALSE;
}
//-------------------*ChildProc Test *-----------------------



bool TransformDialog::OpenDialog()
{
	return false;
}


bool TransformDialog::SetObject(GameObject * obj)
{
	m_transform = &(obj->transform);
	return false;
}

bool TransformDialog::UpdateView()
{
	//ASCII -> UNICODE 변환
	USES_CONVERSION;
	//LPCWSTR meshName = A2W(mesh->id.c_str());

	Edit_SetText(m_hEdit_PosX, std::to_wstring(m_transform->m_position.x).c_str());
	Edit_SetText(m_hEdit_PosY, std::to_wstring(m_transform->m_position.y).c_str());
	Edit_SetText(m_hEdit_PosZ, std::to_wstring(m_transform->m_position.z).c_str());
	
	Edit_SetText(m_hEdit_RotX, std::to_wstring(m_transform->m_rotation.x).c_str());
	Edit_SetText(m_hEdit_RotY, std::to_wstring(m_transform->m_rotation.y).c_str());
	Edit_SetText(m_hEdit_RotZ, std::to_wstring(m_transform->m_rotation.z).c_str());

	Edit_SetText(m_hEdit_ScaleX, std::to_wstring(m_transform->m_scale.x).c_str());
	Edit_SetText(m_hEdit_ScaleY, std::to_wstring(m_transform->m_scale.y).c_str());
	Edit_SetText(m_hEdit_ScaleZ, std::to_wstring(m_transform->m_scale.z).c_str());
	
	//_wtof() 유니코드 문자열 -> 실수 변환
	
	return false;
}

bool TransformDialog::OpenDialog(HWND hwnd)
{

	m_hwndParent = hwnd;

	if (!IsWindow(m_hDlg))
	{
		m_hDlg = CreateDialog(m_hInstance, MAKEINTRESOURCE(TAB_TRANSFORM), hwnd, TransformProc);

		ShowWindow(m_hDlg, SW_SHOW);
		return true;
	}

	return false;
}

void TransformDialog::Init(HWND hDlg)
{
	m_hDlg = hDlg;
	m_hEdit_PosX = GetDlgItem(hDlg, POSITION_X);
	m_hEdit_PosY = GetDlgItem(hDlg, POSITION_Y);
	m_hEdit_PosZ = GetDlgItem(hDlg, POSITION_Z);

	m_hEdit_RotX = GetDlgItem(hDlg, ROTATION_X);
	m_hEdit_RotY = GetDlgItem(hDlg, ROTATION_Y);
	m_hEdit_RotZ = GetDlgItem(hDlg, ROTATION_Z);

	m_hEdit_ScaleX = GetDlgItem(hDlg, SCALE_X);
	m_hEdit_ScaleY = GetDlgItem(hDlg, SCALE_Y);
	m_hEdit_ScaleZ = GetDlgItem(hDlg, SCALE_Z);
}


TransformDialog::TransformDialog(HINSTANCE hInstance) : ComponentDialog(hInstance), m_MeshRenderer(0)
{
	assert(!instantiated);
	instantiated = true;
	g_TransformDialog = this;
}



void TransformDialog::MenuProc(HWND hDlg, WPARAM wParam)
{
	int wmId = LOWORD(wParam);

}





void TransformDialog::KeyDownProc(WPARAM wParam)
{

}


void TransformDialog::MouseMoveProc(WPARAM btnState, LPARAM mousePos)
{

}



void TransformDialog::CharProc(HWND hDlg, WPARAM wParam)
{

}

void TransformDialog::NotifyProc(HWND hDlg, LPARAM lParam)
{
	UINT codeTest = ((LPNMHDR)lParam)->code;
	switch (((LPNMHDR)lParam)->code)
	{

		//탭 컨트롤이 선택됨
	case TCN_SELCHANGE:
		//return OnSelChanged(hDlg);
		return;

	}
}