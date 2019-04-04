/*spliteWidget.cpp*/

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
#include "../include/spliteWidget.h"
#include "../include/qtResourceDef.h"
#include "../include/global.h"
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "../include/spliteTask.h"

extern  CSpliteTask	g_spliteTask;

CSpliteWidget::CSpliteWidget(QWidget* parent):QWidget(parent)
{
	m_pTimerSplite = new QTimer(this);
	m_pTimerSplite->setInterval(500);
	QTextCodec* pTextCodec = QTextCodec::codecForLocale();
	QLabel* pLabDirIn = new QLabel(pTextCodec->toUnicode(WIDGET_SPLITE_LAB_IN_DIR));
	m_pLineEditInDir = new QLineEdit();
	m_pBtnInView = new QPushButton(pTextCodec->toUnicode(WIDGET_SPLITE_BTN_VIEW));

	QHBoxLayout* pLytInDir = new QHBoxLayout();
	pLytInDir->addWidget(pLabDirIn);
	pLytInDir->addWidget(m_pLineEditInDir);
	pLytInDir->addWidget(m_pBtnInView);


	QLabel* pLabDirOut = new QLabel(pTextCodec->toUnicode(WIDGET_SPLITE_LAB_OUT_DIR));
	m_pLineEditOutDir = new QLineEdit();
	m_pBtnOutView = new QPushButton(pTextCodec->toUnicode(WIDGET_SPLITE_BTN_VIEW));

	QHBoxLayout* pLytOutDir = new QHBoxLayout();
	pLytOutDir->addWidget(pLabDirOut);
	pLytOutDir->addWidget(m_pLineEditOutDir);
	pLytOutDir->addWidget(m_pBtnOutView);

	QLabel*	pLabInNum= new QLabel(pTextCodec->toUnicode(WIDGET_SPLITE_LAB_IN_NUM));
	m_pLineEditInNum = new QLineEdit();
	m_pLineEditInNum->setText(QString::number(0));
	m_pLineEditInNum->setEnabled(false);
	pLabInNum->setObjectName("LabInNum");
	m_pLineEditInNum->setObjectName("EditInNum");
	QHBoxLayout* pLytInNum = new QHBoxLayout();
	pLytInNum->addWidget(pLabInNum);
	pLytInNum->addWidget(m_pLineEditInNum);
	

	QLabel*	pLabSuccess = new QLabel(pTextCodec->toUnicode(WIDGET_SPLITE_LAB_SUCCESS_NUM));
	m_pLineEditSuccessNum = new QLineEdit();
	m_pLineEditSuccessNum->setText(QString::number(0));
	pLabSuccess->setObjectName("LabSuccess");
	m_pLineEditSuccessNum->setObjectName("EditSuccess");
	m_pLineEditSuccessNum->setEnabled(false);
	QHBoxLayout* pLytSuccessNum = new QHBoxLayout();
	pLytSuccessNum->addWidget(pLabSuccess);
	pLytSuccessNum->addWidget(m_pLineEditSuccessNum);

	m_pBtnStart = new QPushButton(pTextCodec->toUnicode(WIDGET_SPLITE_BTN_START));
	QHBoxLayout* pLytSplite = new QHBoxLayout();
	pLytSplite->addLayout(pLytInNum);
	pLytSplite->addLayout(pLytSuccessNum);
	pLytSplite->addWidget(m_pBtnStart);
	pLytSplite->setAlignment(pLytInNum, Qt::AlignCenter);
	pLytSplite->setAlignment(pLytSuccessNum, Qt::AlignCenter);
	pLytSplite->setAlignment(m_pBtnStart, Qt::AlignRight);

	m_pProgressSplite = new QProgressBar();
	m_pProgressSplite->setVisible(false);
	QVBoxLayout* pLytMain = new QVBoxLayout();
	pLytMain->addLayout(pLytInDir);
	pLytMain->addLayout(pLytOutDir);
	pLytMain->addLayout(pLytSplite);
	pLytMain->addWidget(m_pProgressSplite);
	pLytMain->setAlignment(pLytInDir, Qt::AlignHCenter|Qt::AlignTop);
	pLytMain->setAlignment(pLytOutDir, Qt::AlignHCenter | Qt::AlignTop);
	pLytMain->setAlignment(pLytSplite, Qt::AlignTop |Qt::AlignHCenter);
	pLytMain->setAlignment(m_pProgressSplite, Qt::AlignCenter);
	this->setLayout(pLytMain);

	RetranslateUi();
}


