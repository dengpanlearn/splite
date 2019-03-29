/*global.cpp*/

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "../include/global.h"

#include "../include/spliteTask.h"

CSpliteTask	g_spliteTask;

BOOL UsrInit()
{
	if (!g_spliteTask.Create(SPLITE_TASK_PRI, SPLITE_TASK_STACKSIZE, SPLITE_TASK_TIMEOUT_MS))
	{
		printf("splite task create fail\n");
		return FALSE;
	}

	return TRUE;
}


void UsrExit()
{
	g_spliteTask.StopWork();
	g_spliteTask.WaitWorkExit(2000);
}
