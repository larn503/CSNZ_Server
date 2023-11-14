#pragma once

#include <QWidget>

namespace Ui
{
	class MainTab;
}

class CMainTab : public QWidget
{
	Q_OBJECT

public:
	CMainTab(QWidget* parent = nullptr);
	~CMainTab();

	int GetConnectedClients();

public slots:
	void UpdateInfo(int status, int totalConnections, int uptime, double memoryUsage);
	void SendNoticeBtnClicked();
	void OpenIPBanList();
	void OpenHWIDBanList();
	void ServerTimeout();

private:
	Ui::MainTab* m_pUI;
	QTimer* m_pServerHeartbeatTimer;
	int m_nConnectedClients;
};