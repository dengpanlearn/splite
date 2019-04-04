/*qtOpencv.h*/

#ifndef __QT_OPENCV_H__

#define	__QT_OPENCV_H__



#include <qmainwindow.h>
class QAction;
class CSpliteWidget;
class CTrainWidget;
class CIdentityWidget;
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
	void OnEnterTrain(bool check);
	void OnEnterIdentity(bool check);

private:
	QAction*		m_pActSpliteForMark;
	QAction*		m_pActTrain;
	QAction*		m_pActIdentity;
	CSpliteWidget*	m_pSpliteWidget;
	CTrainWidget*	m_pTrainWidget;
	CIdentityWidget*	m_pIdentityWidget;
};
#endif // !__QT_OPENCV_H__