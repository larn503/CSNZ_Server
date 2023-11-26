#pragma once

#include "interface/iserverinstance.h"
#include "csvtable.h"

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

extern CServerInstance* g_pServerInstance;

extern CCSVTable* g_pItemTable;
extern CCSVTable* g_pMapListTable;
extern CCSVTable* g_pGameModeListTable;

void* ReadConsoleThread(void*);
void* ListenThread(void*);
void* ListenThreadUDP(void*);
void* EventThread(void*);
