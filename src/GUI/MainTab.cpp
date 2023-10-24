#include "MainTab.h"
#include <ui_maintab.h>

CMainTab::CMainTab(QWidget* parent) : QWidget(parent)
{
	m_pUI = new Ui::MainTab();
	m_pUI->setupUi(this);
}

CMainTab::~CMainTab()
{
	delete m_pUI;
}

// update main tab info
void CMainTab::UpdateInfo(int status, int totalConnections, int uptime, int memoryUsage)
{
	m_pUI->ServerStatusLabel->setText(QString("Server status: %1").arg(status));
	m_pUI->ClientNumberLabel->setText(QString("Total connection: %1").arg(totalConnections));
	m_pUI->UptimeLabel->setText(QString("Uptime %1").arg(uptime));
	m_pUI->MemUsageLabel->setText(QString("Mem usage %1").arg(memoryUsage));
}