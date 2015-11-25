#define WIN32_MEAN_AND_LEAN 

#include <windows.h>
#include <tchar.h>
#include "resource.h"

//globals
HWND hWndMain;
boolean gIsRunning;
UINT gTimerId;
UINT gTimerPeriod = 128;
UINT gBeepPeriod  = 100;
UINT gBeepPitch   = 200;

void check_menu(boolean checked){
	MENUITEMINFO menuinfo;
	menuinfo.cbSize = sizeof(MENUITEMINFO);
	menuinfo.fMask = MIIM_STATE;
	menuinfo.fState = checked?MFS_CHECKED:MFS_ENABLED;

	SetMenuItemInfo(GetMenu(hWndMain), ID_RUNNING, FALSE, &menuinfo);
}

void start_timer(void){
	gIsRunning = TRUE;
	check_menu(TRUE);
	gTimerId = SetTimer(hWndMain, 1, 60000 / gTimerPeriod, NULL);
}

void stop_timer(void){
	KillTimer(hWndMain, gTimerId);
	check_menu(FALSE);
	gIsRunning = FALSE;

}

BOOL MainDialog_OnCommand(HWND hWnd, WORD wCommand, WORD wNotify, HWND hControl){
	char buffer[255];

	switch (wCommand){
		case IDOK:
			GetDlgItemText(hWnd, IDC_EDIT1, buffer, 255);
			gTimerPeriod = atol(buffer);
			GetDlgItemText(hWnd, IDC_EDIT2, buffer, 255);
			gBeepPeriod = atol(buffer);
			GetDlgItemText(hWnd, IDC_EDIT3, buffer, 255);
			gBeepPitch = atol(buffer);
		case IDCANCEL:
			EndDialog(hWnd, wCommand);
			break;
	}
	return TRUE;
}

BOOL MainDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
	char buffer[255];

	switch (uMsg){

		case WM_COMMAND:
			return MainDialog_OnCommand(hWnd, LOWORD(wParam), HIWORD(wParam), (HWND)lParam);

		case WM_INITDIALOG:
			_ltoa(gTimerPeriod, buffer, 10);
			SetDlgItemText(hWnd, IDC_EDIT1, buffer);
			_ltoa(gBeepPeriod, buffer, 10);
			SetDlgItemText(hWnd, IDC_EDIT2, buffer);
			_ltoa(gBeepPitch, buffer, 10);
			SetDlgItemText(hWnd, IDC_EDIT3, buffer);
			return 0;

		case WM_CLOSE:
			EndDialog(hWnd, 0);
			return TRUE;
	}
	return FALSE;
}

void SetTimeDialog(void){
	DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG1), NULL, (DLGPROC)MainDialogProc, 0);
}

BOOL MainWindow_OnCommand(HWND hWnd, WORD wCommand, WORD wNotify, HWND hControl){
	switch (wCommand){

		case ID_SETTIME:
			if (gIsRunning){
				stop_timer();
				SetTimeDialog();
				start_timer();
			}else{
				SetTimeDialog();
			}
			break;


		case ID_EXIT:
			DestroyWindow(hWnd);
			break;

		case ID_RUNNING:
			if (gIsRunning){
				stop_timer();
			}else{
				start_timer();
			}
			break;
	}

	return TRUE;
}

void MainWindow_OnPaint(HWND hWnd, HDC hDC){

	RECT r;

	GetClientRect(hWnd, &r);
	FillRect(hDC, &r, GetStockObject(GRAY_BRUSH));
}



LRESULT CALLBACK MainWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){

	PAINTSTRUCT	ps;
	HDC		hDC;


    switch (uMsg){

		case WM_COMMAND:
			MainWindow_OnCommand(hWnd, LOWORD(wParam), HIWORD(wParam), (HWND)lParam);
			return 0;

		case WM_PAINT:
			hDC = BeginPaint(hWnd, &ps);
			MainWindow_OnPaint(hWnd, hDC);
			EndPaint(hWnd, &ps);
			return 0;

		case WM_TIMER:
			Beep(gBeepPitch, gBeepPeriod);
			return 0;

		case WM_CLOSE:
			DestroyWindow(hWnd);
			return 0;

		case WM_CREATE:
			return 0;

		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void RegisterWindowClass(){
    WNDCLASSEX  wcx;

    ZeroMemory(&wcx, sizeof(WNDCLASSEX));
    wcx.cbSize = sizeof(WNDCLASSEX);
    wcx.lpfnWndProc = MainWindowProc;
    wcx.hInstance = GetModuleHandle(NULL);
    wcx.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_MAIN));
    wcx.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcx.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcx.lpszMenuName = MAKEINTRESOURCE(IDR_MAIN);
    wcx.lpszClassName = "MetronomeWindowClass";
    RegisterClassEx(&wcx);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE a, LPSTR b, int c){

	MSG msg;

	RegisterWindowClass();

    hWndMain = CreateWindowEx(WS_EX_APPWINDOW, "MetronomeWindowClass", "Metronome", WS_OVERLAPPEDWINDOW, 
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
    
	SetWindowPos(hWndMain, 0, 0, 0, 200, 150, SWP_NOZORDER | SWP_NOMOVE);

	ShowWindow(hWndMain, SW_SHOW);

	while (GetMessage(&msg, NULL, 0, 0)) DispatchMessage(&msg);

	return 0;
}
