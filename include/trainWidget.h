/*trainWidget.h
*/


#ifndef __TRAIN_WIDGET_H__
#define __TRAIN_WIDGET_H__


#include <QWidget>
class QLineEdit;
class QPushButton;
class QLabel;
class QProgressBar;
class QTimer;

class CTrainWidget : public QWidget
{
	Q_OBJECT

public:
	CTrainWidget(QWidget *parent=NULL);
	~CTrainWidget();

private:
	void RetranslateUi();

private slots:
	void OnSelectInDir(bool checked);
	void OnSelectOutFile(bool checked);
	void OnStartTrain(bool checked);
	void OnTimeoutTrain();

private:
	QLineEdit*		m_pLineEditInDir;
	QLineEdit*		m_pLineEditOutFile;

	QPushButton*	m_pBtnInView;
	QPushButton*	m_pBtnOutView;

	QLineEdit*		m_pLineEditInNum;
	QLineEdit*		m_pLineEditSuccessNum;

	QPushButton*	m_pBtnStart;
	QProgressBar*	m_pProgressTrain;
	QTimer*			m_pTimerTrain;
};
#endif // !__TRAIN_WIDGET_H__