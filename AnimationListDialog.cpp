#include "AnimationListDialog.h"

AnimationListDialog::AnimationListDialog(HINSTANCE hinst, HWND hParent, std::string & dest, int& dialogVal)
{
	dialogVal = DialogBox(hinst, MAKEINTRESOURCE(ANIMATIONDIALOG), hParent, AniListDialogProc);
	
}


INT_PTR CALLBACK AniListDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_CLOSE:
	{
		static std::string clipName;
		
		int idx = ((int)(DWORD)SNDMSG(GetDlgItem(hDlg, IDC_LIST1), LB_GETCURSEL, 0L, 0L));
		//리스트에서 선택된 애니메이션 클립의 이름을 저장
		//static CHAR clipName[256];
		WCHAR nameBuffer[256];
		if (idx != -1)
		{
			ListBox_GetText(GetDlgItem(hDlg, IDC_LIST1), idx,
				nameBuffer);
			clipName = ConvertSTR(nameBuffer);
			
			HWND hParent = GetParent(hDlg);
			SendMessage(hParent, WM_COMMAND, 0104775, (LPARAM)&clipName);
		}
		
		EndDialog(hDlg, 0);
		return (INT_PTR)TRUE;
	}
	case WM_INITDIALOG:
	{
		AnimationManager& aniManager = AnimationManager::Instance();
		HWND hList = GetDlgItem(hDlg, IDC_LIST1);
		WCHAR clipName[256];
		for (auto& clip : aniManager.mAnimations)
		{
			wcscpy(clipName, ConvertWSTR(clip.first).c_str());
			ListBox_AddString(hList, clipName);
		}
		return (INT_PTR)TRUE;
	}

	case WM_COMMAND:
		return (INT_PTR)TRUE;
	case WM_KEYDOWN:

		return (INT_PTR)TRUE;
	case WM_MOUSEMOVE:

		return (INT_PTR)TRUE;
	case WM_NOTIFY: //리스트 컨트롤 클릭했을 때 발생
		return (INT_PTR)TRUE;
	case WM_MOVE: //
	{
		return (INT_PTR)TRUE;
	}


	}
	return (INT_PTR)FALSE;
}