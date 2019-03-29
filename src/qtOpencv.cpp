
#include <qaction.h>
#include <qmenu.h>
#include <qmenubar.h>
#include <qfile.h>
#include <qtextcodec.h>
#include <qapplication.h>
#include "../include/qtOpencv.h"
#include "../include/qtResourceDef.h"
#include "../include/spliteWidget.h"
#include "../include/global.h"


CQtOpencv::CQtOpencv(QWidget *parent)
	: QMainWindow(parent)
{
	m_pSpliteWidget = NULL;
	m_pCurWidget = NULL;
	QTextCodec* pTextCodec = QTextCodec::codecForLocale();
	m_pActSpliteForMark = new QAction(pTextCodec->toUnicode(MAIN_WINDOW_ACT_SPLITE_FOR_MARK));

	QMenuBar* pMenuBar = this->menuBar();
	QMenu* pMenuFunc = new QMenu(pTextCodec->toUnicode( MAIN_WINDOW_MENU_FUNCTION));
	pMenuFunc->addAction(m_pActSpliteForMark);
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
}

void CQtOpencv::closeEvent(QCloseEvent *closeEvent)
{
	UsrExit();
}

void CQtOpencv::OnEnterSplite(bool check)
{

	if (m_pSpliteWidget == NULL)
	{
		m_pSpliteWidget = new CSpliteWidget();
	}

	if (m_pCurWidget != m_pSpliteWidget)
	{
		if (m_pCurWidget != NULL)
			m_pCurWidget->hide();

		m_pCurWidget = m_pSpliteWidget;
		this->setCentralWidget(m_pCurWidget);
	}
}
