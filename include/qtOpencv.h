#pragma once

#include <qmainwindow.h>
class QAction;
class CSpliteWidget;

class CQtOpencv : public QMainWindow
{
    Q_OBJECT

public:
    CQtOpencv(QWidget *parent = Q_NULLPTR);

protected:
	virtual void closeEvent(QCloseEvent *closeEvent);

private:
	void RetranslateInit();

private slots:
void OnEnterSplite(bool check);

private:
	QAction*		m_pActSpliteForMark;
	CSpliteWidget*	m_pSpliteWidget;
	QWidget*		m_pCurWidget;
};
