/*identityWidget.cpp*/

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
#include <qimage.h>
#include <qtimer.h>
#include <qpixmap.h>
#include <qmessagebox.h>
#include "../include/identityWidget .h"
#include "../include/qtResourceDef.h"
#include "../include/global.h"
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "../include/spliteIdentifyTask.h"

extern  CSpliteIdentifyTask	g_spliteIdentityTask;

CIdentityWidget::CIdentityWidget(QWidget* parent):QWidget(parent)
{
	m_pTimerIdentity = new QTimer(this);
	m_pTimerIdentity->setInterval(500);
	QTextCodec* pTextCodec = QTextCodec::codecForLocale();
	QLabel* pLabFileIn = new QLabel(pTextCodec->toUnicode(WIDGET_IDENTITY_LAB_IN_FILE));
	m_pLineEditInFile = new QLineEdit();
	m_pBtnInView = new QPushButton(pTextCodec->toUnicode(WIDGET_IDENTITY_BTN_VIEW));

	QHBoxLayout* pLytInFile = new QHBoxLayout();
	pLytInFile->addWidget(pLabFileIn);
	pLytInFile->addWidget(m_pLineEditInFile);
	pLytInFile->addWidget(m_pBtnInView);


	QLabel* pLabFileTrain = new QLabel(pTextCodec->toUnicode(WIDGET_IDENTITY_LAB_TRAIN_FILE));
	m_pLineEditInTrain = new QLineEdit();
	m_pBtnTrainView = new QPushButton(pTextCodec->toUnicode(WIDGET_IDENTITY_BTN_VIEW));

	QHBoxLayout* pLytInTrain = new QHBoxLayout();
	pLytInTrain->addWidget(pLabFileTrain);
	pLytInTrain->addWidget(m_pLineEditInTrain);
	pLytInTrain->addWidget(m_pBtnTrainView);


	QLabel* pLabFile= new QLabel(pTextCodec->toUnicode(WIDGET_IDENTITY_LAB_FILE));

	m_pLabInImg = new QLabel();
	m_pLabInImg->setObjectName("Identity_File_Img");
	m_pBtnStart = new QPushButton(pTextCodec->toUnicode(WIDGET_IDENTITY_BTN_START));

	QHBoxLayout* pLytFile = new QHBoxLayout();
	pLytFile->addWidget(pLabFile);
	pLytFile->addWidget(m_pLabInImg);
	pLytFile->addWidget(m_pBtnStart);

	QLabel* pLabIdentity= new QLabel(pTextCodec->toUnicode(WIDGET_IDENTITY_LAB_IDENTITY_RESULT));

	m_pLineEditOutIdentity = new QLineEdit();
	m_pLineEditOutIdentity->setObjectName("Identity_Identity_out");
	m_pProgressIdentity = new QProgressBar();
	m_pProgressIdentity->setVisible(false);
	m_pProgressIdentity->setObjectName("Identity_Progress");


	QHBoxLayout* pLytResult = new QHBoxLayout();
	pLytResult->addWidget(pLabIdentity);
	pLytResult->addWidget(m_pLineEditOutIdentity);
	pLytResult->addWidget(m_pProgressIdentity);
	pLytResult->setAlignment(pLabIdentity, Qt::AlignLeft);
	pLytResult->setAlignment(m_pLineEditOutIdentity, Qt::AlignLeft);
	pLytResult->setAlignment(m_pProgressIdentity, Qt::AlignRight);
	QVBoxLayout* pLytMain = new QVBoxLayout();
	pLytMain->addLayout(pLytInFile);
	pLytMain->addLayout(pLytInTrain);
	pLytMain->addLayout(pLytFile);
	pLytMain->addLayout(pLytResult);
	this->setLayout(pLytMain);

	pLytMain->setAlignment(pLytInFile, Qt::AlignHCenter | Qt::AlignTop);
	pLytMain->setAlignment(pLytInTrain, Qt::AlignHCenter | Qt::AlignTop);
	pLytMain->setAlignment(pLytFile, Qt::AlignTop | Qt::AlignHCenter);
	pLytMain->setAlignment(pLytResult, Qt::AlignCenter);

	RetranslateUi();
}


CIdentityWidget::~CIdentityWidget()
{
}


