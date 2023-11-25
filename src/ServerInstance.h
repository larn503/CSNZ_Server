#pragma once

#include "interface/iserverinstance.h"

#include "net/extendedsocket.h"
#include "manager/packetmanager.h"
#include "manager/usermanager.h"
#include "manager/channelmanager.h"
#include "manager/itemmanager.h"
#include "manager/shopmanager.h"
#include "manager/luckyitemmanager.h"
#include "manager/hostmanager.h"
#include "manager/dedicatedservermanager.h"
#include "manager/questmanager.h"
#include "manager/minigamemanager.h"
#include "manager/clanmanager.h"
#include "manager/rankmanager.h"

class CServerInstance : public IServerInstance
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
	void OnPackets(IExtendedSocket* s, CReceivePacket* msg);
	void OnSecondTick();
	void OnMinuteTick();
	void OnFunction(std::function<void()>& func);
	void UpdateConsoleStatus();
	virtual time_t GetCurrentTime();
	virtual tm* GetCurrentLocalTime();
	virtual double GetMemoryInfo();
	virtual const char* GetMainInfo();
	virtual void DisconnectClient(IExtendedSocket* socket);
	virtual std::vector<IExtendedSocket*> GetSessions();
	virtual IExtendedSocket* GetSocketByID(unsigned int id);

private:
	unsigned int m_nNextClientIndex;

	bool m_bIsServerActive;

	// data buffer
	char network_data[15000];

	time_t m_CurrentTime;
	tm* m_pCurrentLocalTime;
	time_t m_nUptime;
};

void* ReadConsoleThread(void*);
void* ListenThread(void*);
void* ListenThreadUDP(void*);
void* EventThread(void*);
