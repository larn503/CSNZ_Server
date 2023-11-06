#pragma once

#include <string>
#include "IEvent.h"
#include "IManager.h"

class IGUI
{
public:
	virtual bool Init(IManager* mgr, IEvent* event) = 0;
	virtual bool PostInit() = 0;
	virtual void Shutdown() = 0;
	virtual void Exec() = 0;

	// thread safe methods to update GUI
	virtual void Exit() = 0;
	virtual void LogMessage(int level, const std::string& msg) = 0;
	virtual void UpdateInfo(int status, int totalConnections, int uptime, double memoryUsage) = 0;
	virtual void ShowMessageBox(const std::string& title, const std::string& msg, bool fatalError = false) = 0;
	virtual void ShowMainWindow() = 0;
};

inline IGUI* GUI()
{
	extern IGUI* g_pGUI;
	return g_pGUI;
}