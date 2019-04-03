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
#define SPLITE_IMAGE_ROTATE_ANGLE_MIN		(-10)
#define SPLITE_IMAGE_ROTATE_ANGLE_MAX		(10)
#define SPLITE_IMAGE_AVAILABLE_WIDTH(_cols)	((_cols)/4)
#define	SPLITE_IMAGE_RESIZE_WIDTH			(16)
#define	SPLITE_IMAGE_RESIZE_HEIGHT			(25)
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
		QString fileName = m_fileNameList[m_curFileIdx];
		char szFileName[SPLITE_FILE_NAME_MAX] = {0};
		int nameLen = QString2Char(fileName, szFileName);
		if (nameLen < 0)
			break;
		szFileName[nameLen] = '\0';

		Mat orgImg = imread(szFileName);
		if (orgImg.data == NULL)
			break;

		vector<Mat> spliteImgs;
		if (!SpliteWork(orgImg, spliteImgs))
			break;


		ProcessSplitedImage(spliteImgs);
	} while (FALSE);

	UpdateTriggerWorkProgress();
	m_curFileIdx++;
	return step;
}

UINT CSpliteTask::OnPrepareStopWork(UINT step)
{
	return CTriggerTask::OnPrepareStopWork(step);
}

BOOL CSpliteTask::SpliteWork(Mat& orgImg, vector<Mat>& spliteMats)
{
	// 去除边框
	if ((orgImg.rows < SPLITE_BORDER_LINES) || (orgImg.cols < SPLITE_BORDER_LINES))
		return FALSE;

	Mat noneBorderImg(orgImg.rows, orgImg.cols, orgImg.type(), Scalar(255, 255, 255));

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
	if (!SeparatorMat(binImg, splitLines))
		return FALSE;
	
		
	for (int i = 0; i < splitLines.size(); i++)
	{
		int colStart = splitLines[i][0];
		int colEnd = splitLines[i][1];
		Mat tmp = binImg(Range::all(), Range(colStart, colEnd));
		Mat spliteImg = tmp;
		Vec3i rowLines;
		if (FindImgRowStartAndEnd(tmp, rowLines))
		{
			spliteImg = tmp(Range(rowLines[0], rowLines[1]), Range::all());

			Mat minRotateImg = spliteImg;
			int minAngle = 0;
			int minCols = spliteImg.cols;
			for (int angle = SPLITE_IMAGE_ROTATE_ANGLE_MIN; angle <= SPLITE_IMAGE_ROTATE_ANGLE_MAX; angle++)
			{
				Mat rotateImg;
				Vec3i spliteCol;
				RotateMat(spliteImg, rotateImg, angle);
				FindImgColStartAndEnd(rotateImg, spliteCol);
				// y轴占用最小，图像旋转后可能带来噪声，因此需要滤波
				if ((spliteCol[2] < minCols) && (spliteCol[2] > SPLITE_IMAGE_AVAILABLE_WIDTH(rotateImg.cols)))
				{
					minAngle = angle;
					minCols = spliteCol[2];
					minRotateImg = rotateImg;
				}
			}

			spliteImg = minRotateImg;
		}

		resize(spliteImg, spliteImg, Size(SPLITE_IMAGE_RESIZE_WIDTH, SPLITE_IMAGE_RESIZE_HEIGHT));
		spliteMats.push_back(spliteImg);
	}
	
	return TRUE;
}

