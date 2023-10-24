#pragma once

#include "IGUI.h"
#include "MainWindow.h"

#include <QApplication>

class CGUI : public IGUI
{
public:
	CGUI();
	virtual ~CGUI();
	
	virtual bool Init();
	virtual void Shutdown();
	virtual void Exec();

	// thread safe methods to update GUI
	virtual void LogMessage(int level, const std::string& msg);
	virtual void UpdateInfo(int status, int totalConnections, int uptime, int memoryUsage);

private:
	QApplication* m_pApplication;
	CMainWindow* m_pMainWindow;
};