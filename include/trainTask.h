/*trainTask.h
*/

#ifndef __TRAIN_TASK_H__
#define	__TRAIN_TASK_H__

#include <qstringlist.h>
#include <qstring.h>
#include "triggerTask.h"


class TRAIN_TASK_ARG
{
public:
	TRAIN_TASK_ARG(QStringList& fileList, QString& trainOutfile) { m_fileList = fileList; m_outFile = trainOutfile; };
	~TRAIN_TASK_ARG() {}

public:
	inline QStringList GetFileList()const { return m_fileList; }
	inline QString GetOutFile()const { return m_outFile; }

	QStringList		m_fileList;
	QString			m_outFile;
};

class CTrainTask :
	public CTriggerTask
{
public:
	CTrainTask();
	virtual ~CTrainTask();

	BOOL Create(int pri, int stackSize, int timeoutMs);
	void Close();

private:
	static int VertifyTrainFiles(QStringList const&  fileList, QStringList& availFileList);
	
protected:
	virtual BOOL PrepareTriggerParam(void* pArg);
	virtual UINT OnPrepareStartWork(UINT step);
	virtual UINT OnTimeoutWork(UINT step);
	virtual UINT OnPrepareStopWork(UINT step);

private:
	QStringList		m_trainList;
	QString			m_trainOutFile;
	int				m_trainMatCols;
};

#endif // !__TRAIN_TASK_H__