BOOL CSpliteTask::ProcessSplitedImage(vector<Mat>& spliteMats)
{
	QString fileName = m_fileNameList[m_curFileIdx];

	QFileInfo fileInfo(fileName);
	QString fileBaseName = fileInfo.baseName();

	QString spliteOutFileNamePrefix = m_spliteOutDir + "/" + fileBaseName;
	char szSpliteFileOutNamePreFix[SPLITE_FILE_NAME_MAX] = { 0 };
	int nameLen = QString2Char(spliteOutFileNamePrefix, szSpliteFileOutNamePreFix);
	if (nameLen < 0)
		return FALSE;

	szSpliteFileOutNamePreFix[nameLen] = '\0';
	for (int i = 0; i < spliteMats.size(); i++)
	{
		Mat spliteImg = spliteMats[i];
		char fileNameOut[SPLITE_FILE_NAME_MAX] = { 0 };
		sprintf_s(fileNameOut, sizeof(fileNameOut) - 1, "%s_%d.jpg", szSpliteFileOutNamePreFix, i);
		imwrite(fileNameOut, spliteImg);
	}

	return TRUE;
}
#if 0
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
			Mat spliteImg = tmp;
			Vec3i rowLines;
			if (FindImgRowStartAndEnd(tmp, rowLines))
			{
				spliteImg = tmp(Range(rowLines[0], rowLines[1]), Range::all());

				Mat minRotateImg = spliteImg;
				int minAngle = 0;
				int minCols = spliteImg.cols;
				for (int angle = SPLITE_IMAGE_ROTATE_ANGLE_MIN; angle <= SPLITE_IMAGE_ROTATE_ANGLE_MAX; angle++)
				{
					Mat rotateImg;
					Vec3i spliteCol;
					RotateMat(spliteImg, rotateImg, angle);
					FindImgColStartAndEnd(rotateImg, spliteCol);
					// y轴占用最小，图像旋转后可能带来噪声，因此需要滤波
					if ((spliteCol[2] < minCols) &&(spliteCol[2] > SPLITE_IMAGE_AVAILABLE_WIDTH(rotateImg.cols)))
					{
						minAngle = angle;
						minCols = spliteCol[2];
						minRotateImg = rotateImg;
					}
				}

				spliteImg = minRotateImg;
			}

			resize(spliteImg, spliteImg, Size(SPLITE_IMAGE_RESIZE_WIDTH, SPLITE_IMAGE_RESIZE_HEIGHT));
			imwrite(fileNameOut, spliteImg);
		}
	}
	return TRUE;
}
#endif

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
			if (i == (binImg.cols - 1))
			{
				++keepStepTimes;
				goto  _SAVE_CHILD;
			}

			if (j < binImg.rows)
			{
				keepStepTimes = 0;
				continue;
			}


			if (++keepStepTimes < IMG_SEPERATOR_WAIT_END_PIXS)
				continue;

		_SAVE_CHILD:
			int endColIndex = i - keepStepTimes + 1;

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


void CSpliteTask::RotateMat(Mat &src, Mat &dst, float angle)
{
	float radian = (float)(angle / 180.0 * CV_PI);

	//填充图像
	int maxBorder = (int)(max(src.cols, src.rows)* 1.414); //即为sqrt(2)*max
	int dx = (maxBorder - src.cols) / 2;
	int dy = (maxBorder - src.rows) / 2;
	copyMakeBorder(src, dst, dy, dy, dx, dx, BORDER_CONSTANT);

	//旋转
	Point2f center((float)(dst.cols / 2), (float)(dst.rows / 2));
	Mat affine_matrix = getRotationMatrix2D(center, angle, 1.0);//求得旋转矩阵
	warpAffine(dst, dst, affine_matrix, dst.size());

	//计算图像旋转之后包含图像的最大的矩形
	float sinVal = abs(sin(radian));
	float cosVal = abs(cos(radian));
	Size targetSize((int)(src.cols * cosVal + src.rows * sinVal),
		(int)(src.cols * sinVal + src.rows * cosVal));

	//剪掉多余边框
	int x = (dst.cols - targetSize.width) / 2;
	int y = (dst.rows - targetSize.height) / 2;
	Rect rect(x, y, targetSize.width, targetSize.height);
	dst = Mat(dst, rect);
}


