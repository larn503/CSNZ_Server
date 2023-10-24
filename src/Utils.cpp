#include "main.h"
#include "ServerConfig.h"

using namespace std;

void Signal_Break(int n_signal)
{
	printf("Signal_Break called\n");

	// а есть ли смысл?
	if (g_pServerInstance)
		g_pServerInstance->SetServerActive(false);
}

void Signal_Int(int n_signal)
{
	printf("Signal_Int called\n");

	if (g_pServerInstance)
		g_pServerInstance->SetServerActive(false);
}

void ForceEndServer()
{
	// kick user from game
	if (g_pChannelManager)
	{
		for (auto channel : g_pChannelManager->channelServers)
		{
			if (!channel)
				continue;

			for (auto sub : channel->GetChannels())
			{
				if (!sub)
					continue;

				for (auto room : sub->GetRooms())
				{
					if (!room)
						continue;

					if (room->GetStatus() == STATUS_INGAME)
					{
						g_pConsole->Log(OBFUSCATE("Force ending RoomID: %d game match\n"), room->GetID());
						room->EndGame();
					}
				}
			}
		}
	}
}

#ifdef WIN32
typedef BOOL(WINAPI* MINIDUMPWRITEDUMP)
(
	HANDLE hProcess,
	DWORD dwPid,
	HANDLE hFile,
	MINIDUMP_TYPE DumpType,
	CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
	CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
	CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam
);

BOOL WriteMiniDump(EXCEPTION_POINTERS* pep, MINIDUMP_TYPE dumpType, const char* mdmpName)
{
	HMODULE hDbgHelpDll = ::LoadLibrary("DbgHelp.dll");
	if (!hDbgHelpDll)
		return false;

	BOOL bResult = false;
	MINIDUMPWRITEDUMP pfnMiniDumpWrite = (MINIDUMPWRITEDUMP) ::GetProcAddress(hDbgHelpDll, "MiniDumpWriteDump");
	if (pfnMiniDumpWrite)
	{
		char rgchModuleName[MAX_PATH];
		::GetModuleFileName(NULL, rgchModuleName, sizeof(rgchModuleName) / sizeof(char));
		char* pch = strrchr(rgchModuleName, '.');
		if (pch)
		{
			*pch = 0;
		}
		pch = strrchr(rgchModuleName, '\\');
		if (pch)
		{
			// move past the last slash
			pch++;
		}
		else
		{
			strcpy(pch, "unknown");
		}

		char name[MAX_PATH];
		snprintf(name, sizeof(name) / sizeof(char), "%s_%s.mdmp", pch, mdmpName);

		HANDLE hFile = ::CreateFile(name, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL); 
		if (!hFile || hFile == INVALID_HANDLE_VALUE)
			return false;

		MINIDUMP_EXCEPTION_INFORMATION exInfo; 
		exInfo.ThreadId           = GetCurrentThreadId(); 
		exInfo.ExceptionPointers  = pep; 
		exInfo.ClientPointers     = FALSE; 

		bResult = (*pfnMiniDumpWrite)(::GetCurrentProcess(), GetCurrentProcessId(), hFile, dumpType, (pep != NULL) ? &exInfo : NULL, NULL, NULL); 
		CloseHandle(hFile);
	}

	::FreeLibrary(hDbgHelpDll);

	return bResult;
}

