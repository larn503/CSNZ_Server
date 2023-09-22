#pragma once

#ifdef WIN32
#include <Windows.h>
#else
#include <pthread.h>
#endif

#ifdef WIN32
typedef HANDLE ThreadHandle;
#else
typedef pthread_t ThreadHandle;
#endif

typedef void (*Handler)();

// Win32/POSIX thread class
class CThread
{
public:
	CThread(const Handler& function);
	~CThread();

	bool Start();
	void Join();
	void Terminate();

private:
	Handler m_Object;
	ThreadHandle m_hHandle;
};