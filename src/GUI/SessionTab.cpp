#include "SessionTab.h"
#include <ui_sessiontab.h>
#include <QKeyEvent>
#include "GUI.h"

CSessionTab::CSessionTab(QWidget* parent) : QWidget(parent)
{
	m_pUI = new Ui::SessionTab();
	m_pUI->setupUi(this);

	connect(m_pUI->RefreshBtn, SIGNAL(clicked()), this, SLOT(Refresh()));
	connect(m_pUI->KickBtn, SIGNAL(clicked()), this, SLOT(Kick()));
	connect(m_pUI->BanBtn, SIGNAL(clicked()), this, SLOT(Ban()));
	connect(m_pUI->ShowOnlyLoggedIn, SIGNAL(toggled(bool)), this, SLOT(ShowOnlyLoggedInToggled(bool)));

	m_pUI->SessionList->insertRow(0);
	m_pUI->SessionList->setItem(0, 0, new QTableWidgetItem("1"));
	m_pUI->SessionList->setItem(0, 1, new QTableWidgetItem("-1"));
	m_pUI->SessionList->setItem(0, 2, new QTableWidgetItem("127.0.0.1"));
	//m_pUI->SessionList->setItem(0, 3, new QTableWidgetItem(""));
	m_pUI->SessionList->setItem(0, 4, new QTableWidgetItem("00:00:00"));
	m_pUI->SessionList->setItem(0, 5, new QTableWidgetItem("Not logged in"));

	Refresh();
}

CSessionTab::~CSessionTab()
{
	delete m_pUI;
}

void CSessionTab::Refresh()
{
	g_pEvent->AddEventFunction([]()
	{
		printf("Hi!\n");
	});
}

void CSessionTab::Kick()
{
}

void CSessionTab::Ban()
{

}

void CSessionTab::ShowOnlyLoggedInToggled(bool checked)
{
	for (int i = 0; i < m_pUI->SessionList->rowCount(); i++)
	{
		if (m_pUI->SessionList->item(i, 1)->text() == "-1")
		{
			checked ? m_pUI->SessionList->hideRow(i) : m_pUI->SessionList->showRow(i);
		}
	}
}

void CSessionTab::keyPressEvent(QKeyEvent* event)
{
	if (event->key() == Qt::Key_F5)
	{
		// refresh session list if F5 pressed
		Refresh();
	}
}