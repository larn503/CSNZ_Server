#include "main.h"
#include "ServerInstance.h"

CConsole* g_pConsole;
CServerInstance* g_pServerInstance;

CObjectSync g_Event;
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

int main(int argc, char* argv[])
{
	signal(SIGBREAK, &Signal_Break);
	signal(SIGINT, &Signal_Int);

#ifdef WIN32
	SetUnhandledExceptionFilter(ExceptionFilter);
	_set_invalid_parameter_handler(invalid_parameter_function);
#endif

	g_pConsole = new CConsole();

	g_pServerInstance = new CServerInstance();
	g_pServerInstance->Init();

#ifdef WIN32
	CreateThread(NULL, 0, ReadConsoleThread, NULL, 0, 0);
	CreateThread(NULL, 0, ListenThread, NULL, 0, 0);
	CreateThread(NULL, 0, ListenThreadUDP, NULL, 0, 0);
	CreateThread(NULL, 0, MinuteTick, NULL, 0, 0);
	CreateThread(NULL, 0, EventThread, NULL, 0, 0);
#endif

	while (g_pServerInstance->IsServerActive())
	{
		g_EventCriticalSection.Enter();

		Event_s ev;
		ev.type = 2;
		ev.socket = NULL;
		g_Events.push_back(ev);

		g_Event.Signal();

		g_EventCriticalSection.Leave();

		Sleep(1000);
	}

	Sleep(2000);

	delete g_pServerInstance;
	delete g_pConsole;

	return 1;
}
