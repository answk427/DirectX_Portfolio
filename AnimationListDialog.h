#pragma once
#include "MsgProcedure.h"
#include "AnimationManager.h"
#include "UtilFunctions.h"

#define ANIMATIONDIALOG IDD_DIALOG1

INT_PTR CALLBACK AniListDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

class AnimationListDialog  
{
public:
	AnimationListDialog(HINSTANCE hinst, HWND hParent,std::string& dest, int& dialogVal);
	// MsgProcedure을(를) 통해 상속됨
	
};