typedef HINSTANCE(WINAPI* SHELLEXECUTE)
(
	HWND hwnd,
	LPCSTR lpOperation,
	LPCSTR lpFile,
	LPCSTR lpParameters,
	LPCSTR lpDirectory,
	INT nShowCmd
);
LONG __stdcall ExceptionFilter(EXCEPTION_POINTERS* pep)
{
	printf("ExceptionFilter called\n");

	if (g_pConsole)
		g_pConsole->Log("Last packet function: %s()\n", g_pConsole->GetLastPacket());

	time_t currTime = time(NULL);
	struct tm* pTime = localtime(&currTime);

	char name[MAX_PATH];
	snprintf(name, sizeof(name) / sizeof(char),
		"%d-%.2d-%.2d %.2d-%.2d-%.2d",
		pTime->tm_year + 1900,	/* Year less 2000 */
		pTime->tm_mon + 1,		/* month (0 - 11 : 0 = January) */
		pTime->tm_mday,			/* day of month (1 - 31) */
		pTime->tm_hour,			/* hour (0 - 23) */
		pTime->tm_min,		    /* minutes (0 - 59) */
		pTime->tm_sec		    /* seconds (0 - 59) */
	);

#ifndef PUBLIC_RELEASE
	bool isMdmpGenerated = true;
	if (!WriteMiniDump(pep, MiniDumpWithFullMemory, name))
	{
		if (g_pConsole)
			g_pConsole->Log(OBFUSCATE("WriteMiniDump(MiniDumpWithFullMemory) failed, GetLastError: %d\n"), GetLastError());

		if (!WriteMiniDump(pep, MiniDumpNormal, name))
		{
			if (g_pConsole)
				g_pConsole->Log(OBFUSCATE("WriteMiniDump(MiniDumpNormal) failed, GetLastError: %d\n"), GetLastError());

			isMdmpGenerated = false;
		}
	}
#endif

	if (g_pConsole)
	{
		g_pConsole->Log("%s\n", g_pServerInstance->GetMemoryInfo());
		g_pConsole->Log(OBFUSCATE("Trying to shutdown the server\n"));
	}

	ForceEndServer();

	// make zip (crashing)
	//auto hZip = CreateZip(va("Server_%s.zip", name), 0, ZIP_FILENAME);
	//if (hZip != 0)
	//{
	//	ZipAdd(hZip, "Server.log", "Server.log", 0, ZIP_FILENAME);
	//	ZipAdd(hZip, "Users.log", "Users.log", 0, ZIP_FILENAME);
	//	ZipAdd(hZip, "Internal.log", "Internal.log", 0, ZIP_FILENAME);
	//	if (!name[0] == '\0' && isMdmpGenerated)
	//	{
	//		ZipAdd(hZip, name, name, 0, ZIP_FILENAME);
	//	}

	//	CloseZip(hZip);
	//}
	//else
	//{
	//	MoveFile(OBFUSCATE("Server.log"), va(OBFUSCATE("Server_CRASH_%s.log"), name));
	//	MoveFile(OBFUSCATE("Users.log"), va(OBFUSCATE("Users_CRASH_%s.log"), name));
	//	MoveFile(OBFUSCATE("Internal.log"), va(OBFUSCATE("Internal_CRASH_%s.log"), name));
	//}

	// TODO: проверить, что если сервер будет крашить в цикле
	if (g_pServerConfig && g_pServerConfig->restartOnCrash)
	{
		// open new instance
		TCHAR szPath[_MAX_PATH];
		GetModuleFileName(NULL, szPath, _MAX_PATH);
		//ShellExecute(NULL, "open", szPath, NULL, NULL, 1);

		// fuck defender
		HMODULE hShell32 = ::LoadLibrary("Shell32.dll");
		if (hShell32)
		{
			SHELLEXECUTE pfnShellExecute = (SHELLEXECUTE) ::GetProcAddress(hShell32, "ShellExecuteA");
			pfnShellExecute(NULL, "open", szPath, NULL, NULL, 1);
		}
	}

	return EXCEPTION_EXECUTE_HANDLER;
}
#endif

uint32_t ip_string_to_int(const string& in, bool* const success)
{
	uint32_t ret = 0;
	const bool _success = (1 == inet_pton(AF_INET, in.c_str(), &ret));
	ret = ntohl(ret);

	if (success)
	{
		*success = _success;
	}
	else if (!_success)
	{
		char buf[200] = { 0 };
	}

	return ret;
}

string ip_to_string(uint32_t in, bool* const success)
{
	string ret(INET_ADDRSTRLEN, '\0');
	in = htonl(in);
	const bool _success = (NULL != inet_ntop(AF_INET, &in, &ret[0], ret.size()));

	if (success)
	{
		*success = _success;
	}

	if (_success)
	{
		ret.pop_back(); // remove null-terminator required by inet_ntop
	}
	else if (!success)
	{
		char buf[200] = { 0 };
	}

	return ret;
}

bool isNumber(const string& str)
{
	char* ptr;
	strtol(str.c_str(), &ptr, 10);
	return *ptr == '\0';
}

