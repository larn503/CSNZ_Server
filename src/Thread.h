#pragma once

#ifdef WIN32
#include <Windows.h>
#else
#include <pthread.h>
#endif

#ifdef WIN32
typedef DWORD ThreadId;
#else
typedef pthread_t ThreadId;
#endif

typedef void (*Handler)();

ThreadId GetCurrentThreadID();

// Win32/POSIX thread class
class CThread
{
public:
	CThread(const Handler& function);
	~CThread();

	bool Start();
	void Join();
	void Terminate();
	bool IsAlive();
	bool IsCurrentThreadSame();

	// threads are non-copyable
	CThread(const CThread&) = delete;
	CThread& operator=(const CThread&) = delete;

private:
	Handler m_Object;
#ifdef WIN32
	HANDLE m_hHandle;
#endif
	ThreadId m_ID;
};