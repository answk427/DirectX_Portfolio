#include "MeshRendererDialog.h"


MeshRendererDialog* g_MeshRendererDialog = 0;
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

void MeshRendererDialog::UpdateView(MeshRenderer* meshRenderer)
{
	const Mesh* mesh = meshRenderer->GetMesh();

	//ASCII -> UNICODE ��ȯ
	USES_CONVERSION;
	LPCWSTR meshName = A2W(mesh->id.c_str);
	
	//mesh editText�� �̸� ����
	Edit_SetText(GetDlgItem(m_hDlg, IDC_EDIT11), meshName);

	const auto materials = meshRenderer->GetMaterials();
	
	//material Manager�� ���� material ����Ʈ�� �߰��ؾ��Ұ� ����.
	//material�� id�ο�
	
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

		//�� ��Ʈ���� ���õ�
	case TCN_SELCHANGE:
		//return OnSelChanged(hDlg);
		return;

	}
}