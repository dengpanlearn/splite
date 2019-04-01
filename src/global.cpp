/*global.cpp*/

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "../include/global.h"

#include "../include/spliteTask.h"
#include "../include/trainTask.h"

#include <qstring.h>
#include <qstringlist.h>
#include <qdir.h>


CSpliteTask	g_spliteTask;
CTrainTask	g_trainTask;


void TestTrain()
{
	QDir trainDir("E:\\self\\opencv\\train");

	QStringList fileNameFilter;
	fileNameFilter << "*.png" << "*.jpg";
	QString outfile("E:\\self\\opencv\\trainOut\\dd.xml");

	QStringList fileList = trainDir.entryList(fileNameFilter, QDir::Files | QDir::NoDotAndDotDot, QDir::Name);
	QStringList filePathList;
	for (int i = 0; i < fileList.size(); i++)
	{
		filePathList << trainDir.absoluteFilePath(fileList[i]);
	}
	TRAIN_TASK_ARG taskArg(filePathList, outfile);
	g_trainTask.StartTrigger(&taskArg);
}


BOOL UsrInit()
{
	if (!g_spliteTask.Create(SPLITE_TASK_PRI, SPLITE_TASK_STACKSIZE, SPLITE_TASK_TIMEOUT_MS))
	{
		printf("splite task create fail\n");
		return FALSE;
	}

	if (!g_trainTask.Create(SPLITE_TASK_PRI, SPLITE_TASK_STACKSIZE, SPLITE_TASK_TIMEOUT_MS))
	{
		printf("train task create fail\n");
		return FALSE;
	}

	return TRUE;
}


void UsrExit()
{
	g_spliteTask.StopWork();
	g_spliteTask.WaitWorkExit(2000);
	g_trainTask.StopWork();
	g_trainTask.WaitWorkExit(2000);
}
