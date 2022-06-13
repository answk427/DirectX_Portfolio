#pragma once
#include "MsgProcedure.h"

class MainWindowDialog : public MsgProcedure
{
	// MsgProcedure을(를) 통해 상속됨
	virtual bool OpenDialog() override;
	virtual void MenuProc(HWND hDlg, WPARAM wParam) override;
	virtual void CharProc(HWND hDlg, WPARAM wParam) override;
	virtual void KeyDownProc(WPARAM wParam) override;
	virtual void MouseMoveProc(WPARAM btnState, LPARAM mousePos) override;
};
