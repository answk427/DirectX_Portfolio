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
	m_updating = true;
	if (m_Lighting == nullptr)
	{
		m_updating = false;
		return false;
	}
	
	
	const DirectionalLight& dirLight = m_Lighting->GetDirLight();
		
	//ASCII -> UNICODE 변환
	USES_CONVERSION;

	//변경사항이 있으면 edit 박스 수정
	if (!compareRGBA(m_diffuse, dirLight.Diffuse))
	{
		m_diffuse = dirLight.Diffuse;
		Edit_SetText(m_hEdit_DiffuseR, std::to_wstring(m_diffuse.x).c_str());
		Edit_SetText(m_hEdit_DiffuseG, std::to_wstring(m_diffuse.y).c_str());
		Edit_SetText(m_hEdit_DiffuseB, std::to_wstring(m_diffuse.z).c_str());
		Edit_SetText(m_hEdit_DiffuseA, std::to_wstring(m_diffuse.w).c_str());
	}
	if (!compareRGBA(m_ambient, dirLight.Ambient))
	{
		m_ambient = dirLight.Ambient;
		Edit_SetText(m_hEdit_AmbientR, std::to_wstring(m_ambient.x).c_str());
		Edit_SetText(m_hEdit_AmbientG, std::to_wstring(m_ambient.y).c_str());
		Edit_SetText(m_hEdit_AmbientB, std::to_wstring(m_ambient.z).c_str());
		Edit_SetText(m_hEdit_AmbientA, std::to_wstring(m_ambient.w).c_str());
	}
	if (!compareRGBA(m_specular, dirLight.Specular))
	{
		m_specular = dirLight.Specular;
		Edit_SetText(m_hEdit_SpecularR, std::to_wstring(m_specular.x).c_str());
		Edit_SetText(m_hEdit_SpecularG, std::to_wstring(m_specular.y).c_str());
		Edit_SetText(m_hEdit_SpecularB, std::to_wstring(m_specular.z).c_str());
		Edit_SetText(m_hEdit_SpecularA, std::to_wstring(m_specular.w).c_str());
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
}


LightDialog::LightDialog(HINSTANCE hInstance) : ComponentDialog(hInstance), m_Lighting(0), m_updating(0)

{
	assert(!instantiated);
	instantiated = true;
	g_LightDialog = this;
}


void LightDialog::MenuProc(HWND hDlg, WPARAM wParam)
{

	//LOWORD(wParam) = 컨트롤 식별
	int wmId = LOWORD(wParam);


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