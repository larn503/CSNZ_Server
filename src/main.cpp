#include "main.h"
#include "ServerInstance.h"
#include "Thread.h"

#ifdef USE_GUI
#include "GUI/IGUI.h"
#endif

CConsole* g_pConsole;
CServerInstance* g_pServerInstance;

CObjectSync g_Event;
CObjectSync g_GUIInitEvent;
std::vector<Event_s> g_Events;

CCriticalSection g_EventCriticalSection;

void invalid_parameter_function(const wchar_t* expression, const wchar_t* function, const wchar_t* file, unsigned int line, uintptr_t pReserved)
{
	printf("invalid_parameter_function called\n");

	if (g_pConsole)
	{
		g_pConsole->Log(OBFUSCATE("%ls, %ls, %ls, %d, %p\n"), expression, function, file, line, pReserved);
	}
}

#ifdef USE_GUI
void GUIThread()
{
	if (!GUI()->Init())
	{
		__debugbreak();
	}

	g_GUIInitEvent.Signal();

	GUI()->Exec();
	GUI()->Shutdown();

	// shutdown the server after closing gui
	g_EventCriticalSection.Enter();

	g_pServerInstance->SetServerActive(false);

	g_EventCriticalSection.Leave();
}
#endif

int main(int argc, char* argv[])
{
	signal(SIGBREAK, &Signal_Break);
	signal(SIGINT, &Signal_Int);

#ifdef WIN32
	SetUnhandledExceptionFilter(ExceptionFilter);
	_set_invalid_parameter_handler(invalid_parameter_function);

#ifdef USE_GUI
	// hide console when using gui
	ShowWindow(GetConsoleWindow(), SW_HIDE);
#endif
#endif

#ifdef USE_GUI
	CThread qtThread(GUIThread);
	qtThread.Start();
#endif

	// wait for gui init before we init the server
	g_GUIInitEvent.WaitForSignal();

	g_pConsole = new CConsole();
	g_pServerInstance = new CServerInstance();
	if (!g_pServerInstance->Init())
	{
#ifdef USE_GUI
		qtThread.Join();
#endif
		delete g_pServerInstance;
		delete g_pConsole;
		return 1;
	}

#ifdef USE_GUI
	GUI()->ShowMainWindow();
#endif

	CThread readConsoleThread(ReadConsoleThread);
	CThread listenThreadTCP(ListenThread);
	CThread listenThreadUDP(ListenThreadUDP);
	CThread eventThread(EventThread);
	readConsoleThread.Start();
	listenThreadTCP.Start();
	listenThreadUDP.Start();
	eventThread.Start();

	while (g_pServerInstance->IsServerActive())
	{
		g_EventCriticalSection.Enter();

		Event_s ev;
		ev.type = SERVER_EVENT_SECOND_TICK;
		ev.socket = NULL;
		g_Events.push_back(ev);

		g_Event.Signal();

		g_EventCriticalSection.Leave();	

		Sleep(1000);
	}

	listenThreadTCP.Join();
	listenThreadUDP.Join();
	eventThread.Join();

#ifdef USE_GUI
	qtThread.Join();
#endif

	g_EventCriticalSection.Enter();

	// terminate read console thread because of std::getline (should be safe)
	readConsoleThread.Terminate();

	g_EventCriticalSection.Leave();

	delete g_pServerInstance;
	delete g_pConsole;

	return 0;
}
