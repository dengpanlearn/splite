/*trainTask.cpp
*
*/

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/ml.hpp>
#include <qregexp.h>
#include <qfileinfo.h>
#include "../include/trainTask.h"
#include "../include/qtGlobal.h"

using namespace std;
using namespace cv;

#define	TRAIN_TASK_NAME						_T("trainTask")
#define TRAIN_FILE_NAME_MAX					64
#define	TRAIN_FILE_NAME_MARK_CHAR			('_')

CTrainTask::CTrainTask()
{
}

CTrainTask::~CTrainTask()
{

}

BOOL CTrainTask::Create(int pri, int stackSize, int timeoutMs)
{
	return CTriggerTask::Create(TRAIN_TASK_NAME, stackSize, pri, 0, timeoutMs);
}

void CTrainTask::Close()
{
	CTriggerTask::Close();
}

BOOL CTrainTask::PrepareTriggerParam(void* pArg)
{
	TRAIN_TASK_ARG* pTaskArg = (TRAIN_TASK_ARG*)pArg;
	QStringList fileList = pTaskArg->GetFileList();

	m_trainOutFile = pTaskArg->GetOutFile();
	m_trainMatCols = VertifyTrainFiles(fileList, m_trainList);
	return (m_trainMatCols > 0);
}


int CTrainTask::VertifyTrainFiles(QStringList const&  fileList, QStringList& availFileList)
{
	int totalFileCounts = fileList.size();
	int trainRowsPerFile = 0;
	int trainColsPerFile = 0;
	int availabeIdx = 0;
	do
	{
		QString fileName = fileList[availabeIdx++];

		int prefixIdx = fileName.indexOf(QRegExp("[\\da-z]_\\d+"));
		if (prefixIdx < 0)
			continue;

		char szFileName[TRAIN_FILE_NAME_MAX] = { 0 };
		int nameLen = QString2Char(fileName, szFileName);
		if (nameLen <= 0)
			continue;

		szFileName[nameLen] = '\0';

		Mat trainImg = imread(szFileName, IMREAD_GRAYSCALE);
		if (trainImg.data == NULL)
			continue;

		trainRowsPerFile = trainImg.rows;
		trainColsPerFile = trainImg.cols;
		availFileList.push_back(fileName);
		break;

	} while (TRUE);
	

	if (availFileList.isEmpty())
		return 0;


	for (availabeIdx; availabeIdx < totalFileCounts; availabeIdx++)
	{
		QString fileName = fileList[availabeIdx];
		int curPrefixIdx = fileName.indexOf(QRegExp("[\\da-z]_\\d+"));
		if (curPrefixIdx < 0)
			continue;
		
		// check cols and rows
		char szFileName[TRAIN_FILE_NAME_MAX] = { 0 };
		int nameLen = QString2Char(fileName, szFileName);
		if (nameLen <= 0)
			continue;

		szFileName[nameLen] = '\0';

		Mat trainImg = imread(szFileName, IMREAD_GRAYSCALE);
		if (trainImg.data == NULL)
			continue;

		if ((trainImg.rows != trainRowsPerFile) || (trainImg.cols != trainColsPerFile))
			continue;

		availFileList.push_back(fileName);
	}

	return trainRowsPerFile*trainColsPerFile;
}


UINT CTrainTask::OnPrepareStartWork(UINT step)
{
	return CTriggerTask::OnPrepareStartWork(step);
}

UINT CTrainTask::OnTimeoutWork(UINT step)
{
	int trainRows = m_trainList.size();
	if (trainRows <= 0)
		return CTriggerTask::OnTimeoutWork(step);

	Mat trainMat(trainRows, m_trainMatCols, CV_8UC1);
	Mat labMat (trainRows, 1, CV_32SC1);
	for (int i = 0; i < trainRows; i++)
	{
		QString fileName = m_trainList[i];
		
		// check cols and rows
		char szFileName[TRAIN_FILE_NAME_MAX] = { 0 };
		int nameLen = QString2Char(fileName, szFileName);
		if (nameLen <= 0)
			continue;


		Mat trainImg = imread(szFileName, IMREAD_GRAYSCALE);
		if (trainImg.data == NULL)
			continue;

		int indexOfLabVal = fileName.indexOf(QRegExp(("[\\da-z]_\\d+")));
		if (indexOfLabVal < 0)
			continue;

		nameLen = QString2Char(fileName.mid(indexOfLabVal), szFileName);
		if (nameLen <= 0)
			continue;

		szFileName[nameLen] = '\0';

		Mat toBeTrainImg =trainImg.reshape(0, 1);
		Mat rowMat = trainMat.rowRange(Range(i, i + 1));
		toBeTrainImg.copyTo(rowMat);
		labMat.at<int>(i, 0) = (int)szFileName[0];
		UpdateTriggerWorkProgress();
	}

	char saveFileName[TRAIN_FILE_NAME_MAX] = { 0 };
	int nameLen = QString2Char(m_trainOutFile, saveFileName);
	if (nameLen <= 0)
		return CTriggerTask::OnTimeoutWork(step);

	Mat trainMatFloat(trainMat.size(),CV_32FC1);
	trainMat.convertTo(trainMatFloat, CV_32FC1);
	Ptr<ml::SVM> svm = ml::SVM::create();
	svm->setType(ml::SVM::C_SVC);

	svm->setKernel(ml::SVM::RBF);
	svm->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER, 100, 1e-6));
	svm->train(trainMatFloat, ml::ROW_SAMPLE, labMat);
	svm->save(saveFileName);

	return CTriggerTask::OnTimeoutWork(step);
}

UINT CTrainTask::OnPrepareStopWork(UINT step)
{
	return CTriggerTask::OnPrepareStopWork(step);
}