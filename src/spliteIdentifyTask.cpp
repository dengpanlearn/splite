/*
* spliteIdentifyTask.h
*/


#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/ml.hpp>
#include <qfileinfo.h>
#include <qtextcodec.h>
#include "../include/spliteTask.h"
#include "../include/qtGlobal.h"
#include "../include/spliteIdentifyTask.h"

using namespace std;
using namespace cv;

#define	SPLITE_IDENTITY_FILE_NAME_MAX		64
CSpliteIdentifyTask::CSpliteIdentifyTask()
{
	m_maxSpliteCounts = 0;
	m_pTrainOutput = NULL;
}

CSpliteIdentifyTask::~CSpliteIdentifyTask()
{

}

BOOL CSpliteIdentifyTask::Create(int pri, int stackSize, int timeoutMs)
{
	return CSpliteTask::Create(pri, stackSize, timeoutMs);
}

void CSpliteIdentifyTask::Close()
{
	return CSpliteTask::Close();
}

BOOL CSpliteIdentifyTask::PrepareTriggerParam(void* pArg)
{
	SPLITE_IDENTITY_TASK_ARG* pTaskArg = (SPLITE_IDENTITY_TASK_ARG* )pArg;

	m_toIdentityFileName = pTaskArg->GetToIdentityFileName();
	m_trainFileName = pTaskArg->GetTrainFileName();
	m_maxSpliteCounts = pTaskArg->GetMaxSpliteCounts();
	if (m_pTrainOutput != NULL)
	{
		if (m_maxSpliteCounts < pTaskArg->GetMaxSpliteCounts())
		{
			free(m_pTrainOutput);
			m_pTrainOutput = NULL;
			m_maxSpliteCounts = pTaskArg->GetMaxSpliteCounts();
		}
	}

	if (m_pTrainOutput == NULL)
		m_pTrainOutput = (char*)calloc(m_maxSpliteCounts + 1, 1);
	return (m_pTrainOutput != NULL);
}

UINT CSpliteIdentifyTask::OnPrepareStartWork(UINT step)
{
	return CSpliteTask::OnPrepareStartWork(step);
}

UINT CSpliteIdentifyTask::OnTimeoutWork(UINT step)
{
	char identityFileName[SPLITE_IDENTITY_FILE_NAME_MAX] = { 0 };
	int nameLen = QString2Char(m_toIdentityFileName, identityFileName);
	if (nameLen <= 0)
		return CTriggerTask::OnTimeoutWork(step);

	identityFileName[nameLen] = '\0';
	
	Mat orgImg = imread(identityFileName, IMREAD_COLOR);
	if (orgImg.data == NULL)
		return CTriggerTask::OnTimeoutWork(step);
	vector<Mat> splitImgs;
	if (!SpliteWork(orgImg, splitImgs))
		return CTriggerTask::OnTimeoutWork(step);

	if (!ProcessSplitedImage(splitImgs))
		return CTriggerTask::OnTimeoutWork(step);
	
	UpdateTriggerWorkProgress();
	return TRIGGER_STEP_WAIT_END;
}

UINT CSpliteIdentifyTask::OnPrepareStopWork(UINT step)
{
	return CSpliteTask::OnPrepareStopWork(step);
}


BOOL CSpliteIdentifyTask::ProcessSplitedImage(vector<Mat>& spliteMats)
{
	char trainFileName[SPLITE_IDENTITY_FILE_NAME_MAX] = { 0 };
	int nameLen = QString2Char(m_trainFileName, trainFileName);
	if (nameLen <= 0)
		return FALSE;

	trainFileName[nameLen] = '\0';

	Ptr<ml::SVM> svm = ml::SVM::load(trainFileName);
	if (!svm->isTrained())
		return FALSE;

	int varCount = svm->getVarCount();

	for (int i = 0; i < spliteMats.size()&&i < m_maxSpliteCounts; i++)
	{
		Mat spliteImg = spliteMats[i];
		Mat reshapeImg = spliteImg.reshape(0, 1);
		if (reshapeImg.cols != varCount)
			return FALSE;

		Mat floatImg;
		reshapeImg.convertTo(floatImg, CV_32FC1);
		
		int predictVal = (int)svm->predict(floatImg);

		CSingleLock lock(GetMutex(), TRUE);
		m_pTrainOutput[i] = (char)predictVal;
	}

	return TRUE;
}