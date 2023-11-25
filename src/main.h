#pragma once

#include "net/network.h"
#include "console.h"

#define NOMINMAX

#ifdef _WIN32
#include <windows.h>
#include <dbghelp.h>
#include <psapi.h>
#else
#include <time.h>
#include <stdio.h>

#define MAX_PATH 260

#endif

#include <iostream>  //std::string, std::cout
#include <vector>    //std::vector
#include <map>		 //std::map
#include <string>    //std::operator+
#include <cstdint>   //int definitions
#include <time.h>    //time, localtime functions
#include <signal.h>  //signals...
#include <iterator>
#include <algorithm>
#include <numeric>
#include <random>
#include <iomanip>
#include <sstream>
#include <functional>

#pragma comment (lib, "dbghelp.lib")

#include "obfuscate.h"
#include "common/utils.h"

#include "serverinstance.h"
#include "csvtable.h"
#include "common/thread.h"
#include "event.h"

extern CConsole* g_pConsole;
extern class CServerInstance* g_pServerInstance;
extern class CServerConfig* g_pServerConfig;
#ifdef DB_SQLITE
extern class CUserDatabaseSQLite* g_pUserDatabase;
#elif defined DB_MYSQL
extern class CUserDatabaseMySQL* g_pUserDatabase;
#elif defined DB_POSTGRESQL
extern class CUserDatabasePostgreSQL* g_pUserDatabase;
#else
extern class CUserDatabase* g_pUserDatabase;
#endif
extern class CPacketManager* g_pPacketManager;
extern class CNetwork* g_pNetwork;
extern class CUserManager* g_pUserManager;
extern class CHostManager* g_pHostManager;
extern class CChannelManager* g_pChannelManager;
extern class CItemManager* g_pItemManager;
extern class CShopManager* g_pShopManager;
extern class CLuckyItemManager* g_pLuckyItemManager;
extern class CQuestManager* g_pQuestManager;
extern class CMiniGameManager* g_pMiniGameManager;
extern class CCSVTable* g_pItemTable;
extern class CCSVTable* g_pMapListTable;
extern class CCSVTable* g_pGameModeListTable;
extern class CDedicatedServerManager* g_pDedicatedServerManager;
extern class CClanManager* g_pClanManager;
extern class CRankManager* g_pRankManager;

extern CEvent g_Event;
extern CCriticalSection g_ServerCriticalSection;