#pragma once

#include "ExtendedSocket.h"
#include "PacketManager.h"
#include "UserManager.h"
#include "ChannelManager.h"
#include "ItemManager.h"
#include "ShopManager.h"
#include "LuckyItemManager.h"
#include "HostManager.h"
#include "DedicatedServerManager.h"
#include "QuestManager.h"
#include "MiniGameManager.h"
#include "ClanManager.h"
#include "RankManager.h"

enum ServerEvent
{
	SERVER_EVENT_CONSOLE_COMMAND = 0,
	SERVER_EVENT_TCP_PACKET = 1,
	SERVER_EVENT_SECOND_TICK = 2,
};

struct Event_s
{
	int type;
	std::string cmd;
	CExtendedSocket* socket;
	std::vector<CReceivePacket*> msgs;
};

class CServerInstance
{
public:
	CServerInstance();
	~CServerInstance();

	bool Init();
	bool LoadConfigs();
	void UnloadConfigs();
	void ListenTCP();
	void ListenUDP();
	void SetServerActive(bool active);
	bool IsServerActive();
	void OnCommand(std::string command);
	void OnEvent();
	void OnPackets(CExtendedSocket* s, std::vector<CReceivePacket*>& msgs);
	void OnSecondTick();
	void OnMinuteTick();
	void UpdateConsoleStatus();
	time_t GetCurrentTime();
	tm* GetCurrentLocalTime();
	double GetMemoryInfo();
	const char* GetMainInfo();
	void DisconnectClient(CExtendedSocket* socket);

private:
	unsigned int m_nNextClientIndex;

	bool m_bIsServerActive;

	// data buffer
	char network_data[15000];

	time_t m_CurrentTime;
	tm* m_pCurrentLocalTime;
	time_t m_nUptime;
};

void ReadConsoleThread();
void ListenThread();
void ListenThreadUDP();
void EventThread();
