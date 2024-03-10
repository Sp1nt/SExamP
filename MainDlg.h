#pragma once
#include "header.h"

class MainDlg
{
public:
	static MainDlg* ptr;
	
	MainDlg(void);
	static INT_PTR CALLBACK DlgProc(HWND hWnd, UINT mes, WPARAM wp, LPARAM lp);
	BOOL Cls_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
	void Cls_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
	void Cls_OnClose(HWND hwnd);
	
	void LoadBannedWords();

	void AddFile();
	void DelFile();

	void AddBandWord();
	void DelBanWord();
	void EditBanWord();

	void RefreshList();
	void CheckerRepeating(WCHAR* szPathFile);


	void Start();

	void ShowN();

	BOOL isWork;

	HWND hDialog;
	HWND hFiles, hBanWords;
	HWND hEditBanWords;
	HWND hBtnUploadF, hBtnDelF, hBtnAddW, hBtnEditnW, hBtnDelW, hBtnStart, hBtnPause, hBtnStop;
	HWND hProgressBar;

	vector <wstring> FileNamw;
	vector <int> BanWordN;
	vector <wstring> BanWord;



	HANDLE hThread;

};
