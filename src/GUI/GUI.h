#pragma once

#include "IGUI.h"
#include "MainWindow.h"
//#include "../IUserManager.h"

#include <QApplication>

class CGUI : public IGUI
{
public:
	CGUI();
	virtual ~CGUI();
	
	virtual bool Init(IManager* mgr, IEvent* event);
	virtual void Shutdown();
	virtual void Exec();
	virtual bool PostInit();

	// thread safe methods to update GUI
	virtual void LogMessage(int level, const std::string& msg);
	virtual void UpdateInfo(int status, int totalConnections, int uptime, double memoryUsage);
	virtual void ShowMessageBox(const std::string& title, const std::string& msg, bool fatalError = false);
	virtual void ShowMainWindow();

private:
	QApplication* m_pApplication;
	CMainWindow* m_pMainWindow;
};

extern IEvent* g_pEvent;
extern IManager* g_pManager;
//extern IUserManager* g_pUserManager;