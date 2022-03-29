#pragma once
#include "stdafx.h"


#define C_PAGES 3 
#define sz_Or_Ord WORD
#define titleLen 100
#define stringLen 100



typedef struct {
	WORD      dlgVer;
	WORD      signature;
	DWORD     helpID;
	DWORD     exStyle;
	DWORD     style;
	WORD      cDlgItems;
	short     x;
	short     y;
	short     cx;
	short     cy;
	sz_Or_Ord menu;
	sz_Or_Ord windowClass;
	WCHAR     title[titleLen];
	WORD      pointsize;
	WORD      weight;
	BYTE      italic;
	BYTE      charset;
	WCHAR     typeface[stringLen];
} DLGTEMPLATEEX;

typedef struct tag_dlghdr {
	HWND hwndTab;       // tab control 
	HWND hwndDisplay;   // current child dialog box 
	RECT rcDisplay;     // display rectangle for the tab control 
	DLGTEMPLATEEX *apRes[C_PAGES];

} DLGHDR;