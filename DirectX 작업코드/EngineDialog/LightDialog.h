#pragma once

#include "stdafx.h"
#include <CommCtrl.h>
#include <resource.h>
#include <cassert>


#include "ComponentDialog.h"
#include "GameObject.h"
#include "DialogStructureEX.h"
#include "DialogSizeDefine.h"
#include "Light.h"

#include "MeshRendererCommand.h"



#define TAB_LIGHT IDD_DIALOG7

#define DIFFUSE_R	IDC_EDIT1
#define DIFFUSE_G	IDC_EDIT2
#define DIFFUSE_B	IDC_EDIT3
#define DIFFUSE_A	IDC_EDIT10

#define AMBIENT_R	IDC_EDIT4
#define AMBIENT_G	IDC_EDIT5
#define AMBIENT_B	IDC_EDIT6
#define AMBIENT_A	IDC_EDIT12

#define SPECULAR_R	IDC_EDIT7
#define SPECULAR_G	IDC_EDIT8
#define SPECULAR_B	IDC_EDIT9
#define SPECULAR_A	IDC_EDIT13

#define COMBO_LIGHTTYPE IDC_COMBO1

#define DIRECTION_X IDC_EDIT14
#define DIRECTION_Y IDC_EDIT15
#define DIRECTION_Z IDC_EDIT16

#define EDIT_RANGE IDC_EDIT17
#define EDIT_SPOT IDC_EDIT18

#define ATT_X IDC_EDIT19
#define ATT_Y IDC_EDIT20
#define ATT_Z IDC_EDIT21




INT_PTR CALLBACK LightProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

class LightDialog : public ComponentDialog
{
public:
	static bool instantiated;

public:
	LightDialog(HINSTANCE hInstance);
public:
	bool OpenDialog(HWND hwnd);
	void Init(HWND hDlg);

private:
	bool firstUpdate;
	XMFLOAT4 m_diffuse;
	XMFLOAT4 m_specular;
	XMFLOAT4 m_ambient;
	XMFLOAT3 m_direction;
	XMFLOAT3 m_att;
	
private:
	Lighting* m_Lighting;

	//Edit Box Handles
	HWND m_hEdit_DiffuseR;
	HWND m_hEdit_DiffuseG;
	HWND m_hEdit_DiffuseB;
	HWND m_hEdit_DiffuseA;

	HWND m_hEdit_AmbientR;
	HWND m_hEdit_AmbientG;
	HWND m_hEdit_AmbientB;
	HWND m_hEdit_AmbientA;

	HWND m_hEdit_SpecularR;
	HWND m_hEdit_SpecularG;
	HWND m_hEdit_SpecularB;
	HWND m_hEdit_SpecularA;

	HWND m_hCombo_LightType;
	HWND m_hEdit_DirectionX;
	HWND m_hEdit_DirectionY;
	HWND m_hEdit_DirectionZ;
	
	HWND m_hEdit_Range;
	HWND m_hEdit_Spot;
	HWND m_hEdit_AttX;
	HWND m_hEdit_AttY;
	HWND m_hEdit_AttZ;

public:
	//메뉴눌렸을때 WM_COMMAND 
	virtual void MenuProc(HWND hDlg, WPARAM wParam) override;

	virtual void KeyDownProc(WPARAM wParam) override;

	virtual void MouseMoveProc(WPARAM btnState, LPARAM mousePos) override;

	virtual void CharProc(HWND hDlg, WPARAM wParam) override;

	void NotifyProc(HWND hDlg, LPARAM lParam);

	VOID WINAPI OnChildDialogInit(HWND hwndDlg);

private:
	// MsgProcedure을(를) 통해 상속됨
	virtual bool OpenDialog() override;//Tab의 자식 Dialog로 생성해야 하므로 사용하지 않는 함수

public:
	// ComponentDialog을(를) 통해 상속됨
	virtual bool UpdateView() override;


	// ComponentDialog을(를) 통해 상속됨
	virtual bool SetObject(GameObject * obj) override;

};


