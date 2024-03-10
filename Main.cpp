#include "MainDlg.h"

int WINAPI _tWinMain(HINSTANCE hInst, HINSTANCE hPrev, LPTSTR lpszCmdLine, int nCmdShow)
{
	MainDlg dlg;
	return DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG), NULL, MainDlg::DlgProc);
}