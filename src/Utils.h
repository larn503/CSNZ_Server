#pragma once

void Signal_Break(int n_signal);
void Signal_Int(int n_signal);
void ForceEndServer();
#ifdef _WIN32
LONG __stdcall ExceptionFilter(EXCEPTION_POINTERS* pep);
#endif
uint32_t ip_string_to_int(const std::string& in, bool* const success = nullptr);
std::string ip_to_string(uint32_t in, bool* const success = nullptr);
bool isNumber(const std::string& str);
bool yesOrNo(float probabilityOfYes);
const char* FormatSeconds(int seconds);
char* va(const char* format, ...);
#ifdef WIN32
const char* WSAGetLastErrorString();
#endif
std::vector<std::string> deserialize_array_str(std::string const& csv);
std::vector<int> deserialize_array_int(std::string const& csv);
std::vector<unsigned char> deserialize_array_uchar(std::string const& csv);
std::string serialize_array_str(const std::vector<std::string>& arr);
std::string serialize_array_int(const std::vector<int>& arr);
std::string serialize_array_uchar(const std::vector<unsigned char>& arr);
size_t findCaseInsensitive(std::string data, std::string toSearch, size_t pos = 0);
size_t findCaseInsensitive(std::string data, std::vector<std::string> toSearch, size_t pos = 0);
std::vector<std::string> ParseArguments(const char* cmd);

#ifdef _LINUX
#include <pthread.h>
#endif

extern CConsole* g_pConsole;

class CObjectSync
{
public:
	CObjectSync()
	{
#ifdef _WIN32
		m_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		if (m_hEvent == NULL)
			printf("CObjectSync: Failed to create event\n");
#elif _LINUX
		m_bSignalled = false;
		pthread_mutex_init(&m_Mutex, NULL);
		pthread_cond_init(&m_Cond, NULL);
#else
#error
#endif
	}
	~CObjectSync()
	{
#ifdef _WIN32
		CloseHandle(m_hEvent);
#elif _LINUX
		pthread_mutex_destroy(&m_Mutex);
		pthread_cond_destroy(&m_Cond);
#endif
	}

	void WaitForSignal()
	{
#ifdef _WIN32
		DWORD dwEvent = WaitForSingleObject(m_hEvent, INFINITE);
		switch (dwEvent)
		{
		case WAIT_ABANDONED:
			g_pConsole->Log(OBFUSCATE("CObjectSync: WAIT_ABANDONED: %d\n"), GetLastError());
			break;
		case WAIT_OBJECT_0:
			break;
		case WAIT_TIMEOUT:
			g_pConsole->Log(OBFUSCATE("CObjectSync: WAIT_TIMEOUT: %d\n"), GetLastError());
			break;
		case WAIT_FAILED:
			g_pConsole->Log(OBFUSCATE("CObjectSync: WAIT_FAILED: %d\n"), GetLastError());
			break;
		}
#elif _LINUX
		pthread_mutex_lock(&m_Mutex);
		while (!m_bSignalled)
		{
			pthread_cond_wait(&m_Cond, &m_Mutex);
		}
		m_bSignalled = false;
		pthread_mutex_unlock(&m_Mutex);
#endif
	}

	void Signal()
	{
#ifdef _WIN32
		SetEvent(m_hEvent);
#elif _LINUX
		pthread_mutex_lock(&m_Mutex);
		m_bSignalled = true;
		pthread_mutex_unlock(&m_Mutex);
		pthread_cond_signal(&m_Cond);
#endif
	}

private:
#ifdef _WIN32
	HANDLE m_hEvent;
#elif _LINUX
	bool m_bSignalled;
	pthread_mutex_t m_Mutex;
	pthread_cond_t m_Cond;
#endif
};

class CCriticalSection
{
public:
	CCriticalSection()
	{
#ifdef _WIN32
		InitializeCriticalSection(&m_CriticalSection);
#elif _LINUX
		pthread_mutex_init(&m_Mutex, NULL);
#else
#error
#endif
	}

	~CCriticalSection()
	{
#ifdef _WIN32
		DeleteCriticalSection(&m_CriticalSection);
#elif _LINUX
		pthread_mutex_destroy(&m_Mutex);
#endif
	}

	void Enter()
	{
#ifdef _WIN32
		EnterCriticalSection(&m_CriticalSection);
#elif _LINUX
		pthread_mutex_lock(&m_Mutex);
#endif
	}

	void Leave()
	{
#ifdef _WIN32
		LeaveCriticalSection(&m_CriticalSection);
#elif _LINUX
		pthread_mutex_unlock(&m_Mutex);
#endif
	}

private:
#ifdef _WIN32
	CRITICAL_SECTION m_CriticalSection;
#elif _LINUX
	pthread_mutex_t m_mutex;
#endif
};

class Randomer
{
private:
	std::mt19937 gen;
	std::uniform_int_distribution<size_t> dis;
public:
	inline Randomer(size_t max) : dis(0, max), gen(std::random_device()()) {}
	inline Randomer(size_t max, unsigned int seed) : dis(0, max), gen(seed) {}
	inline size_t operator()() { return dis(gen); }
	// if you want predictable numbers
	inline void SetSeed(unsigned int seed) { gen.seed(seed); }
};