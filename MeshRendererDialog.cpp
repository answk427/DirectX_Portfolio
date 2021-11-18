#include "MeshRendererDialog.h"


MeshRendererDialog* g_MeshRendererDialog = 0;
bool MeshRendererDialog::instantiated = false;

//-------------------*ChildProc Test *-----------------------
VOID WINAPI MeshRendererDialog::OnChildDialogInit(HWND hwndDlg)
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

void MeshRendererDialog::UpdateView(MeshRenderer* meshRenderer)
{
	const Mesh* mesh = meshRenderer->GetMesh();

	//ASCII -> UNICODE 변환
	USES_CONVERSION;
	LPCWSTR meshName = A2W(mesh->id.c_str);
	
	//mesh editText에 이름 설정
	Edit_SetText(GetDlgItem(m_hDlg, IDC_EDIT11), meshName);

	const auto materials = meshRenderer->GetMaterials();
	
	//material Manager를 만들어서 material 리스트에 추가해야할것 같음.
	//material에 id부여
	
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
		
		return (INT_PTR)TRUE;
	}

	case WM_COMMAND:
		
		return (INT_PTR)TRUE;
	case WM_KEYDOWN:

		return (INT_PTR)TRUE;
	case WM_MOUSEMOVE:

		return (INT_PTR)TRUE;
	case WM_NOTIFY: //리스트 컨트롤 클릭했을 때 발생
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
bool MeshRendererDialog::OpenDialog(HWND hwnd)
{

	m_hwndParent = hwnd;

	if (!IsWindow(m_hDlg))
	{
		m_hDlg = CreateDialog(m_hInstance, MAKEINTRESOURCE(TAB_MESHRENDERER), 0, MeshRendererProc);

		ShowWindow(m_hDlg, SW_SHOW);
		return true;
	}

	return false;
}


MeshRendererDialog::MeshRendererDialog(HINSTANCE hInstance) : MsgProcedure(hInstance), m_MeshRenderer(0)
{
	assert(!instantiated);
	instantiated = true;
	g_MeshRendererDialog = this;
}



void MeshRendererDialog::MenuProc(HWND hDlg, WPARAM wParam)
{
	int wmId = LOWORD(wParam);

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

		//탭 컨트롤이 선택됨
	case TCN_SELCHANGE:
		//return OnSelChanged(hDlg);
		return;

	}
}