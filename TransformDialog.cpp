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
		g_TransformDialog->MenuProc(hDlg,wParam);
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
	//USES_CONVERSION;
	
	m_updating = true;
	

	//변경사항이 있으면 edit 박스 수정
	if (!compareRGB(m_position, m_transform->m_position) || !firstUpdate)
	{
		m_position = m_transform->m_position;
		Edit_SetText(m_hEdit_PosX, std::to_wstring(m_position.x).c_str());
		Edit_SetText(m_hEdit_PosY, std::to_wstring(m_position.y).c_str());
		Edit_SetText(m_hEdit_PosZ, std::to_wstring(m_position.z).c_str());
	}
	if (!compareRGB(m_rotation, m_transform->m_rotation) || !firstUpdate)
	{
		m_rotation = m_transform->m_rotation;
		Edit_SetText(m_hEdit_RotX, std::to_wstring(m_rotation.x).c_str());
		Edit_SetText(m_hEdit_RotY, std::to_wstring(m_rotation.y).c_str());
		Edit_SetText(m_hEdit_RotZ, std::to_wstring(m_rotation.z).c_str());
	}
	if (!compareRGB(m_scale, m_transform->m_scale) || !firstUpdate)
	{
		m_scale = m_transform->m_scale;
		Edit_SetText(m_hEdit_ScaleX, std::to_wstring(m_scale.x).c_str());
		Edit_SetText(m_hEdit_ScaleY, std::to_wstring(m_scale.y).c_str());
		Edit_SetText(m_hEdit_ScaleZ, std::to_wstring(m_scale.z).c_str());


	}
	m_updating = false;

	
	return true;
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

	firstUpdate = false;
	UpdateView();
	firstUpdate = true;
}


TransformDialog::TransformDialog(HINSTANCE hInstance) : ComponentDialog(hInstance), m_MeshRenderer(0),
m_position(0, 0, 0), m_rotation(0, 0, 0), m_scale(1, 1, 1)
{
	assert(!instantiated);
	instantiated = true;
	g_TransformDialog = this;
}



void TransformDialog::MenuProc(HWND hDlg, WPARAM wParam)
{
	int wmId = LOWORD(wParam);

	//컨트롤의 값이 변경 되었을 때
	if (HIWORD(wParam) == EN_CHANGE)
	{
		//프레임마다 호출되는 update View에서 edit box가 변경됨에 따라 Message가 호출된 경우 리턴
		if (m_updating)
			return;

		switch (wmId)
		{
		case POSITION_X:
			m_position.x = EditText2Float(m_hEdit_PosX);
			m_transform->SetPosition(m_position);
			break;
		case POSITION_Y:
			m_position.y = EditText2Float(m_hEdit_PosY);
			m_transform->SetPosition(m_position);
			break;
		case POSITION_Z:
			m_position.z = EditText2Float(m_hEdit_PosZ);
			m_transform->SetPosition(m_position);
			break;
		case ROTATION_X:
			m_rotation.x = EditText2Float(m_hEdit_RotX);
			m_transform->SetRotation(m_rotation);
			break;
		case ROTATION_Y:
			m_rotation.y = EditText2Float(m_hEdit_RotY);
			m_transform->SetRotation(m_rotation);
			break;
		case ROTATION_Z:
			m_rotation.z = EditText2Float(m_hEdit_RotZ);
			m_transform->SetRotation(m_rotation);
			break;
		case SCALE_X:
			m_scale.x = EditText2Float(m_hEdit_ScaleX);
			m_transform->SetScale(m_scale);
			break;
		case SCALE_Y:
			m_scale.y = EditText2Float(m_hEdit_ScaleY);
			m_transform->SetScale(m_scale);
			break;
		case SCALE_Z:
			m_scale.z = EditText2Float(m_hEdit_ScaleZ);
			m_transform->SetScale(m_scale);
			break;
		}
		m_transform->UpdateWorld();

	}
	

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