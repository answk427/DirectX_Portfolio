#pragma once
#include "MsgProcedure.h"
#include "GameObject.h"

typedef std::pair<HWND, float*> handleFloatPair;



class ComponentDialog : public MsgProcedure
{
public:
	HWND m_hDlg; //dialog	
	HWND m_hwndParent; //main window
public:
	ComponentDialog(HINSTANCE hInstance) : MsgProcedure(hInstance), m_updating(0),
	firstUpdate(0) {}
public:
	// MsgProcedure을(를) 통해 상속됨
	virtual bool OpenDialog() override;
	
private:
	virtual void MenuProc(HWND hDlg, WPARAM wParam) override;
	virtual void CharProc(HWND hDlg, WPARAM wParam) override;
	virtual void KeyDownProc(WPARAM wParam) override;
	virtual void MouseMoveProc(WPARAM btnState, LPARAM mousePos) override;
protected:
	bool m_updating;
	bool firstUpdate;
public:
	virtual bool SetObject(GameObject* obj) = 0;
	virtual bool UpdateView() = 0;
	virtual bool OpenDialog(HWND hwnd) = 0;
	
public:
	float EditText2Float(HWND h_edit);
};
