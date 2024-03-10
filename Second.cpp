#include "Second.h"
#include "MainDlg.h"

EditWords* EditWords::ptr = NULL;
EditWords::EditWords(void)
{
	ptr = this;
}
EditWords::~EditWords(void)
{

}

void EditWords::Cls_OnClose(HWND hwnd)
{
	EndDialog(hwnd, IDCANCEL);
}

BOOL EditWords::Cls_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	hDialog = hwnd;
	hEditSecondBanWord = GetDlgItem(hDialog, IDC_EDIT_SECOND_BAN_WORD);
	SetWindowText(hEditSecondBanWord, str);
	return TRUE;
}

void EditWords::Cls_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	if (id == IDC_BTN_OK)
	{
		GetWindowText(hEditSecondBanWord, str, sizeof(str));
		if (lstrlen(str))
		{
			EndDialog(hwnd, IDOK);
		}
		else
			MessageBox(hwnd, TEXT("Empty changed word field"), TEXT("Error"), MB_OK | MB_ICONERROR);
	}
	else if (id == IDC_BTN_CANCEL)
	{
		EndDialog(hwnd, IDCANCEL);
	}
}

INT_PTR CALLBACK EditWords::DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		HANDLE_MSG(hwnd, WM_CLOSE, ptr->Cls_OnClose);
		HANDLE_MSG(hwnd, WM_INITDIALOG, ptr->Cls_OnInitDialog);
		HANDLE_MSG(hwnd, WM_COMMAND, ptr->Cls_OnCommand);
	}
	return FALSE;
}
