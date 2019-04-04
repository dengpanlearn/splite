#include <qlineedit.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtextcodec.h>
#include <qapplication.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qdir.h>
#include <qprogressbar.h>
#include <qfiledialog.h>
#include <qtimer.h>
#include <qmessagebox.h>
#include "../include/qtResourceDef.h"
#include "../include/trainWidget.h"
#include "../include/global.h"
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "../include/trainTask.h"

extern CTrainTask	g_trainTask;

CTrainWidget::CTrainWidget(QWidget *parent)
	: QWidget(parent)
{
	m_pTimerTrain = new QTimer(this);
	m_pTimerTrain->setInterval(500);
	QTextCodec* pTextCodec = QTextCodec::codecForLocale();
	QLabel* pLabDirIn = new QLabel(pTextCodec->toUnicode(WIDGET_TRAIN_LAB_IN_DIR));
	m_pLineEditInDir = new QLineEdit();
	m_pBtnInView = new QPushButton(pTextCodec->toUnicode(WIDGET_TRAIN_BTN_VIEW));

	QHBoxLayout* pLytInDir = new QHBoxLayout();
	pLytInDir->addWidget(pLabDirIn);
	pLytInDir->addWidget(m_pLineEditInDir);
	pLytInDir->addWidget(m_pBtnInView);


	QLabel* pLabFileOut = new QLabel(pTextCodec->toUnicode(WIDGET_TRAIN_LAB_OUT_SAVE_FILE));
	m_pLineEditOutFile = new QLineEdit();
	m_pBtnOutView = new QPushButton(pTextCodec->toUnicode(WIDGET_TRAIN_BTN_VIEW));

	QHBoxLayout* pLytOutDir = new QHBoxLayout();
	pLytOutDir->addWidget(pLabFileOut);
	pLytOutDir->addWidget(m_pLineEditOutFile);
	pLytOutDir->addWidget(m_pBtnOutView);

	QLabel*	pLabInNum = new QLabel(pTextCodec->toUnicode(WIDGET_TRAIN_LAB_IN_NUM));
	m_pLineEditInNum = new QLineEdit();
	m_pLineEditInNum->setText(QString::number(0));
	m_pLineEditInNum->setEnabled(false);
	pLabInNum->setObjectName("LabInNum");
	m_pLineEditInNum->setObjectName("EditInNum");
	QHBoxLayout* pLytInNum = new QHBoxLayout();
	pLytInNum->addWidget(pLabInNum);
	pLytInNum->addWidget(m_pLineEditInNum);


	QLabel*	pLabSuccess = new QLabel(pTextCodec->toUnicode(WIDGET_TRAIN_LAB_SUCCESS_NUM));
	m_pLineEditSuccessNum = new QLineEdit();
	m_pLineEditSuccessNum->setText(QString::number(0));
	pLabSuccess->setObjectName("LabSuccess");
	m_pLineEditSuccessNum->setObjectName("EditSuccess");
	m_pLineEditSuccessNum->setEnabled(false);
	QHBoxLayout* pLytSuccessNum = new QHBoxLayout();
	pLytSuccessNum->addWidget(pLabSuccess);
	pLytSuccessNum->addWidget(m_pLineEditSuccessNum);



	m_pBtnStart = new QPushButton(pTextCodec->toUnicode(WIDGET_TRAIN_BTN_START));
	QHBoxLayout* pLytTrain = new QHBoxLayout();
	pLytTrain->addLayout(pLytInNum);
	pLytTrain->addLayout(pLytSuccessNum);
	pLytTrain->addWidget(m_pBtnStart);
	pLytTrain->setAlignment(pLytInNum, Qt::AlignCenter);
	pLytTrain->setAlignment(pLytSuccessNum, Qt::AlignCenter);
	pLytTrain->setAlignment(m_pBtnStart, Qt::AlignRight);

	m_pProgressTrain = new QProgressBar();
	m_pProgressTrain->setVisible(false);

	QVBoxLayout* pLytMain = new QVBoxLayout();
	pLytMain->addLayout(pLytInDir);
	pLytMain->addLayout(pLytOutDir);
	pLytMain->addLayout(pLytTrain);
	pLytMain->addWidget(m_pProgressTrain);

	pLytMain->setAlignment(pLytInDir, Qt::AlignHCenter | Qt::AlignTop);
	pLytMain->setAlignment(pLytOutDir, Qt::AlignHCenter | Qt::AlignTop);
	pLytMain->setAlignment(pLytTrain, Qt::AlignTop | Qt::AlignHCenter);
	pLytMain->setAlignment(m_pProgressTrain, Qt::AlignHCenter | Qt::AlignTop);
	this->setLayout(pLytMain);
	RetranslateUi();
}

