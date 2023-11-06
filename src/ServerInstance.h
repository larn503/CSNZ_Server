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
	void OnCommand(const std::string& command);
	void OnEvent();
	void OnPackets(CExtendedSocket* s, std::vector<CReceivePacket*>& msgs);
	void OnSecondTick();
	void OnMinuteTick();
	void OnFunction(std::vector<std::function<void()>>& funcs);
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
