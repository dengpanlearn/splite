#ifndef __IDENTITY_WIDGET_H__
#define __IDENTITY_WIDGET_H__


#include <qwidget.h>
class QLineEdit;
class QPushButton;
class QLabel;
class QProgressBar;
class QTimer;
class CIdentityWidget : public QWidget
{
	Q_OBJECT
public:
	CIdentityWidget(QWidget* parent = NULL);
	~CIdentityWidget();


private:
	void RetranslateUi();

private slots:
	void OnSelectInFile(bool checked);
	void OnSelectInTrain(bool checked);
	void OnStartIdentity(bool checked);
	void OnTimeoutIdentity();

private:
	QLineEdit*		m_pLineEditInFile;
	QLineEdit*		m_pLineEditInTrain;
	QLineEdit*		m_pLineEditOutIdentity;

	QPushButton*	m_pBtnInView;
	QPushButton*	m_pBtnTrainView;
	QLabel*			m_pLabInImg;

	QPushButton*	m_pBtnStart;
	QProgressBar*	m_pProgressIdentity;
	QTimer*			m_pTimerIdentity;
};

#endif // !__IDENTITY_WIDGET_H__