CTrainWidget::~CTrainWidget()
{
}


void CTrainWidget::RetranslateUi()
{
#if 0
	QTextCodec* pTextCodec = QTextCodec::codecForLocale();
	QFile qssFile(pTextCodec->toUnicode(":/widget/spliteWidget.qss"));
	if (qssFile.open(QFile::ReadOnly))
	{
		qApp->setStyleSheet(qssFile.readAll());
		qssFile.close();
	}
	connect(m_pBtnInView, SIGNAL(clicked(bool)), this, SLOT(OnSelectInDir(bool)));
	connect(m_pBtnOutView, SIGNAL(clicked(bool)), this, SLOT(OnSelectOutFile(bool)));
	connect(m_pBtnStart, SIGNAL(clicked(bool)), this, SLOT(OnStartTrain(bool)));
	connect(m_pTimerTrain, SIGNAL(timeout()), this, SLOT(OnTimeoutTrain()));
#endif
}


void CTrainWidget::OnSelectInDir(bool checked)
{
	QTextCodec* pTextCodec = QTextCodec::codecForLocale();
	QString defaultDir = m_pLineEditInDir->text();
	if (defaultDir.isEmpty())
		defaultDir = QDir::currentPath();
	QString selectDirPath = QFileDialog::getExistingDirectory(this, pTextCodec->toUnicode("选择待训练的图片路径"), defaultDir, QFileDialog::ShowDirsOnly
		| QFileDialog::DontResolveSymlinks);

	if (!selectDirPath.isEmpty())
	{
		m_pLineEditInDir->setText(selectDirPath);
	}
}

void CTrainWidget::OnSelectOutFile(bool checked)
{
	QTextCodec* pTextCodec = QTextCodec::codecForLocale();
	QString defaultSaveFile = m_pLineEditOutFile->text();
	QString defaultSaveDir;
	if (defaultSaveFile.isEmpty())
		defaultSaveDir = QDir::currentPath();
	else
	{
		QFileInfo fileInfo(defaultSaveFile);
		defaultSaveDir = fileInfo.absolutePath();
	}

	defaultSaveFile = QFileDialog::getSaveFileName(this, pTextCodec->toUnicode("选择待训练的图片路径"), defaultSaveDir, "xml (*.xml)");
	if (!defaultSaveFile.isEmpty())
		m_pLineEditOutFile->setText(defaultSaveFile);
}