CSpliteWidget::~CSpliteWidget()
{
}


void CSpliteWidget::RetranslateUi()
{
#if 0
	QTextCodec* pTextCodec = QTextCodec::codecForLocale();
	QFile qssFile(pTextCodec->toUnicode(":/widget/spliteWidget.qss"));
	if (qssFile.open(QFile::ReadOnly))
	{
		qApp->setStyleSheet(qssFile.readAll());
		qssFile.close();
	}
#endif
	connect(m_pBtnInView, SIGNAL(clicked(bool)), this, SLOT(OnSelectInDir(bool)));
	connect(m_pBtnOutView, SIGNAL(clicked(bool)), this, SLOT(OnSelectOutDir(bool)));
	connect(m_pBtnStart, SIGNAL(clicked(bool)), this, SLOT(OnStartSplite(bool)));
	connect(m_pTimerSplite, SIGNAL(timeout()), this, SLOT(OnTimeoutSplite()));
}

void CSpliteWidget::OnSelectInDir(bool checked)
{
	QTextCodec* pTextCodec = QTextCodec::codecForLocale();
	QString defaultDir = m_pLineEditInDir->text();
	if (defaultDir.isEmpty())
		defaultDir = QDir::currentPath();
	QString selectDirPath = QFileDialog::getExistingDirectory(this, pTextCodec->toUnicode("选择待分割的图片路径"), defaultDir, QFileDialog::ShowDirsOnly
		| QFileDialog::DontResolveSymlinks);

	if (!selectDirPath.isEmpty())
	{
		m_pLineEditInDir->setText(selectDirPath);	
	}
}

void CSpliteWidget::OnSelectOutDir(bool checked)
{
	QTextCodec* pTextCodec = QTextCodec::codecForLocale();
	QString defaultDir = m_pLineEditOutDir->text();
	if (defaultDir.isEmpty())
		defaultDir = QDir::currentPath();
	QString selectDir = QFileDialog::getExistingDirectory(this, pTextCodec->toUnicode("选择图片分割后存储路径"), defaultDir, QFileDialog::ShowDirsOnly
		| QFileDialog::DontResolveSymlinks);

	if (!selectDir.isEmpty())
	{
		m_pLineEditOutDir->setText(selectDir);
	}
}

void CSpliteWidget::OnStartSplite(bool checked)
{
	QString selectDirPath = m_pLineEditInDir->text();
	QString selectOutDir = m_pLineEditOutDir->text();
	if (!selectDirPath.isEmpty() && !selectOutDir.isEmpty())
	{
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
		SPLITE_TASK_ARG spliteArg(selectedFilesPath, selectOutDir);
		if (g_spliteTask.StartTrigger(&spliteArg))
		{
			m_pProgressSplite->setVisible(TRUE);
			m_pProgressSplite->setValue(0);
			m_pProgressSplite->setMaximum(selectedFilesCount);
			m_pBtnStart->setEnabled(FALSE);
			m_pBtnInView->setEnabled(FALSE);
			m_pBtnOutView->setEnabled(FALSE);


			m_pTimerSplite->start();
		}
		else {
			QTextCodec* pTextCodec = QTextCodec::codecForLocale();
			QMessageBox msgBox;

			msgBox.setWindowTitle(pTextCodec->toUnicode("提示"));
			msgBox.setText(pTextCodec->toUnicode("分割失败"));

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
	else
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
	}
}

void CSpliteWidget::OnTimeoutSplite()
{
	int val = g_spliteTask.GetTriggerWorkProgress();
	m_pLineEditSuccessNum->setText(QString::number(val));
	m_pProgressSplite->setValue(val);
	if (g_spliteTask.IsTriggerStop())
	{
		m_pProgressSplite->setVisible(FALSE);

		QTextCodec* pTextCodec = QTextCodec::codecForLocale();
		QMessageBox msgBox;

		msgBox.setWindowTitle(pTextCodec->toUnicode("提示"));
		msgBox.setText(pTextCodec->toUnicode("分割完成"));
		
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
		m_pTimerSplite->stop();
	}
}