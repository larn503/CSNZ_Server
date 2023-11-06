#include "main.h"
#include "CrashDump.h"

#ifdef USE_GUI
#include "GUI/IGUI.h"
#endif

CConsole* g_pConsole;
CServerInstance* g_pServerInstance;

CEvent g_Event;
CCriticalSection g_ServerCriticalSection;

#ifdef WIN32
void invalid_parameter_function(const wchar_t* expression, const wchar_t* function, const wchar_t* file, unsigned int line, uintptr_t pReserved)
{
	printf("invalid_parameter_function called\n");

	if (g_pConsole)
	{
		g_pConsole->Log(OBFUSCATE("%ls, %ls, %ls, %d, %p\n"), expression, function, file, line, pReserved);
	}
}

BOOL WINAPI CtrlHandler(DWORD ctrlType)
{
	if (ctrlType == CTRL_CLOSE_EVENT)
	{
		g_pServerInstance->SetServerActive(false);

		ExitThread(0); // hack to ignore close event
		return TRUE;
	}

	return FALSE;
}
#endif

#ifdef USE_GUI
CObjectSync g_GUIInitEvent;

void GUIThread()
{
	if (!GUI()->Init(&Manager(), &g_Event))
	{
		__debugbreak();
	}
	
	g_GUIInitEvent.Signal();

	GUI()->Exec();
	GUI()->Shutdown();

	// shutdown the server after closing gui
	g_ServerCriticalSection.Enter();

	g_pServerInstance->SetServerActive(false);

	g_ServerCriticalSection.Leave();
}
#endif

int main(int argc, char* argv[])
{
#ifdef WIN32
	SetUnhandledExceptionFilter(ExceptionFilter);
	_set_invalid_parameter_handler(invalid_parameter_function);
	SetConsoleCtrlHandler(CtrlHandler, TRUE);

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
	GUI()->PostInit();
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
		Event_s ev;
		ev.type = SERVER_EVENT_SECOND_TICK;
		ev.socket = NULL;

		g_Event.AddEvent(ev);

		Sleep(1000);
	}

#ifdef USE_GUI
	GUI()->Exit();
	qtThread.Join();
#endif

	listenThreadTCP.Join();
	listenThreadUDP.Join();
	eventThread.Join();

	g_ServerCriticalSection.Enter();

	// terminate read console thread because of std::getline (should be safe)
	readConsoleThread.Terminate();

	g_ServerCriticalSection.Leave();

	delete g_pServerInstance;
	delete g_pConsole;

	return 0;
}