BOOL CSpliteTask::FindImgColStartAndEnd(Mat const& orgImg, Vec3i& splitLine)
{
	SEPERATOR_STEP findStep = SEPERATOR_STEP_FINDING_START;
	int keepStepTimes = 0;
	int startColIndex = 0;


	if ((orgImg.cols < IMG_SEPERATOR_WAIT_START_PIXS) || (orgImg.cols < IMG_SEPERATOR_WAIT_END_PIXS))
		return false;


	// 分割字符
	for (int i = 0; i < orgImg.cols; i++)
	{
		int j = 0;
		for (; j < orgImg.rows; j++)
		{
			if (orgImg.at<unsigned char>(j, i))
			{
				continue;
			}

			if ((j > 0) && !orgImg.at<unsigned char>(j - 1, i))
				break;

			if ((j > 0) && (i > 0) && !orgImg.at<unsigned char>(j - 1, i - 1))
				break;

			if ((j > 0) && (i < (orgImg.cols - 1)) && !orgImg.at<unsigned char>(j - 1, i + 1))
				break;

			if ((j < (orgImg.rows - 1)) && (i < (orgImg.cols - 1)) && !orgImg.at<unsigned char>(j + 1, i + 1))
				break;
		}

		if (findStep == SEPERATOR_STEP_FINDING_START)
		{
			if (j >= orgImg.rows)
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
			if (i == (orgImg.cols - 1))
			{
				++keepStepTimes;
				goto  _SAVE_CHILD;
			}

			if (j < orgImg.rows)
			{
				keepStepTimes = 0;
				continue;
			}


			if (++keepStepTimes < IMG_SEPERATOR_WAIT_END_PIXS)
				continue;

		_SAVE_CHILD:
			int endColIndex = i - keepStepTimes + 1;

			splitLine[0] = startColIndex;
			splitLine[1] = endColIndex + 1;
			splitLine[2] = endColIndex - startColIndex + 1;

			keepStepTimes = 0;
			findStep = SEPERATOR_STEP_FINDING_START;
			break;
		}
	}

	return true;
}

BOOL CSpliteTask::FindImgRowStartAndEnd(Mat const& orgImg, Vec3i& splitLine)
{
	SEPERATOR_STEP findStep = SEPERATOR_STEP_FINDING_START;
	int keepStepTimes = 0;
	int startRowIndex = 0;


	if ((orgImg.rows < IMG_SEPERATOR_WAIT_START_PIXS) || (orgImg.rows < IMG_SEPERATOR_WAIT_END_PIXS))
		return false;

	for (int i = 0; i < orgImg.rows; i++)
	{
		int j = 0;
		for (; j < orgImg.cols; j++)
		{
			if (orgImg.at<unsigned char>(i, j))
			{
				continue;
			}

			if ((j > 0) && !orgImg.at<unsigned char>(i, j - 1))
				break;

			if ((j > 0) && (i > 0) && !orgImg.at<unsigned char>(i - 1, j - 1))
				break;

			if ((j > 0) && (i < (orgImg.rows - 1)) && !orgImg.at<unsigned char>(i + 1, j - 1))
				break;

			if ((j < (orgImg.cols - 1)) && (i < (orgImg.rows - 1)) && !orgImg.at<unsigned char>(i + 1, j + 1))
				break;
		}

		if (findStep == SEPERATOR_STEP_FINDING_START)
		{
			if (j >= orgImg.cols)
			{
				keepStepTimes = 0;
				continue;
			}


			if (++keepStepTimes < IMG_SEPERATOR_WAIT_START_PIXS)
				continue;


			// 找到	
			startRowIndex = i - IMG_SEPERATOR_WAIT_START_PIXS + 1;
			keepStepTimes = 0;
			findStep = SEPERATOR_STEP_FINDING_END;
		}
		else
		{
			if (i == (orgImg.rows - 1))
			{
				++keepStepTimes;
				goto  _SAVE_CHILD;
			}

			if (j < orgImg.cols)
			{
				keepStepTimes = 0;
				continue;
			}


			if (++keepStepTimes < IMG_SEPERATOR_WAIT_END_PIXS+5)
				continue;

		_SAVE_CHILD:
			int endRowIndex = i - keepStepTimes + 1;


			splitLine[0] = startRowIndex;
			splitLine[1] = endRowIndex + 1;
			splitLine[2] = endRowIndex - startRowIndex + 1;

			keepStepTimes = 0;
			findStep = SEPERATOR_STEP_FINDING_START;
			break;
		}
	}

	return true;
}

