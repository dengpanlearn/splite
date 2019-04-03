/*global.cpp*/

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "../include/global.h"

#include "../include/spliteTask.h"
#include "../include/trainTask.h"
#include "../include/spliteIdentifyTask.h"

#include <qstring.h>
#include <qstringlist.h>
#include <qdir.h>


CSpliteTask	g_spliteTask;
CTrainTask	g_trainTask;
CSpliteIdentifyTask g_spliteIdentityTask;

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

void TestIdentity()
{
	SPLITE_IDENTITY_TASK_ARG taskArg(QString("E:\\self\\opencv\\img\\test9.jpg"), QString("E:\\self\\opencv\\trainOut\\dd.xml"), 6);
	g_spliteIdentityTask.StartTrigger(&taskArg);
}


BOOL UsrInit()
{
#if 1
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
#endif
	if (!g_spliteIdentityTask.Create(IDENTITY_TASK_PRI, IDENTITY_TASK_STACKSIZE, IDENTITY_TASK_TIMEOUT_MS))
	{
		printf("identity task create fail\n");
		return FALSE;
	}

	return TRUE;
}


void UsrExit()
{
	g_spliteIdentityTask.StopWork();
	g_spliteIdentityTask.WaitWorkExit(2000);
	g_spliteTask.StopWork();
	g_spliteTask.WaitWorkExit(2000);
	g_trainTask.StopWork();
	g_trainTask.WaitWorkExit(2000);
}