void CTrainWidget::OnStartTrain(bool checked)
{
	QString selectDirPath = m_pLineEditInDir->text();
	QString selectOutFile = m_pLineEditOutFile->text();
	if (selectDirPath.isEmpty() || selectOutFile.isEmpty())
	{
		QTextCodec* pTextCodec = QTextCodec::codecForLocale();
		QMessageBox msgBox;

		msgBox.setWindowTitle(pTextCodec->toUnicode("提示"));
		msgBox.setText(pTextCodec->toUnicode("请输入正确路径"));

		msgBox.setIcon(QMessageBox::Information);
		msgBox.addButton(QMessageBox::Ok);
		msgBox.setStyleSheet(
			"QPushButton {"
			"border: 1px solid #555;"
			"padding: 4px;"
			"min-width: 65px;"
			"min-height: 40px;"
			"border - radius:5px;"
			"font-size: 20px;"
			"text-align:center;"
			"}"
			"QPushButton:hover {"
			" background-color: #999;"
			"}"
			"QPushButton:pressed {"
			"background-color: #333;"
			"border-color: #555;"
			"color: beige;"
			"}"

			"QLabel{ min-width: 100px;min-height:50px;font:15px;}"
		);

		msgBox.exec();
		return;
	}

	QDir selectDir = QDir(selectDirPath);
	QStringList fileNameFilter;
	fileNameFilter << "*.png" << "*.jpg";

	QStringList selectedFiles = selectDir.entryList(fileNameFilter, QDir::Files | QDir::NoDotAndDotDot, QDir::Name);
	QStringList selectedFilesPath;
	for (int i = 0; i < selectedFiles.size(); i++)
		selectedFilesPath << (selectDir.filePath(selectedFiles[i]));
	int selectedFilesCount = selectedFiles.size();

	m_pLineEditInNum->setText(QString::number(selectedFilesCount));
	m_pLineEditSuccessNum->setText(QString::number(0));

	TRAIN_TASK_ARG taskArg(selectedFilesPath, selectOutFile);
	if (g_trainTask.StartTrigger(&taskArg))
	{
		m_pProgressTrain->setVisible(TRUE);
		m_pProgressTrain->setValue(0);
		m_pProgressTrain->setMaximum(selectedFilesCount);
		m_pBtnStart->setEnabled(FALSE);
		m_pBtnInView->setEnabled(FALSE);
		m_pBtnOutView->setEnabled(FALSE);


		m_pTimerTrain->start();
	}
	else
	{
		QTextCodec* pTextCodec = QTextCodec::codecForLocale();
		QMessageBox msgBox;

		msgBox.setWindowTitle(pTextCodec->toUnicode("提示"));
		msgBox.setText(pTextCodec->toUnicode("训练失败"));

		msgBox.setIcon(QMessageBox::Information);
		msgBox.addButton(QMessageBox::Ok);
		msgBox.setStyleSheet(
			"QPushButton {"
			"border: 1px solid #555;"
			"padding: 4px;"
			"min-width: 65px;"
			"min-height: 40px;"
			"border - radius:5px;"
			"font-size: 20px;"
			"text-align:center;"
			"}"
			"QPushButton:hover {"
			" background-color: #999;"
			"}"
			"QPushButton:pressed {"
			"background-color: #333;"
			"border-color: #555;"
			"color: beige;"
			"}"

			"QLabel{ min-width: 100px;min-height:50px;font:15px;}"
		);

		msgBox.exec();
	}

}

void CTrainWidget::OnTimeoutTrain()
{
	int val = g_trainTask.GetTriggerWorkProgress();
	m_pLineEditSuccessNum->setText(QString::number(val));
	m_pProgressTrain->setValue(val);
	if (g_trainTask.IsTriggerStop())
	{
		m_pProgressTrain->setVisible(FALSE);

		QTextCodec* pTextCodec = QTextCodec::codecForLocale();
		QMessageBox msgBox;

		msgBox.setWindowTitle(pTextCodec->toUnicode("提示"));
		msgBox.setText(pTextCodec->toUnicode("训练完成"));

		msgBox.setIcon(QMessageBox::Information);
		msgBox.addButton(QMessageBox::Ok);
		msgBox.setStyleSheet(
			"QPushButton {"
			"border: 1px solid #555;"
			"padding: 4px;"
			"min-width: 65px;"
			"min-height: 40px;"
			"border - radius:5px;"
			"font-size: 20px;"
			"text-align:center;"
			"}"
			"QPushButton:hover {"
			" background-color: #999;"
			"}"
			"QPushButton:pressed {"
			"background-color: #333;"
			"border-color: #555;"
			"color: beige;"
			"}"

			"QLabel{ min-width: 100px;min-height:50px;font:15px;}"
		);

		msgBox.exec();
		m_pBtnStart->setEnabled(TRUE);
		m_pBtnInView->setEnabled(TRUE);
		m_pBtnOutView->setEnabled(TRUE);
		m_pTimerTrain->stop();
	}
}