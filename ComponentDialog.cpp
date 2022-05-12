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

void ComponentDialog::PrintBitMap(HDC hdc, const std::wstring & filePath, LPRECT controlRect)
{
	static PAINTSTRUCT ps;
	static HDC hMemDC;
	static HBITMAP hImage, hOldBitmap;
	static BITMAP bit;
		
	hMemDC = CreateCompatibleDC(hdc);
	/*hImage = (HBITMAP)LoadImage(NULL, filePath.c_str(),
		IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);*/
	hImage = (HBITMAP)LoadImage(NULL, filePath.c_str(),
		IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_LOADFROMFILE);

	if (hImage == NULL)
	{
		DWORD lastError = GetLastError();
		return;
	}

	hOldBitmap = (HBITMAP)SelectObject(hMemDC, hImage);

	// ��Ʈ�� ������ �˾Ƴ���
	GetObject(hImage, sizeof(BITMAP), &bit);
	LONG bx = bit.bmWidth;
	LONG by = bit.bmHeight;
	

	//�Ű������� ���� control�� ũ�⿡ ���� ��Ʈ�� ���
	StretchBlt(hdc, controlRect->left, controlRect->top,
		controlRect->right-controlRect->left,
		controlRect->bottom-controlRect->top,
		hMemDC, 0, 0, bx, by, SRCCOPY);
	//StretchBlt(hdc, 0, 0,
	//	controlRect->right - controlRect->left,
	//	controlRect->bottom - controlRect->top,
	//	hMemDC, 0, 0, bx, by, SRCCOPY);

	SelectObject(hMemDC, hOldBitmap); // hImage ������ �����ϱ� ���� hOldBitmap�� �����Ѵ�

	DeleteObject(hImage); // ���� ������ ��Ʈ���� �����Ѵ�

	DeleteDC(hMemDC); // �޸� DC�� �����Ѵ�

}

void ComponentDialog::Screen2Client(HWND hwnd, LPRECT controlRect)
{
	POINT leftPt;
	leftPt.x = controlRect->left;
	leftPt.y = controlRect->top;
	ScreenToClient(hwnd, &leftPt);

	POINT rightPt;
	rightPt.x = controlRect->right;
	rightPt.y = controlRect->bottom;
	ScreenToClient(hwnd, &rightPt);

	controlRect->left = leftPt.x;
	controlRect->top = leftPt.y;
	controlRect->right = rightPt.x;
	controlRect->bottom = rightPt.y;
}

bool ComponentDialog::PrintCImage(HDC hdc, const std::wstring & filePath, LPRECT controlRect)
{
	SetStretchBltMode(hdc, HALFTONE); //// HALFTONE���� �����ؼ� ũ�� ��ҷ� ���� �̹��� ���� ���� �ּ�ȭ
	CImage cimage;
	cimage.Load(filePath.c_str());
	if (cimage.IsNull())
		return false;

	return cimage.Draw(hdc, controlRect->left, controlRect->top,
		controlRect->right - controlRect->left,
		controlRect->bottom - controlRect->top);

	
}

std::wstring ComponentDialog::ExtractTitle(const std::wstring & FilePath)
{
	//diffuse, normal map�� �̸� ����
	int rFirstdot = FilePath.rfind(L'.');
	if (rFirstdot != -1)
	{
		int rFirstSlash = FilePath.rfind(L'\\', rFirstdot);
		return FilePath.substr(rFirstSlash + 1);
	}
	return std::wstring(L"NULL");
}

