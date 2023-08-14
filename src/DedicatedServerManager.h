#pragma once

#include <mutex>

class CDedicatedServer
{
public:
	CDedicatedServer(CExtendedSocket* s, int ip, int port);

	void SetRoom(CRoom* room);
	void SetMemoryUsage(int memShift);

	CExtendedSocket* GetSocket();
	CRoom* GetRoom();
	int GetMemoryUsage();
	int GetIP();
	int GetPort();

private:
	CExtendedSocket* m_pSocket;
	CRoom* m_pRoom;

	int m_iLastMemory;
	int m_iIP;
	int m_iPort;
};

class CDedicatedServerManager
{
public:
	CDedicatedServerManager();
	~CDedicatedServerManager();

	bool OnPacket(CReceivePacket* msg, CExtendedSocket* socket);
	void Shutdown();

	void AddServer(CDedicatedServer* server);

	CDedicatedServer* GetAvailableServerFromPools(CRoom* room);
	bool IsPoolAvailable();
	CDedicatedServer* GetServerBySocket(CExtendedSocket* socket);
	void RemoveServer(CExtendedSocket* socket);

private:
	std::mutex hMutex;
	std::vector<CDedicatedServer*> vServerPools;
};