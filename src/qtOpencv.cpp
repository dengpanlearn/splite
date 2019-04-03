
#include <qaction.h>
#include <qmenu.h>
#include <qmenubar.h>
#include <qfile.h>
#include <qtextcodec.h>
#include <qapplication.h>
#include "../include/qtOpencv.h"
#include "../include/qtResourceDef.h"
#include "../include/trainWidget.h"
#include "../include/spliteWidget.h"
#include "../include/identityWidget .h"
#include "../include/global.h"


CQtOpencv::CQtOpencv(QWidget *parent)
	: QMainWindow(parent)
{
	m_pSpliteWidget = NULL;
	m_pTrainWidget = NULL;
	m_pIdentityWidget = NULL;
	QTextCodec* pTextCodec = QTextCodec::codecForLocale();
	m_pActSpliteForMark = new QAction(pTextCodec->toUnicode(MAIN_WINDOW_ACT_SPLITE_FOR_MARK));
	m_pActTrain = new QAction(pTextCodec->toUnicode(MAIN_WINDOW_ACT_TRAIN));
	m_pActIdentity = new QAction(pTextCodec->toUnicode(MAIN_WINDOW_ACT_IDENTITY));
	QMenuBar* pMenuBar = this->menuBar();
	QMenu* pMenuFunc = new QMenu(pTextCodec->toUnicode( MAIN_WINDOW_MENU_FUNCTION));
	pMenuFunc->addAction(m_pActSpliteForMark);
	pMenuFunc -> addAction(m_pActTrain);
	pMenuFunc->addAction(m_pActIdentity);
	pMenuBar->addMenu(pMenuFunc);
	RetranslateInit();
}

void CQtOpencv::RetranslateInit()
{
	QTextCodec* pTextCodec = QTextCodec::codecForLocale();
	QFile qssFile(pTextCodec->toUnicode(":/widget/mainWindow.qss"));
	if (qssFile.open(QFile::ReadOnly))
	{
		qApp->setStyleSheet(qssFile.readAll());
		qssFile.close();
	}
	connect(m_pActSpliteForMark, SIGNAL(triggered(bool)), this, SLOT(OnEnterSplite(bool)));
	connect(m_pActTrain, SIGNAL(triggered(bool)), this, SLOT(OnEnterTrain(bool)));
	connect(m_pActIdentity, SIGNAL(triggered(bool)), this, SLOT(OnEnterIdentity(bool)));
}

void CQtOpencv::closeEvent(QCloseEvent *closeEvent)
{
	if (this->centralWidget() != NULL)
		this->takeCentralWidget();

	if (m_pSpliteWidget != NULL)
		delete m_pSpliteWidget;


	if (m_pTrainWidget != NULL)
		delete m_pTrainWidget;

	if (m_pIdentityWidget != NULL)
		delete m_pIdentityWidget;

	UsrExit();
}

void CQtOpencv::OnEnterSplite(bool check)
{

	if (m_pSpliteWidget == NULL)
	{
		m_pSpliteWidget = new CSpliteWidget();
	}
	if (this->centralWidget() != NULL)
		this->takeCentralWidget();
	this->setCentralWidget(m_pSpliteWidget);
}

void CQtOpencv::OnEnterTrain(bool check)
{
	if (m_pTrainWidget == NULL)
	{
		m_pTrainWidget = new CTrainWidget();
	}


	if (this->centralWidget() != NULL)
		this->takeCentralWidget();

	this->setCentralWidget(m_pTrainWidget);
}

void CQtOpencv::OnEnterIdentity(bool check)
{

	if (m_pIdentityWidget == NULL)
	{
		m_pIdentityWidget = new CIdentityWidget();
	}


	if (this->centralWidget() != NULL)
		this->takeCentralWidget();
	this->setCentralWidget(m_pIdentityWidget);
}