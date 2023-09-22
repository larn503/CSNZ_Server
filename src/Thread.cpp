#include "Thread.h"

CThread::CThread(const Handler& function)
{
    m_Object = function;
    m_hHandle = 0;
}

CThread::~CThread()
{
#ifdef WIN32
    CloseHandle(m_hHandle);
#endif
}

// just start thread
bool CThread::Start()
{
    if (m_hHandle)
    {
        // already running
        return true;
    }

#ifdef WIN32
    m_hHandle = CreateThread(0, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(m_Object), this, 0, 0);
    if (m_hHandle == 0)
    {
        return false;
    }
#else
    int result = pthread_create(&m_hHandle, NULL, m_Object, NULL);
    if (result != 0)
    {
        return false;
    }
#endif

    return true;
}

// pauses current thread execution until other thread is finish
void CThread::Join()
{
    if (!m_hHandle)
        return;

#ifdef WIN32
    DWORD dwResult = WaitForSingleObject(m_hHandle, INFINITE);
    if (dwResult == WAIT_FAILED)
        printf("CThread::Join: dwResult == WAIT_FAILED\n");
#else
    if (pthread_join(&m_hHandle, NULL) != 0)
        printf("CThread::Join: pthread_join != 0\n");
#endif
}

// terminates thread (very unsafe if you don't know what you're doing)
void CThread::Terminate()
{
#ifdef WIN32
    TerminateThread(m_hHandle, 0);
    CloseHandle(m_hHandle);
#else
    pthread_kill(m_hHandle, SIGKILL);
#endif

    m_hHandle = 0;
}
