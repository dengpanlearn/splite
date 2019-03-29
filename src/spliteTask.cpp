/*spliteTask.cpp*/



#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <qfileinfo.h>
#include <qtextcodec.h>
#include "../include/spliteTask.h"
#include "../include/qtGlobal.h"

using namespace std;
using namespace cv;
#define	SPLITE_TASK_NAME					_T("taskSplite")
#define	SPLITE_BORDER_LINES					2
#define IMG_SEPERATOR_WAIT_START_PIXS		2
#define IMG_SEPERATOR_WAIT_END_PIXS			3
#define	SPLITE_FILE_NAME_MAX				128
CSpliteTask::CSpliteTask()
{
	m_curFileIdx = 0;
}


CSpliteTask::~CSpliteTask()
{
	Close();
}

BOOL CSpliteTask::Create(int pri, int stackSize, int timeoutMs)
{
	return CTriggerTask::Create(SPLITE_TASK_NAME, stackSize, pri, 0, timeoutMs);
}

void CSpliteTask::Close()
{
	CTriggerTask::Close();
}

BOOL CSpliteTask::PrepareTriggerParam(void* pArg)
{
	if (!CTriggerTask::PrepareTriggerParam(pArg))
		return FALSE;

	SPLITE_TASK_ARG* pTaskArg = (SPLITE_TASK_ARG*)pArg;
	m_fileNameList = pTaskArg->GetFileList();
	m_spliteOutDir = pTaskArg->GetOutDir();
	return TRUE;
}

UINT CSpliteTask::OnPrepareStartWork(UINT step)
{
	m_curFileIdx = 0;
	return CTriggerTask::OnPrepareStartWork(step);
}

UINT CSpliteTask::OnTimeoutWork(UINT step)
{
	if (m_curFileIdx == m_fileNameList.size())
		return CTriggerTask::OnTimeoutWork(step);

	do
	{
		QString fileName = m_fileNameList[m_curFileIdx++];
		char szFileName[SPLITE_FILE_NAME_MAX] = {0};
		int nameLen = QString2Char(fileName, szFileName);
		if (nameLen < 0)
			break;
		szFileName[nameLen] = '\0';

		QFileInfo fileInfo(fileName);
		QString fileBaseName = fileInfo.baseName();
	
	
		QString spliteOutFileNamePrefix = m_spliteOutDir +"/"+ fileBaseName;
		char szSpliteFileOutNamePreFix[SPLITE_FILE_NAME_MAX] = {0};
		nameLen = QString2Char(spliteOutFileNamePrefix, szSpliteFileOutNamePreFix);
		if (nameLen < 0)
			break;

		szSpliteFileOutNamePreFix[nameLen] = '\0';
		SpliteWork(szFileName, szSpliteFileOutNamePreFix);
		UpdateTriggerWorkProgress();
	} while (FALSE);

	return step;
}

UINT CSpliteTask::OnPrepareStopWork(UINT step)
{
	return CTriggerTask::OnPrepareStopWork(step);
}

BOOL CSpliteTask::SpliteWork(const char* pFileName, const char* pOutFileNamePrefix)
{
	Mat orgImg = imread(pFileName);

	if (orgImg.data == NULL)
		return FALSE;

	// 去除边框
	if ((orgImg.rows < SPLITE_BORDER_LINES) || (orgImg.cols < SPLITE_BORDER_LINES))
		return FALSE;

	Mat noneBorderImg(orgImg.rows, orgImg.cols, orgImg.type(), Scalar(255,255,255));

	Mat toCopyImg = noneBorderImg(Range(SPLITE_BORDER_LINES, orgImg.rows - SPLITE_BORDER_LINES + 1),
		Range(SPLITE_BORDER_LINES, orgImg.cols - SPLITE_BORDER_LINES + 1));

	orgImg(Range(SPLITE_BORDER_LINES, orgImg.rows - SPLITE_BORDER_LINES + 1),
		Range(SPLITE_BORDER_LINES, orgImg.cols - SPLITE_BORDER_LINES + 1)).copyTo(toCopyImg);


	// 灰度化

	Mat grayImg;
	grayImg.create(noneBorderImg.size(), noneBorderImg.type());
	cvtColor(noneBorderImg, grayImg, COLOR_BGR2GRAY);

	// 二值化
	Mat binImg;
	binImg.create(grayImg.size(), grayImg.type());
	threshold(grayImg, binImg, 120, 255, THRESH_BINARY);

	// 分割
	vector<Vec3i> splitLines;
	if (SeparatorMat(binImg, splitLines))
	{
		// 保存分割图形
		for (int i = 0; i < splitLines.size(); i++)
		{
			char fileNameOut[64] = { 0 };
			sprintf_s(fileNameOut, sizeof(fileNameOut)-1, "%s_%d.jpg", pOutFileNamePrefix, i);

			int colStart = splitLines[i][0];
			int colEnd = splitLines[i][1];
			Mat tmp = binImg(Range::all(), Range(colStart, colEnd));
			imwrite(fileNameOut, tmp);
		}
	}
	return TRUE;
}

BOOL CSpliteTask::SeparatorMat(Mat const& binImg, vector<Vec3i>& splitLines)
{
	SEPERATOR_STEP findStep = SEPERATOR_STEP_FINDING_START;
	int keepStepTimes = 0;
	int startColIndex = 0;


	if ((binImg.cols < IMG_SEPERATOR_WAIT_START_PIXS) || (binImg.cols < IMG_SEPERATOR_WAIT_END_PIXS))
		return FALSE;

	// 分割字符
	for (int i = 0; i < binImg.cols; i++)
	{
		int j = 0;
		for (; j < binImg.rows; j++)
		{
			if (binImg.at<unsigned char>(j, i))
			{
				continue;
			}

			if ((j > 0) && !binImg.at<unsigned char>(j - 1, i))
				break;

			if ((j > 0) && (i > 0) && !binImg.at<unsigned char>(j - 1, i - 1))
				break;

			if ((j > 0) && (i < (binImg.cols - 1)) && !binImg.at<unsigned char>(j - 1, i + 1))
				break;

			if ((j < (binImg.rows - 1)) && (i < (binImg.cols - 1)) && !binImg.at<unsigned char>(j + 1, i + 1))
				break;
		}

		if (findStep == SEPERATOR_STEP_FINDING_START)
		{
			if (j >= binImg.rows)
			{
				keepStepTimes = 0;
				continue;
			}


			if (++keepStepTimes < IMG_SEPERATOR_WAIT_START_PIXS)
				continue;


			// 找到	
			startColIndex = i - IMG_SEPERATOR_WAIT_START_PIXS + 1;
			keepStepTimes = 0;
			findStep = SEPERATOR_STEP_FINDING_END;
		}
		else
		{
			if (j == (binImg.cols - 1))
				goto  _SAVE_CHILD;

			if (j < binImg.rows)
			{
				keepStepTimes = 0;
				continue;
			}


			if (++keepStepTimes < IMG_SEPERATOR_WAIT_END_PIXS)
				continue;

		_SAVE_CHILD:
			int endColIndex = i - IMG_SEPERATOR_WAIT_END_PIXS + 1;

			Vec3i spliteLine;
			spliteLine[0] = startColIndex;
			spliteLine[1] = endColIndex + 1;
			spliteLine[2] = endColIndex - startColIndex + 1;
			splitLines.push_back(spliteLine);
			keepStepTimes = 0;
			findStep = SEPERATOR_STEP_FINDING_START;
		}
	}


	return TRUE;
}
