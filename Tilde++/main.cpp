#include "main.h"
#include <iostream>


const PCWSTR g_szClassName = L"myWindowClass";
char hwndow[50];

UINT shellHookMessage;
updateFunction uFunc;
handleHotkeyFunction hhFunc;
registerHotkeyFunction rhFunc;
HHOOK hhook;
WIN32_FIND_DATAA tFindData;
HANDLE tHandle;

LRESULT CALLBACK windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	RECT rect;
	switch (msg)
	{
	default:
		if(msg == shellHookMessage)
		{
			UpdateWindowList(wParam, lParam, WindowList);
		}
		if(msg == WM_HOTKEY)
		{
			if(wParam == arrowKeyRight)
			{
				if(!(ChangeDll(1)))
				{
					idbgmsg("Failed to change DLLs!",NULL);
				}
				else
				{
					UpdateMonitorHotkeys(FindCurrentMonitor(GetForegroundWindow(), MonitorList), GetForegroundWindow());
					MonitorList.at(FindCurrentMonitor(GetForegroundWindow(), MonitorList)).tFunc(FindCurrentMonitor(GetForegroundWindow(), MonitorList));
				}
			}
			else if(wParam == arrowKeyLeft)
			{
				if(!(ChangeDll(-1)))
				{
					idbgmsg("Falied to change DLLs!",NULL);
				}
				else
				{
					UpdateMonitorHotkeys(FindCurrentMonitor(GetForegroundWindow(), MonitorList), GetForegroundWindow());
					MonitorList.at(FindCurrentMonitor(GetForegroundWindow(), MonitorList)).tFunc(FindCurrentMonitor(GetForegroundWindow(), MonitorList));
				}
			}
			else if(wParam == arrowKeyDown)
			{
				MessageBoxA(NULL, "Tilde++ shutting down!", "Tilde++", MB_OK);
				exit(0);
			}
			else if((wParam == numRow1)||(wParam == numRow1Alt)||(wParam == numRow2)||(wParam == numRow2Alt)||(wParam == numRow3)||(wParam == numRow3Alt)||(wParam == numRow4)||(wParam == numRow4Alt)||(wParam == numRow5)||(wParam == numRow5Alt)||(wParam == numRow6)||(wParam == numRow6Alt)||(wParam == numRow7)||(wParam == numRow7Alt)||(wParam == numRow8)||(wParam == numRow8Alt)||(wParam == numRow9)||(wParam == numRow9Alt))
			{
				HandleWorkspaceKey(wParam);
			}
			else
			{
				if(!(MonitorList.at(FindCurrentMonitor(GetForegroundWindow(), MonitorList)).hhFunc(wParam)))
				{
					if(FindCurrentMonitor(GetForegroundWindow(), MonitorList)+1 >= MonitorList.size())
					{
						return MonitorList.at(0).hhFunc(wParam);
					} 
					else
					{
						return MonitorList.at(FindCurrentMonitor(GetForegroundWindow(), MonitorList)).hhFunc(wParam);
					}
				return TRUE;
				}
			}
		}
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	

	return 0;
}



int main(HINSTANCE hInstance, HINSTANCE hPrevInstance)
{
#ifndef DEBUGENABLED
	FreeConsole();
#endif
	WNDCLASSEX wc;
	HWND hwnd;
	MSG msg;
	BOOL bRet;

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = windowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = g_szClassName;
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	char buffer[64];
	char dwBuf[128];

	bool fakewindowpassed = FALSE;

	tagWINDOWINFO wi;
	EnumDisplayMonitors(NULL, NULL, GetMonitors, NULL);
	EnumWindows(sendWindowsToTempWindowArray, NULL);

	WindowList.resize(numWindows);

	for(int i = 0; i < numWindows; i++){
		dbgmsg("Window HWND: %p",tempWindowList.at(i));
		dbgmsg("Is Iconic: %d",IsIconic(tempWindowList.at(i)));
		dbgmsg("Is Visible: %d",IsWindowVisible(tempWindowList.at(i)));
		dbgmsg("Is Enabled: %d",IsWindowEnabled(tempWindowList.at(i)));
		if(IsWindowVisible(tempWindowList.at(i)) && GetWindowTextA(tempWindowList.at(i), buffer, 64)!=0 && strcmp(buffer, "Program Manager")!=0 && strcmp(buffer, " ")!=0 && strcmp(buffer,"")!=0)
		{
			sendWindowsToWindowArray(tempWindowList.at(i));
			SendWindowToMonitor(tempWindowList.at(i), MonitorList);
			idbgmsg("Window sent to array",NULL);
			idbgmsg("Window HWND in array: %p", WindowList.at(WindowList.size()-1));
			idbgmsg("Window Title: %d",buffer);
		}
	}
	WindowList.erase(WindowList.begin());
	numWindows = WindowList.size();
	for(int i = 0; i < numWindows;i++){
		GetWindowTextA(WindowList.at(i), buffer, 64);
		GetWindowInfo(WindowList.at(i), &wi);
		sprintf(dwBuf, "%d", wi.dwStyle);
		idbgmsg("Window %d name: %p", i, buffer);
		idbgmsg("Window %d style: %p", i, dwBuf);
	}
	dbgmsg("numWindows: %d", numWindows);

	if(!RegisterClassEx(&wc))
	{
		MessageBox(NULL, L"Window Registration Failed", L"Error!!", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	if(!(hwnd = CreateWindowEx(
		WS_EX_APPWINDOW,
		g_szClassName,
		L"Tilde++",
		WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT, 240,120,
		HWND_MESSAGE, NULL, hInstance, NULL)))
	{
		int error = GetLastError();
		exit(error);
	}
	
	if(!(RegisterShellHookWindow(hwnd)))
	{
		int error = GetLastError();
		exit(error);
	}
	RegisterMainHotkeys(hwnd);
	shellHookMessage = RegisterWindowMessage(TEXT("SHELLHOOK"));

	char locationBuffer[MAX_PATH];
	char dllName[MAX_PATH];

	GetFullPathNameA("Config.ini",MAX_PATH,locationBuffer, NULL);

	GetPrivateProfileStringA("Tilde","DLL","Ship.dll",dllName,MAX_PATH,locationBuffer);

	for(int i = 0; i < MonitorList.size(); i++)
	{
		if(!(MonitorList.at(i).SetFunctions(dllName)))
			exit(404);
		MonitorList.at(i).tFunc(i);
	}
	UpdateMonitorHotkeys(FindCurrentMonitor(GetForegroundWindow(),MonitorList), hwnd);

	
	tHandle = FindFirstFileA("*.dll",&tFindData);
	dllList.push_back(tFindData.cFileName);
	while (FindNextFileA(tHandle,&tFindData))
	{
		if((!(strcmp(tFindData.cFileName, "Libde++.dll"))||(!(strcmp(tFindData.cFileName, "msvcr110.dll")))||(!(strcmp(tFindData.cFileName, "msvcp110.dll")))))
		{
			continue;
		}
		dllList.push_back(tFindData.cFileName);
	}

	while( (bRet = GetMessage( &msg, NULL, 0, 0 )) != 0 ) 
	{
		if (bRet == -1)
        {
         // handle the error and possibly exit
        }
        else
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
		}
	}




	std::cin.get();
	return 0;
}
