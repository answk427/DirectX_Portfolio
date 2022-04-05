#include "ComponentDialog.h"

bool ComponentDialog::OpenDialog()
{
	return false;
}

void ComponentDialog::MenuProc(HWND hDlg, WPARAM wParam)
{
}

void ComponentDialog::CharProc(HWND hDlg, WPARAM wParam)
{
}

void ComponentDialog::KeyDownProc(WPARAM wParam)
{
}

void ComponentDialog::MouseMoveProc(WPARAM btnState, LPARAM mousePos)
{
}

float ComponentDialog::EditText2Float(HWND h_edit)
{
	WCHAR str[20];
	
	Edit_GetText(h_edit, str, 20);
	
	return _wtof(str);
}
