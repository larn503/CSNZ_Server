#pragma once

#ifdef WIN32
#include <windows.h> 
#include "shlwapi.h"
#endif
#include <iostream>  //std::string, std::cout
#include <vector>    //std::vector
#include <string>    //std::operator+
#include <cstdint>
#include <time.h>

enum OutMode
{
	CON_WARNING = 0,
	CON_ERROR,
	CON_LOG
};

enum TextColor
{
	CON_YELLOW = 14,
	CON_RED = 12,
	CON_WHITE = 7,
	CON_GREEN = 10,
	CON_CYAN = 11
};

enum LogLevel
{
	LOG_INFO = (1 << 0),
	LOG_WARN = (1 << 1),
	LOG_ERROR = (1 << 2),
	LOG_DEBUG_PACKET = (1 << 3),
};

// Console and log system implementation
class CConsole
{
public:
	CConsole();
	~CConsole();

	void Warn(const char* msg, ...);
	void Error(const char* msg, ...);
	void Log(const char* msg, ...);
	void Debug(const char* msg, ...);
	void SetStatus(const char* status);
	void SetLogLevel(int logLevel);
	void SetLastPacket(const char* name);
	const char* GetLastPacket();
	void UpdateStatus();

private:
	void   Write(OutMode mode, const char* msg);
	void   WriteToConsole(const char* msg);
	void   SetTextColor(TextColor color);
	const char* GetCurrTime();

#ifdef WIN32
	HANDLE m_Output;
	WORD m_Attrib;		// attrib colours for status bar
	char m_szStatusLine[81];	// first line in console is status line
#endif

	char m_szServerLogPath[MAX_PATH];
	const char* m_szLastPacket;

	int m_nLogLevel;
};