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


	LONG parentWidth = parentWndRect.right - parentWndRect.left;
	LONG parentHeight = (parentWndRect.bottom - parentWndRect.top);
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
	m_updating = true;
	if (m_Lighting == nullptr)
	{
		m_updating = false;
		return false;
	}
	
	
	const DirectionalLight& dirLight = m_Lighting->GetDirLight();
	const SpotLight& spotLight = m_Lighting->GetSpotLight();
		
	//ASCII -> UNICODE 변환
	USES_CONVERSION;

	//변경사항이 있으면 edit 박스 수정
	if (!compareRGBA(m_diffuse, dirLight.Diffuse) || !firstUpdate)
	{
		m_diffuse = dirLight.Diffuse;
		Edit_SetText(m_hEdit_DiffuseR, std::to_wstring(m_diffuse.x).c_str());
		Edit_SetText(m_hEdit_DiffuseG, std::to_wstring(m_diffuse.y).c_str());
		Edit_SetText(m_hEdit_DiffuseB, std::to_wstring(m_diffuse.z).c_str());
		Edit_SetText(m_hEdit_DiffuseA, std::to_wstring(m_diffuse.w).c_str());
	}
	if (!compareRGBA(m_ambient, dirLight.Ambient) || !firstUpdate)
	{
		m_ambient = dirLight.Ambient;
		Edit_SetText(m_hEdit_AmbientR, std::to_wstring(m_ambient.x).c_str());
		Edit_SetText(m_hEdit_AmbientG, std::to_wstring(m_ambient.y).c_str());
		Edit_SetText(m_hEdit_AmbientB, std::to_wstring(m_ambient.z).c_str());
		Edit_SetText(m_hEdit_AmbientA, std::to_wstring(m_ambient.w).c_str());
	}
	if (!compareRGBA(m_specular, dirLight.Specular) || !firstUpdate)
	{
		m_specular = dirLight.Specular;
		Edit_SetText(m_hEdit_SpecularR, std::to_wstring(m_specular.x).c_str());
		Edit_SetText(m_hEdit_SpecularG, std::to_wstring(m_specular.y).c_str());
		Edit_SetText(m_hEdit_SpecularB, std::to_wstring(m_specular.z).c_str());
		Edit_SetText(m_hEdit_SpecularA, std::to_wstring(m_specular.w).c_str());
	}
	if (!compareRGB(m_direction, dirLight.Direction) || !firstUpdate)
	{
		m_direction = dirLight.Direction;
		Edit_SetText(m_hEdit_DirectionX, std::to_wstring(m_direction.x).c_str());
		Edit_SetText(m_hEdit_DirectionY, std::to_wstring(m_direction.y).c_str());
		Edit_SetText(m_hEdit_DirectionZ, std::to_wstring(m_direction.z).c_str());
	}
	if (!compareRGB(m_att, spotLight.Att) || !firstUpdate)
	{
		m_att = spotLight.Att;
		Edit_SetText(m_hEdit_AttX, std::to_wstring(m_att.x).c_str());
		Edit_SetText(m_hEdit_AttY, std::to_wstring(m_att.y).c_str());
		Edit_SetText(m_hEdit_AttZ, std::to_wstring(m_att.z).c_str());
	}
	if (!firstUpdate)
	{
		Edit_SetText(m_hEdit_Range, std::to_wstring(spotLight.Range).c_str());
		Edit_SetText(m_hEdit_Spot, std::to_wstring(spotLight.Spot).c_str());
		ComboBox_SetCurSel(m_hCombo_LightType, m_Lighting->GetLightType());
	}
		
	m_updating = false;

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

	m_hCombo_LightType = GetDlgItem(hDlg, COMBO_LIGHTTYPE);
	m_hEdit_DirectionX = GetDlgItem(hDlg, DIRECTION_X);
	m_hEdit_DirectionY = GetDlgItem(hDlg, DIRECTION_Y);
	m_hEdit_DirectionZ = GetDlgItem(hDlg, DIRECTION_Z);

	m_hEdit_Range = GetDlgItem(hDlg, EDIT_RANGE);
	m_hEdit_Spot = GetDlgItem(hDlg, EDIT_SPOT);
	m_hEdit_AttX = GetDlgItem(hDlg, ATT_X);
	m_hEdit_AttY = GetDlgItem(hDlg, ATT_Y);
	m_hEdit_AttZ = GetDlgItem(hDlg, ATT_Z);

	//콤보박스 아이템 추가
	ComboBox_AddString(m_hCombo_LightType, TEXT("DIRECTIONAL"));
	ComboBox_AddString(m_hCombo_LightType, TEXT("POINT"));
	ComboBox_AddString(m_hCombo_LightType, TEXT("SPOT"));
	

	firstUpdate = false;
	UpdateView();
	firstUpdate = true;
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

	//콤보박스 변경
	if (HIWORD(wParam) == CBN_SELCHANGE)
	{
		switch (wmId)
		{
		case COMBO_LIGHTTYPE:
			m_Lighting->SetLightType(
				(LightType)ComboBox_GetCurSel(m_hCombo_LightType));
			break;
		}
		
	}

	if (HIWORD(wParam) == EN_CHANGE)
	{
		//프레임마다 호출되는 update View에서 edit box가 변경됨에 따라 Message가 호출된 경우 리턴
		if (m_updating)
			return;

		switch (wmId)
		{
		case DIFFUSE_R:
			m_diffuse.x = EditText2Float(m_hEdit_DiffuseR);
			m_Lighting->SetDiffuse(m_diffuse);
			break;
		case DIFFUSE_G:
			m_diffuse.y = EditText2Float(m_hEdit_DiffuseG);
			m_Lighting->SetDiffuse(m_diffuse);
			break;
		case DIFFUSE_B:
			m_diffuse.z = EditText2Float(m_hEdit_DiffuseB);
			m_Lighting->SetDiffuse(m_diffuse);
			break;
		case DIFFUSE_A:
			m_diffuse.w = EditText2Float(m_hEdit_DiffuseA);
			m_Lighting->SetDiffuse(m_diffuse);
			break;
		case AMBIENT_R:
			m_ambient.x = EditText2Float(m_hEdit_AmbientR);
			m_Lighting->SetAmbient(m_ambient);
			break;
		case AMBIENT_G:
			m_ambient.y = EditText2Float(m_hEdit_AmbientG);
			m_Lighting->SetAmbient(m_ambient);
			break;
		case AMBIENT_B:
			m_ambient.z = EditText2Float(m_hEdit_AmbientB);
			m_Lighting->SetAmbient(m_ambient);
			break;
		case AMBIENT_A:
			m_ambient.w = EditText2Float(m_hEdit_AmbientA);
			m_Lighting->SetAmbient(m_ambient);
			break;
		case SPECULAR_R:
			m_specular.x = EditText2Float(m_hEdit_SpecularR);
			m_Lighting->SetSpecular(m_specular);
			break;
		case SPECULAR_G:
			m_specular.y = EditText2Float(m_hEdit_SpecularG);
			m_Lighting->SetSpecular(m_specular);
			break;
		case SPECULAR_B:
			m_specular.z = EditText2Float(m_hEdit_SpecularB);
			m_Lighting->SetSpecular(m_specular);
			break;
		case SPECULAR_A:
			m_specular.z = EditText2Float(m_hEdit_SpecularA);
			m_Lighting->SetSpecular(m_specular);
			break;
		
		case DIRECTION_X:
			m_direction.x = EditText2Float(m_hEdit_DirectionX);
			m_Lighting->SetDirection(m_direction);
			break;
		case DIRECTION_Y:
			m_direction.y = EditText2Float(m_hEdit_DirectionY);
			m_Lighting->SetDirection(m_direction);
			break;
		case DIRECTION_Z:
			m_direction.z = EditText2Float(m_hEdit_DirectionZ);
			m_Lighting->SetDirection(m_direction);
			break;
		case EDIT_RANGE:
			m_Lighting->SetRange(EditText2Float(m_hEdit_Range));
			break;
		case EDIT_SPOT:
			m_Lighting->SetSpot(EditText2Float(m_hEdit_Spot));
			break;
		case ATT_X:
			m_att.x = EditText2Float(m_hEdit_AttX);
			m_Lighting->SetAtt(m_att);
			break;
		case ATT_Y:
			m_att.y = EditText2Float(m_hEdit_AttY);
			m_Lighting->SetAtt(m_att);
			break;
		case ATT_Z:
			m_att.z = EditText2Float(m_hEdit_AttZ);
			m_Lighting->SetAtt(m_att);
			break;
		}
		

	}
	
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