#include "MainTab.h"
#include "Utils.h"

#include <ui_maintab.h>
#include <QDateTime.h>

CMainTab::CMainTab(QWidget* parent) : QWidget(parent)
{
	m_pUI = new Ui::MainTab();
	m_pUI->setupUi(this);

	connect(m_pUI->SendNoticeBtn, &QPushButton::clicked, this, &CMainTab::SendNoticeBtnClicked);
}

CMainTab::~CMainTab()
{
	delete m_pUI;
}

// update main tab info
void CMainTab::UpdateInfo(int status, int totalConnections, int uptime, double memoryUsage)
{
	m_pUI->ServerStatusLabel->setText(QString("Server status: %1").arg(status));
	m_pUI->ClientNumberLabel->setText(QString("Total connection: %1").arg(totalConnections));
	m_pUI->UptimeLabel->setText(QString("Uptime: %1").arg(FormatSeconds(uptime)));
	m_pUI->MemUsageLabel->setText(QString("Memory usage: %1mb").arg(memoryUsage, 0, 'f', 3));
}

void CMainTab::SendNoticeBtnClicked()
{

}