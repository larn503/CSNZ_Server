#include "GUI.h"

static CGUI g_GUI;

IGUI* g_pGUI = NULL;

CGUI::CGUI()
{
	g_pGUI = this;
	m_pApplication = NULL;
	m_pMainWindow = NULL;
}

CGUI::~CGUI()
{
	g_pGUI = NULL;
	Shutdown();
}

bool CGUI::Init()
{
	int argc = 0;
	m_pApplication = new QApplication(argc, NULL);

	m_pMainWindow = new CMainWindow();
	m_pMainWindow->show();

	return true;
}

void CGUI::Shutdown()
{
	delete m_pMainWindow;
	m_pMainWindow = NULL;
	delete m_pApplication;
	m_pApplication = NULL;
}

void CGUI::Exec()
{
	m_pApplication->exec();
}

void CGUI::LogMessage(int level, const std::string& msg)
{
	if (m_pMainWindow)
		QMetaObject::invokeMethod(m_pMainWindow->GetConsoleTab(), "Log", Q_ARG(int, level), Q_ARG(const std::string&, msg));
}

void CGUI::UpdateInfo(int status, int totalConnections, int uptime, int memoryUsage)
{
	if (m_pMainWindow)
		QMetaObject::invokeMethod(m_pMainWindow->GetMainTab(), "UpdateInfo", Q_ARG(int, status), Q_ARG(int, totalConnections), Q_ARG(int, uptime), Q_ARG(int, memoryUsage));
}

void CGUI::ShowMessageBox(const std::string& title, const std::string& msg, bool fatalError)
{
	if (m_pMainWindow)
		QMetaObject::invokeMethod(m_pMainWindow, "ShowMessageBox", Q_ARG(const std::string&, title), Q_ARG(const std::string&, msg), Q_ARG(bool, fatalError));
}