#pragma once
#include "header.h"

class EditWords
{
public:
	EditWords(void);
	~EditWords(void);

	static INT_PTR CALLBACK DlgProc(HWND hWnd, UINT mes, WPARAM wp, LPARAM lp);
	static EditWords* ptr;

	BOOL Cls_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
	void Cls_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
	void Cls_OnClose(HWND hwnd);

	HWND hDialog, hEditSecondBanWord;
	WCHAR str[MAX_SIZE];
};

