/*spliteTask.h
*
*/

#ifndef __SPLITE_TASK_H
#define	__SPLITE_TASK_H

#include <dp.h>
#include <triggerTask.h>
#include <qstringlist.h>

class SPLITE_TASK_ARG
{
public:
	SPLITE_TASK_ARG(QStringList& fileList, QString& outDir) { m_fileList = fileList; m_outDir = outDir; };
	~SPLITE_TASK_ARG(){}

public:
	inline QStringList GetFileList()const { return m_fileList; }
	inline QString GetOutDir()const { return m_outDir; }

	QStringList		m_fileList;
	QString			m_outDir;
};

using cv::Mat;
using cv::Vec3i;
using std::vector;

class CSpliteTask :
	public CTriggerTask
{
public:
	CSpliteTask();
	virtual ~CSpliteTask();

	BOOL Create(int pri, int stackSize, int timeoutMs);
	void Close();


protected:
	virtual BOOL PrepareTriggerParam(void* pArg);
	virtual UINT OnPrepareStartWork(UINT step);
	virtual UINT OnTimeoutWork(UINT step);
	virtual UINT OnPrepareStopWork(UINT step);


private:

	enum SEPERATOR_STEP
	{
		SEPERATOR_STEP_FINDING_START = 0,
		SEPERATOR_STEP_FINDING_END = 1
	};

	BOOL SpliteWork(const char* pFileName, const char* pOutFileNamePrefix);

	BOOL SeparatorMat(Mat const& binImg, vector<Vec3i>& splitLines);

private:
	QStringList		m_fileNameList;
	QString			m_spliteOutDir;
	UINT			m_curFileIdx;
};

#endif // !__SPLITE_TASK_H