#include "MainTab.h"
#include "Utils.h"
#include "NoticeDialog.h"
#include "HWIDBanListDialog.h"
#include "IPBanListDialog.h"

#include <ui_maintab.h>
#include <QDateTime.h>
#include <QTimer.h>
#include <QMessageBox.h>

CMainTab::CMainTab(QWidget* parent) : QWidget(parent)
{
	m_pUI = new Ui::MainTab();
	m_pUI->setupUi(this);
	
	m_nConnectedClients = 0;

	m_pServerHeartbeatTimer = new QTimer(this);
	m_pServerHeartbeatTimer->setInterval(120000); // 2 minutes

	connect(m_pUI->SendNoticeBtn, &QPushButton::clicked, this, &CMainTab::SendNoticeBtnClicked);
	connect(m_pUI->IPBanListBtn, &QPushButton::clicked, this, &CMainTab::OpenIPBanList);
	connect(m_pUI->HWIDBanListBtn, &QPushButton::clicked, this, &CMainTab::OpenHWIDBanList);
	connect(m_pServerHeartbeatTimer, &QTimer::timeout, this, &CMainTab::ServerTimeout);

	m_pServerHeartbeatTimer->start();
}

CMainTab::~CMainTab()
{
	delete m_pUI;
}

int CMainTab::GetConnectedClients()
{
	return m_nConnectedClients;
}

// update main tab info
void CMainTab::UpdateInfo(int status, int totalConnections, int uptime, double memoryUsage)
{
	m_nConnectedClients = totalConnections;

	m_pUI->ServerStatusLabel->setText(QString("Server status: %1").arg(status));
	m_pUI->ClientNumberLabel->setText(QString("Total connection: %1").arg(totalConnections));
	m_pUI->UptimeLabel->setText(QString("Uptime: %1").arg(FormatSeconds(uptime)));
	m_pUI->MemUsageLabel->setText(QString("Memory usage: %1mb").arg(memoryUsage, 0, 'f', 3));

	m_pServerHeartbeatTimer->start();
}

void CMainTab::SendNoticeBtnClicked()
{
	CNoticeDialog noticeDialog(this);
	noticeDialog.exec();
}

void CMainTab::OpenIPBanList()
{
	CIPBanListDialog dlg(this);
	dlg.exec();
}

void CMainTab::OpenHWIDBanList()
{
	CHWIDBanListDialog dlg(this);
	dlg.exec();
}

void CMainTab::ServerTimeout()
{
	QMessageBox::warning(this, "Warning", "The server does not respond for more than 2 minutes.");
	m_pServerHeartbeatTimer->stop();
}
