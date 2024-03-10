#include "MainDlg.h"
#include "Second.h"


CRITICAL_SECTION cs;



MainDlg* MainDlg::ptr = NULL;

WCHAR PathBanWords[] = TEXT("Ban.txt");
WCHAR EncryptedReplace[] = TEXT("*******");

MainDlg::MainDlg(void)
{
	InitializeCriticalSection(&cs);
	ptr = this;
}
void MainDlg::Cls_OnClose(HWND hwnd)
{

	wofstream in(PathBanWords);
	if (!in)
	{
		return;
	}

	for (int i = 0; i < BanWord.size(); i++)
	{
		in << BanWord[i] << endl;
	}
	in.close();

	CloseHandle(hThread);

	EndDialog(hwnd, 0);
}

BOOL MainDlg::Cls_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	hDialog = hwnd;

	HANDLE hMutex = CreateMutex(NULL, FALSE, TEXT("{1DA61A47-7F47-45FC-A4D1-0553526A3246}"));
	DWORD dwAnswer = WaitForSingleObject(hMutex, 0);
	if (dwAnswer == WAIT_TIMEOUT)
	{
		MessageBox(hDialog, TEXT("Невозможно запустить более одной копии приложения."), TEXT("ОШИБКА"), MB_OK | MB_ICONERROR);
		EndDialog(hDialog, 0);
	}

	//list
	hFiles = GetDlgItem(hDialog, IDC_LIST_FILE_TEXT);
	hBanWords = GetDlgItem(hDialog, IDC_LISTBAN);

	//edit
	hEditBanWords = GetDlgItem(hDialog, IDC_EDIT_ADD_BAN);

	//progress bar
	hProgressBar = GetDlgItem(hDialog, IDC_PROGRESS_BAR);

	//button
	hBtnUploadF = GetDlgItem(hDialog, IDC_BTN_UPLOAD_FILE);
	hBtnDelF = GetDlgItem(hDialog, IDC_BTN_DEL_FILE);
	hBtnAddW = GetDlgItem(hDialog, IDC_BTN_ADD_BAN);
	hBtnEditnW = GetDlgItem(hDialog, IDC_BTN_EDIT_WORD);
	hBtnDelW = GetDlgItem(hDialog, IDC_BTN_DEL_BAN);
	hBtnStart = GetDlgItem(hDialog, IDC_BTN_START);
	hBtnPause = GetDlgItem(hDialog, IDC_BTN_PAUSE);
	hBtnStop = GetDlgItem(hDialog, IDC_BTN_STOP);

	EnableWindow(hBtnPause, FALSE);
	EnableWindow(hBtnStop, FALSE);

	SendMessage(hProgressBar, PBM_SETRANGE, 0, MAKELPARAM(0, 200));
	SendMessage(hProgressBar, PBM_SETSTEP, 1, 0);
	SendMessage(hProgressBar, PBM_SETPOS, 0, 0);
	SendMessage(hProgressBar, PBM_SETBKCOLOR, 0, LPARAM(RGB(0, 0, 255)));
	SendMessage(hProgressBar, PBM_SETBARCOLOR, 0, LPARAM(RGB(255, 255, 0)));

	LoadBannedWords();

	return TRUE;
}