bool yesOrNo(float probabilityOfYes)
{
	return rand() % 100 < probabilityOfYes;
}

bool yesOrNo(int probabilityOfYes)
{
	return rand() % 100 < probabilityOfYes;
}

const char* FormatSeconds(int seconds)
{
	static char string[64];

	int hours = 0;
	int minutes = seconds / 60;

	if (minutes > 0)
	{
		seconds -= (minutes * 60);
		hours = minutes / 60;

		if (hours > 0)
		{
			minutes -= (hours * 60);
		}
	}

	if (hours > 0)
	{
		snprintf(string, sizeof(string), "%2i:%02i:%02i", hours, minutes, seconds);
	}
	else
	{
		snprintf(string, sizeof(string), "%02i:%02i", minutes, seconds);
	}

	return string;
}

char* va(const char* format, ...)
{
	va_list argptr;
	static char string[8][2048];
	static int curstring = 0;

	curstring = (curstring + 1) % 8;

	va_start(argptr, format);
	vsnprintf(string[curstring], sizeof(string[curstring]), format, argptr);
	va_end(argptr);

	return string[curstring];
}

#ifdef WIN32
const char* WSAGetLastErrorString()
{
	int error = WSAGetLastError();
	static char msgbuf[256];	// for a message up to 255 bytes.
	msgbuf[0] = '\0';	// Microsoft doesn't guarantee this on man page.

	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,	// flags
		NULL,			// lpsource
		error,			// message id
		MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),	// languageid
		msgbuf,			// output buffer
		sizeof(msgbuf),	// size of msgbuf, bytes
		NULL);			// va_list of arguments

	if (!*msgbuf)
		sprintf(msgbuf, "%d", error);  // provide error # if no string available

	return msgbuf;
}
#endif

vector<string> deserialize_array_str(string const& csv)
{
	istringstream parse(csv);
	vector<string> ret;
	for (string token; getline(parse, token, ','); ret.push_back(token));
	return ret;
}

vector<int> deserialize_array_int(string const& csv)
{
	istringstream parse(csv);
	vector<int> ret;
	for (string token; getline(parse, token, ','); ret.push_back(stoi(token)));
	return ret;
}

vector<unsigned char> deserialize_array_uchar(string const& csv)
{
	istringstream parse(csv);
	vector<unsigned char> ret;
	for (string token; getline(parse, token, ','); ret.push_back(stoi(token)));
	return ret;
}

string serialize_array_str(const vector<string>& arr)
{
	ostringstream cat;
	for (size_t index = 0; index < arr.size(); ++index)
		cat << arr[index] << ',';
	string ret = cat.str();
	return ret.substr(0, ret.size() - 1);
}

string serialize_array_int(const vector<int>& arr)
{
	ostringstream cat;
	for (size_t index = 0; index < arr.size(); ++index)
		cat << arr[index] << ',';
	string ret = cat.str();
	return ret.substr(0, ret.size() - 1);
}

string serialize_array_uchar(const vector<unsigned char>& arr)
{
	ostringstream cat;
	for (size_t index = 0; index < arr.size(); ++index)
		cat << arr[index] << ',';
	string ret = cat.str();
	return ret.substr(0, ret.size() - 1);
}

size_t findCaseInsensitive(string data, string toSearch, size_t pos)
{
	// Convert complete given String to lower case
	transform(data.begin(), data.end(), data.begin(), ::tolower);
	// Convert complete given Sub String to lower case
	transform(toSearch.begin(), toSearch.end(), toSearch.begin(), ::tolower);
	// Find sub string in given string
	return data.find(toSearch, pos);
}

size_t findCaseInsensitive(string data, vector<string> toSearch, size_t pos)
{
	for (auto str : toSearch)
	{
		if (findCaseInsensitive(data, str) != string::npos)
			return 1;
	}

	return 0;
}

vector<string> ParseArguments(const char* cmd)
{
	stringstream iss(cmd);
	vector<string> args((istream_iterator<string>(iss)), istream_iterator<string>());

	return args;
}

char* build_number(void);
inline static char* ServerBuild()
{
	return va("Server build: %s", build_number());
}