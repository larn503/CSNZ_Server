#pragma once

#include <string>

class IGUI
{
public:
	virtual bool Init() = 0;
	virtual void Shutdown() = 0;
	virtual void Exec() = 0;

	// thread safe methods to update GUI
	virtual void LogMessage(int level, const std::string& msg) = 0;
	virtual void UpdateInfo(int status, int totalConnections, int uptime, int memoryUsage) = 0;
	virtual void ShowMessageBox(const std::string& title, const std::string& msg, bool fatalError = false) = 0;
};

inline IGUI* GUI()
{
	extern IGUI* g_pGUI;
	return g_pGUI;
}