DWORD WINAPI Thread(LPVOID lp)
{
	MainDlg* dialog = (MainDlg*)lp;
	WCHAR exePath[MAX_PATH];

	GetModuleFileName(NULL, exePath, MAX_PATH);
	wstring executablePath = exePath;

	size_t lastBackslashPos = executablePath.find_last_of(TEXT("\\"));
	wstring resultDirectory = TEXT("C:\\Users\\Даниил\\Desktop\\Exam\\Result\\");

	for (int i = 0; i < dialog->FileNamw.size(); ++i)
	{
		wstring line;
		char buffer[MAX_SIZE];
		WCHAR wideBuffer[MAX_SIZE];
		int lastSlashPos = dialog->FileNamw[i].find_last_of(TEXT("\\"));
		int dotPos = dialog->FileNamw[i].find(TEXT("."));
		wstring fileName = dialog->FileNamw[i].substr(lastSlashPos + 1, dotPos - lastSlashPos - 1);

		wstring saveFilePath = resultDirectory + fileName + TEXT(".txt");

		wofstream write(saveFilePath, ios::out);
		ifstream read(dialog->FileNamw[i], ios::in);
		if (!read)
		{
			MessageBox(0, TEXT("Не могу открыть файл"), TEXT("ИНФО"), MB_OK | MB_ICONINFORMATION);
			continue;
		}

		while (!read.eof())
		{
			for (int j = 0; j < dialog->BanWord.size(); ++j)
			{
				read.getline(buffer, MAX_SIZE);
				MultiByteToWideChar(CP_UTF8, 0, buffer, -1, wideBuffer, MAX_SIZE);
				line = wideBuffer;

				int index = 0;
				while ((index = line.find(dialog->BanWord[j], index)) != wstring::npos)
				{
					dialog->BanWordN[j]++;
					line.replace(index, dialog->BanWord[j].length(), EncryptedReplace);
					index += lstrlen(EncryptedReplace);
				}
				write << line << endl;
			}
		}
		write.close();
		read.close();

		SendMessage(dialog->hProgressBar, PBM_STEPIT, 0, 0);
		Sleep(2000);
	}
	
	dialog->ShowN();

	SendMessage(dialog->hProgressBar, PBM_SETRANGE, 0, MAKELPARAM(0, 100));
	SendMessage(dialog->hProgressBar, PBM_SETSTEP, 20, 0);
	SendMessage(dialog->hProgressBar, PBM_SETPOS, 0, 0);
	SendMessage(dialog->hProgressBar, PBM_SETBKCOLOR, 0, LPARAM(RGB(0, 0, 255)));
	SendMessage(dialog->hProgressBar, PBM_SETBARCOLOR, 0, LPARAM(RGB(255, 255, 0)));

	EnableWindow(dialog->hBtnStart, TRUE);
	EnableWindow(dialog->hBtnPause, FALSE);
	EnableWindow(dialog->hBtnStop, FALSE);
	return 0;
}

void MainDlg::AddFile()
{
	WCHAR szPathFile[MAX_PATH] = { 0 };
	OPENFILENAME open = { sizeof(OPENFILENAME) };
	open.hwndOwner = hDialog;
	open.lpstrFilter = TEXT("Text Files(*.txt)\0*.txt");
	open.lpstrFile = szPathFile;
	open.nMaxFile = MAX_PATH;
	open.Flags = OFN_CREATEPROMPT | OFN_PATHMUSTEXIST;

	if (GetOpenFileName(&open))
	{
		ifstream in(szPathFile, ios::in);
		if (!in)
		{
			MessageBox(hDialog, TEXT("Не могу открыть файл"), TEXT("ОШИБКА"), MB_OK | MB_ICONERROR);
			return;
		}
		in.close();

		CheckerRepeating(szPathFile);
	}
}

void MainDlg::LoadBannedWords()
{
	ifstream in(PathBanWords);
	if (!in)
	{
		MessageBox(hDialog, TEXT("Не могу открыть файл с запрещенными словами"), TEXT("ОШИБКА"), MB_OK | MB_ICONERROR);
		return;
	}

	char buf[MAX_SIZE];
	WCHAR wbuf[MAX_SIZE];
	while (!in.eof())
	{
		in.getline(buf, MAX_SIZE);
		if (strlen(buf) != 0)
		{
			MultiByteToWideChar(CP_UTF8, 0, buf, -1, wbuf, MAX_SIZE);
			BanWord.push_back(wbuf);
		}
	}
	in.close();

	RefreshList();
}




