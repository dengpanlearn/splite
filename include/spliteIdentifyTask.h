/*
* spliteIdentityTask.h
]*/


#ifndef __SPLITE_IDENTITY_TASK_H__
#define	__SPLITE_IDENTITY_TASK_H__

#include "../include/spliteTask.h"

class SPLITE_IDENTITY_TASK_ARG
{
public:
	SPLITE_IDENTITY_TASK_ARG(QString& toIdentityFileName, QString const& trainFileName,int maxSpliteCounts) 
	{ 
		m_toIdentityFileName = toIdentityFileName; 
		m_trainFileName = trainFileName;
		m_maxSpliteCounts = maxSpliteCounts; 
	
	}

	~SPLITE_IDENTITY_TASK_ARG() {}

public:
	inline QString GetToIdentityFileName()const { return m_toIdentityFileName; }
	inline QString GetTrainFileName()const { return m_trainFileName; }
	inline int GetMaxSpliteCounts()const { return m_maxSpliteCounts; }

	QString			m_toIdentityFileName;
	QString			m_trainFileName;
	int				m_maxSpliteCounts;
};

class CSpliteIdentifyTask :
	public CSpliteTask
{
public:
	CSpliteIdentifyTask();
	~CSpliteIdentifyTask();

	BOOL Create(int pri, int stackSize, int timeoutMs);
	void Close();

public:
	inline BOOL GetIdentityResult(char* pBuf, int size);

protected:
	virtual BOOL PrepareTriggerParam(void* pArg);
	virtual UINT OnPrepareStartWork(UINT step);
	virtual UINT OnTimeoutWork(UINT step);
	virtual UINT OnPrepareStopWork(UINT step);

	virtual BOOL ProcessSplitedImage(vector<Mat>& spliteMats);

private:
	QString		m_toIdentityFileName;
	QString		m_trainFileName;
	int			m_maxSpliteCounts;
	char*		m_pTrainOutput;
	
};

inline BOOL CSpliteIdentifyTask::GetIdentityResult(char* pBuf, int size)
{
	CSingleLock loc(GetMutex(), TRUE);
	if (!IsTriggerEnd())
		return FALSE;

	strncpy(pBuf, m_pTrainOutput, size - 1);
	return TRUE;
}
#endif // !__SPLITE_IDENTITY_TASK_H__