void CIdentityWidget::RetranslateUi()
{

	connect(m_pBtnInView, SIGNAL(clicked(bool)), this, SLOT(OnSelectInFile(bool)));
	connect(m_pBtnTrainView, SIGNAL(clicked(bool)), this, SLOT(OnSelectInTrain(bool)));
	connect(m_pBtnStart, SIGNAL(clicked(bool)), this, SLOT(OnStartIdentity(bool)));
	connect(m_pTimerIdentity, SIGNAL(timeout()), this, SLOT(OnTimeoutIdentity()));
}

void CIdentityWidget::OnSelectInFile(bool checked)
{
	QTextCodec* pTextCodec = QTextCodec::codecForLocale();
	QString defaultFile = m_pLineEditInFile->text();
	QString defaultFileDir;
	if (defaultFile.isEmpty())
		defaultFileDir = QDir::currentPath();
	else
	{
		defaultFileDir = QFileInfo(defaultFile).absolutePath();
	}

	QString filePath = QFileDialog::getOpenFileName(this, pTextCodec->toUnicode("选择待识别图片"), defaultFileDir, "Images(*.png *.jpg)");
	if (!filePath.isEmpty())
	{
		m_pLineEditInFile->setText(filePath);
		QImage img(filePath);
		m_pLabInImg->setPixmap(QPixmap::fromImage(img));
	}
}

void CIdentityWidget::OnSelectInTrain(bool checked)
{
	QTextCodec* pTextCodec = QTextCodec::codecForLocale();
	QString defaultFile = m_pLineEditInTrain->text();
	QString defaultFileDir;
	if (defaultFile.isEmpty())
		defaultFileDir = QDir::currentPath();
	else
	{
		defaultFileDir = QFileInfo(defaultFile).absolutePath();
	}

	QString filePath = QFileDialog::getOpenFileName(this, pTextCodec->toUnicode("选择待识别图片"), defaultFileDir, "xml(*.xml)");
	if (!filePath.isEmpty())
	{
		m_pLineEditInTrain->setText(filePath);
	}
}

void CIdentityWidget::OnStartIdentity(bool checked)
{
	QString selectInFile = m_pLineEditInFile->text();
	QString selectInTrain = m_pLineEditInTrain->text();
	if (!selectInFile.isEmpty() && !selectInTrain.isEmpty())
	{
		SPLITE_IDENTITY_TASK_ARG taskArg(selectInFile, selectInTrain, 6);
		if (!g_spliteIdentityTask.StartTrigger(&taskArg))
		{
			QTextCodec* pTextCodec = QTextCodec::codecForLocale();
			QMessageBox msgBox;

			msgBox.setWindowTitle(pTextCodec->toUnicode("提示"));
			msgBox.setText(pTextCodec->toUnicode("识别失败"));

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
		else
		{
			m_pBtnInView->setEnabled(FALSE);
			m_pBtnTrainView->setEnabled(FALSE);
			m_pBtnStart->setEnabled(FALSE);
			m_pProgressIdentity->setMaximum(1);
			m_pTimerIdentity->start();
			m_pLineEditOutIdentity->setText("");
		}
	}
	else {
	
	}
}

void CIdentityWidget::OnTimeoutIdentity()
{
	int val = g_spliteIdentityTask.GetTriggerWorkProgress();
	
	m_pProgressIdentity->setValue(val);
	if (g_spliteIdentityTask.IsTriggerEnd())
	{
		m_pProgressIdentity->setVisible(FALSE);

		QTextCodec* pTextCodec = QTextCodec::codecForLocale();
		QMessageBox msgBox;

		msgBox.setWindowTitle(pTextCodec->toUnicode("提示"));
		msgBox.setText(pTextCodec->toUnicode("识别完成"));
		
		char result[20] = { 0 };
		g_spliteIdentityTask.GetIdentityResult(result, 20);
		m_pLineEditOutIdentity->setText(result);
		g_spliteIdentityTask.StopTrigger();
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
		m_pBtnTrainView->setEnabled(TRUE);
		m_pTimerIdentity->stop();
	}
	else if (g_spliteIdentityTask.IsTriggerStop())
	{
		QMessageBox msgBox;
		QTextCodec* pTextCodec = QTextCodec::codecForLocale();
		msgBox.setWindowTitle(pTextCodec->toUnicode("提示"));
		msgBox.setText(pTextCodec->toUnicode("识别失败"));

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
		m_pBtnTrainView->setEnabled(TRUE);
		m_pTimerIdentity->stop();
	}
}