void MainDlg::DelFile()
{
	int currentWord = SendMessage(hFiles, LB_GETCURSEL, 0, 0);
	if (currentWord != LB_ERR)
	{
		SendMessage(hFiles, LB_DELETESTRING, currentWord, 0);
		FileNamw.erase(FileNamw.begin() + currentWord);
		MessageBox(hDialog, TEXT("Файл удален из списка"), TEXT("ИНФО"), MB_OK | MB_ICONINFORMATION);
	}
	else
		MessageBox(hDialog, TEXT("Файл из списка не выбран"), TEXT("ОШИБКА"), MB_OK | MB_ICONERROR);
}
void MainDlg::CheckerRepeating(WCHAR* szPathFile)
{
	int dIndex = SendMessage(hFiles, LB_FINDSTRINGEXACT, -1, (LPARAM)szPathFile);
	if (dIndex == LB_ERR)
	{
		wstring str = szPathFile;
		int index = str.find(PathBanWords);
		if (index != string::npos)
		{
			MessageBox(hDialog, TEXT("Файл с запрещенными словами нельзя добавить в список."), TEXT("ОШИБКА"), MB_OK | MB_ICONERROR);
			return;
		}

		SendMessage(hFiles, LB_ADDSTRING, 0, (LPARAM)szPathFile);
		FileNamw.push_back(szPathFile);
	}
	else
		MessageBox(hDialog, TEXT("Этот файл уже добавлен в список"), TEXT("ОШИБКА"), MB_OK | MB_ICONERROR);
}


void MainDlg::DelBanWord()
{
	int currentWord = SendMessage(hBanWords, LB_GETCURSEL, 0, 0);
	if (currentWord != LB_ERR)
	{
		SendMessage(hBanWords, LB_DELETESTRING, currentWord, 0);
		BanWord.erase(BanWord.begin() + currentWord);
	}
	else
		MessageBox(hDialog, TEXT("Слово из списка не выбрано"), TEXT("ОШИБКА"), MB_OK | MB_ICONERROR);
}
void MainDlg::Start()
{
	SendMessage(hProgressBar, PBM_SETRANGE, 0, MAKELPARAM(0, 200));
	SendMessage(hProgressBar, PBM_SETSTEP, 1, 0);
	SendMessage(hProgressBar, PBM_SETPOS, 0, 0);
	SendMessage(hProgressBar, PBM_SETBKCOLOR, 0, LPARAM(RGB(0, 0, 255)));
	SendMessage(hProgressBar, PBM_SETBARCOLOR, 0, LPARAM(RGB(255, 255, 0)));

	if (FileNamw.size() == 0)
	{
		MessageBox(hDialog, TEXT("В программу не добавлены файлы"), TEXT("ОШИБКА"), MB_OK | MB_ICONERROR);
		return;
	}
	else if (BanWord.size() == 0)
	{
		MessageBox(hDialog, TEXT("Нет слов для поиска"), TEXT("ОШИБКА"), MB_OK | MB_ICONERROR);
		return;
	}

	vector <int> temp(BanWord.size());
	BanWordN = temp;

	isWork = TRUE;

	EnableWindow(hBtnStart, FALSE);
	EnableWindow(hBtnPause, TRUE);
	EnableWindow(hBtnStop, TRUE);

	MainDlg dlg(*this);

	SendMessage(hProgressBar, PBM_SETRANGE, 0, MAKELPARAM(0, FileNamw.size()));
	hThread = CreateThread(NULL, 0, Thread, this, 0, NULL);
}

void MainDlg::EditBanWord()
{
	int currentWord = SendMessage(hBanWords, LB_GETCURSEL, 0, 0);
	if (currentWord != LB_ERR)
	{
		EditWords dlg;
		wcscpy_s(dlg.str, BanWord[currentWord].c_str());
		WCHAR szBuf[MAX_SIZE];

		INT_PTR dpResult = DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG_2), hDialog, EditWords::DlgProc);

		if (dpResult == IDOK)
		{
			int index = SendMessage(hBanWords, LB_FINDSTRINGEXACT, -1, (LPARAM)dlg.str);
			if (index == LB_ERR)
			{
				BanWord[currentWord] = dlg.str;
				RefreshList();
			}
			else
				MessageBox(hDialog, TEXT("Такое запрещенное слово уже есть в списке"), TEXT("ОШИБКА"), MB_OK | MB_ICONERROR);
		}
	}
	else
		MessageBox(hDialog, TEXT("Слово из списка не выбрано"), TEXT("ОШИБКА"), MB_OK | MB_ICONERROR);
}
void MainDlg::RefreshList()
{
	SendMessage(hBanWords, LB_RESETCONTENT, 0, 0);
	for (int i = 0; i < BanWord.size(); i++)
		SendMessage(hBanWords, LB_ADDSTRING, 0, (LPARAM)BanWord[i].c_str());
}

