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

struct Event_s
{
	int type;
	std::string cmd;
	CExtendedSocket* socket;
	std::vector<CReceivePacket*> msgs;
	CReceivePacket* msg;
};

class CServerInstance
{
public:
	CServerInstance();
	~CServerInstance();

	void Init();
	bool LoadConfigs();
	void UnloadConfigs();
	void Listen();
	void AddBanIP(std::string ip, bool unban = false);
	void AddBanHWID(std::vector<unsigned char>& hwid, bool unban = false);
	void OnCommand(std::string command);
	void SetServerActive(bool active);
	bool IsServerActive();
	void OnEvent();
	void OnPackets(CExtendedSocket* s, CReceivePacket* msg, std::vector<CReceivePacket*>& msgs);
	void ReceiveUdpMessage();
	void OnSecondTick();
	void UpdateConsoleStatus();
	time_t GetCurrentTime();
	tm* GetCurrentLocalTime();
	void OnMinuteTick();
	const char* GetMemoryInfo();
	void DisconnectClient(CExtendedSocket* socket);

private:
	unsigned int m_nNextClientIndex;

	bool m_bIsServerActive;

	// data buffer
	char network_data[15000];

	int m_nAutoSaveCounter;

	time_t m_CurrentTime;
	tm* m_pCurrentLocalTime;
};

DWORD WINAPI ReadConsoleThread(LPVOID lpParameter);
DWORD WINAPI ListenThread(LPVOID lpParameter);
DWORD WINAPI ListenThreadUDP(LPVOID lpParameter);
DWORD WINAPI MinuteTick(LPVOID lpParameter);
DWORD WINAPI EventThread(LPVOID lpParameter);
