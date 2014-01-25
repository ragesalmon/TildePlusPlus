#include "stdafx.h"
#include "Config.h"
#include "Tiling.h"
#include <math.h>

extern "C" __declspec(dllexport) void Main_Tile(int currentMonitor)
{
	if(MonitorList.at(currentMonitor).WindowList.size() == 1)
	{
			SetWindowPos(MonitorList.at(currentMonitor).WindowList.at(0), HWND_TOP, MonitorList.at(currentMonitor).lB + options->BHor, MonitorList.at(currentMonitor).tB + options->TBTSize + options->BVer, MonitorList.at(currentMonitor).usableWidth, MonitorList.at(currentMonitor).usableHeight, NULL);
			return;
	}
	int rows, cols, WindowSizeHorizontal, WindowSizeVertical, WindowMovementHorizontal, WindowMovementVertical;
	int currentWindow = 0;

	double sqrtWin = sqrt(MonitorList.at(currentMonitor).WindowList.size());
	if((sqrtWin - floor(sqrtWin)) < ((ceil(sqrtWin) - sqrtWin)))
	{
		cols = floor(sqrtWin);
	}
	else 
	{
		cols = ceil(sqrtWin);
	}
	rows = ceil(sqrtWin);
	WindowSizeHorizontal = (MonitorList.at(currentMonitor).usableWidth - options->PHor) / cols;
	WindowSizeVertical = (MonitorList.at(currentMonitor).usableHeight-(options->PVer)) / rows;

	WindowMovementHorizontal = WindowSizeHorizontal + options->PHor;
	WindowMovementVertical = WindowSizeVertical + options->PVer;

	for(int i = 0; i < rows; i++)
	{
		for(int j = 0; j < cols; j++)
		{
			if(currentWindow >= MonitorList.at(currentMonitor).WindowList.size())
			{
				break;
			}
			SetWindowPos(MonitorList.at(currentMonitor).WindowList.at(currentWindow), HWND_TOP, MonitorList.at(currentMonitor).lB + options->BHor + (WindowMovementHorizontal*j), MonitorList.at(currentMonitor).tB + options->BHor + (WindowMovementVertical*i), WindowSizeHorizontal, WindowSizeVertical, NULL);
			currentWindow++;
		}
	}
}