void MainDlg::ShowN()
{
	wstring szStat;
	szStat.append(TEXT("Статистика запрещенных слов: \n"));
	for (int i = 0; i < BanWordN.size(); i++)
	{
		WCHAR buf[MAX_SIZE];
		wsprintf(buf, TEXT("%s: %d\n"), BanWord[i].c_str(), BanWordN[i]);
		szStat.append(buf);
	}

	struct _stat fileStat;
	if (_wstat(FileNamw[0].c_str(), &fileStat) == 0)
	{
		wstringstream fileInfo;
		fileInfo << TEXT("Path file: ") << FileNamw[0] << TEXT("\n");
		fileInfo << TEXT("Size file: ") << fileStat.st_size << TEXT(" байт\n\n");
		szStat.insert(0, fileInfo.str());
	}

	MessageBox(hDialog, szStat.c_str(), TEXT("ИНФО"), MB_OK | MB_ICONINFORMATION);
}

void MainDlg::AddBandWord()
{
	WCHAR szBuffer[MAX_SIZE];
	GetWindowText(hEditBanWords, szBuffer, sizeof(szBuffer));
	if (lstrlen(szBuffer))
	{
		int index = SendMessage(hBanWords, LB_FINDSTRINGEXACT, -1, (LPARAM)szBuffer);
		if (index == LB_ERR)
		{
			SendMessage(hBanWords, LB_ADDSTRING, 0, LPARAM(szBuffer));
			SetWindowText(hEditBanWords, NULL);
			BanWord.push_back(szBuffer);
		}
		else
			MessageBox(hDialog, TEXT("Такое запрещенное слово уже есть в списке"), TEXT("ОШИБКА"), MB_OK | MB_ICONERROR);
	}
	else
		MessageBox(hDialog, TEXT("Пустое поле запрещенного слова"), TEXT("ОШИБКА"), MB_OK | MB_ICONERROR);
}

void MainDlg::Cls_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch (id)
	{
		case IDC_BTN_UPLOAD_FILE:
		{
			AddFile();
			break;
		}
		case IDC_BTN_DEL_FILE:
		{
			DelFile();
			break;
		}
		case IDC_BTN_ADD_BAN:
		{
			AddBandWord();
			break;
		}
		case IDC_BTN_DEL_BAN:
		{
			DelBanWord();
			break;
		}
		case IDC_BTN_EDIT_WORD:
		{
			EditBanWord();
			break;
		}
		case IDC_BTN_START:
		{
			Start();
			break;
		}
		case IDC_BTN_PAUSE:
		{
			if (isWork)
			{
				SuspendThread(hThread);
				SetWindowText(hBtnPause, TEXT("Продолжить"));
			}
			else
			{
				ResumeThread(hThread);
				SetWindowText(hBtnPause, TEXT("Пауза"));
			}
			isWork = !isWork;
			break;
		}
		case IDC_BTN_STOP:
		{
			TerminateThread(hThread, 0);
			EnableWindow(hBtnStart, TRUE);
			EnableWindow(hBtnPause, FALSE);
			EnableWindow(hBtnStop, FALSE);

			MessageBox(hDialog, TEXT("Прогресс остановлен"), TEXT("ИНФО"), MB_OK | MB_ICONINFORMATION);

			SendMessage(hProgressBar, PBM_SETRANGE, 0, MAKELPARAM(0, 200));
			SendMessage(hProgressBar, PBM_SETSTEP, 1, 0);
			SendMessage(hProgressBar, PBM_SETPOS, 0, 0);
			SendMessage(hProgressBar, PBM_SETBKCOLOR, 0, LPARAM(RGB(0, 0, 255)));
			SendMessage(hProgressBar, PBM_SETBARCOLOR, 0, LPARAM(RGB(255, 255, 0)));
		}
		default:
			break;
	}
}

INT_PTR CALLBACK MainDlg::DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		HANDLE_MSG(hwnd, WM_CLOSE, ptr->Cls_OnClose);
		HANDLE_MSG(hwnd, WM_INITDIALOG, ptr->Cls_OnInitDialog);
		HANDLE_MSG(hwnd, WM_COMMAND, ptr->Cls_OnCommand);
	}
	return FALSE;
}
