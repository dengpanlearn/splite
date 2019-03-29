#ifndef __SPLITE_WIDGET_H__
#define __SPLITE_WIDGET_H__


#include <qwidget.h>
class QLineEdit;
class QPushButton;
class QLabel;
class QProgressBar;
class QTimer;
class CSpliteWidget : public QWidget
{
	Q_OBJECT
public:
	CSpliteWidget(QWidget* parent = NULL);
	~CSpliteWidget();


private:
	void RetranslateUi();

private slots:
	void OnSelectInDir(bool checked);
	void OnSelectOutDir(bool checked);
	void OnStartSplite(bool checked);
	void OnTimeoutSplite();

private:
	QLineEdit*		m_pLineEditInDir;
	QLineEdit*		m_pLineEditOutDir;

	QPushButton*	m_pBtnInView;
	QPushButton*	m_pBtnOutView;


	QLineEdit*		m_pLineEditInNum;
	QLineEdit*		m_pLineEditSuccessNum;
	QPushButton*	m_pBtnStart;
	QProgressBar*	m_pProgressSplite;
	QTimer*			m_pTimerSplite;
};

#endif // !__SPLITE_WIDGET_H__


