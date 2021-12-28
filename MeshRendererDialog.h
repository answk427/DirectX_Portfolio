#pragma once

#include "stdafx.h"
#include <CommCtrl.h>
#include <resource.h>
#include <cassert>


#include "ComponentDialog.h"
#include "GameObject.h"
#include "DialogStructureEX.h"
#include "DialogSizeDefine.h"
#include "Renderer.h"

#include "MeshRendererCommand.h"


#define TAB_MESHRENDERER IDD_DIALOG4

#define DIFFUSEMAPCONTROL IDC_EDIT1
#define NORMALMAPCONTROL IDC_EDIT12

#define DIFFUSEMAPBUTTON IDC_BUTTON1
#define NORMALMAPBUTTON IDC_BUTTON3

#define MATERIALLIST IDC_LIST6

#define DIFFUSETILEX_EDIT IDC_EDIT3
#define DIFFUSETILEY_EDIT IDC_EDIT4
#define DIFFUSEOFFSETX_EDIT IDC_EDIT5
#define DIFFUSEOFFSETY_EDIT IDC_EDIT6

#define NORMALTILEX_EDIT IDC_EDIT7
#define NORMALTILEY_EDIT IDC_EDIT8
#define NORMALOFFSETX_EDIT IDC_EDIT9
#define NORMALOFFSETY_EDIT IDC_EDIT10

#define BOUNDMINX_EDIT IDC_EDIT13
#define BOUNDMINY_EDIT IDC_EDIT14
#define BOUNDMINZ_EDIT IDC_EDIT17

#define BOUNDMAXX_EDIT IDC_EDIT15
#define BOUNDMAXY_EDIT IDC_EDIT16
#define BOUNDMAXZ_EDIT IDC_EDIT18



INT_PTR CALLBACK MeshRendererProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

class MeshRendererDialog : public ComponentDialog
{
public:
	static bool instantiated;
public:
	HWND m_hList; //list view �ڵ�
public:
	MeshRendererDialog(HINSTANCE hInstance);	
public:
	bool OpenDialog(HWND hwnd);
	void Init(HWND hDlg);
private:
	//control ID�� �ش��ϴ� �ڵ�, �����ؾߵ� �� ������
	std::map<int, handleFloatPair> controlMap;

	//Material Map ��� EditBox
	HWND m_hDiffuseTileX;
	HWND m_hDiffuseTileY;
	HWND m_hDiffuseOffsetX;
	HWND m_hDiffuseOffsetY;
	HWND m_hNormalTileX;
	HWND m_hNormalTileY;
	HWND m_hNormalOffsetX;
	HWND m_hNormalOffsetY;

	HWND m_BoundMinX;
	HWND m_BoundMinY;
	HWND m_BoundMinZ;
	HWND m_BoundMaxX;
	HWND m_BoundMaxY;
	HWND m_BoundMaxZ;



private:
	MeshRenderer* m_MeshRenderer;
	std::vector<GeneralMaterial>* materials;
	const Mesh* mesh;

private:
	void MapEditBoxUpdate(int materialIdx);
public:
	//�޴��������� WM_COMMAND 
	virtual void MenuProc(HWND hDlg, WPARAM wParam) override;

	virtual void KeyDownProc(WPARAM wParam) override;

	virtual void MouseMoveProc(WPARAM btnState, LPARAM mousePos) override;

	virtual void CharProc(HWND hDlg, WPARAM wParam) override;
	
	void NotifyProc(HWND hDlg, LPARAM lParam);

	VOID WINAPI OnChildDialogInit(HWND hwndDlg);

private:
	// MsgProcedure��(��) ���� ��ӵ�
	virtual bool OpenDialog() override;//Tab�� �ڽ� Dialog�� �����ؾ� �ϹǷ� ������� �ʴ� �Լ�

public:
	// ComponentDialog��(��) ���� ��ӵ�
	virtual bool UpdateView() override;


	// ComponentDialog��(��) ���� ��ӵ�
	virtual bool SetObject(GameObject * obj) override;

};



