#pragma once

#include "socketshared.h"
#include "common/thread.h"

#include <string>
#include <vector>
#include <chrono>

class IExtendedSocket;
class CExtendedSocket;
class IServerListenerTCP;

void* SendThread(void* data);

/**
 * Class that accepts TCP connection
 */
class CTCPServer : public ISocketListenable
{
public:
	CTCPServer();
	~CTCPServer();
	
	bool Start(const std::string& port, int tcpSendBufferSize);
	void Stop();
	void Listen();
	void Send();

	IExtendedSocket* Accept(unsigned int id);
	IExtendedSocket* GetExSocketBySocket(SOCKET socket);
	void DisconnectClient(IExtendedSocket* socket);
	std::vector<IExtendedSocket*>& GetClients();

	bool IsRunning();

	void SetListener(IServerListenerTCP* listener);
	void SetCriticalSection(CCriticalSection* criticalSection);

private:
	SOCKET m_Socket;
	bool m_bIsRunning;
	int m_nResult;
	CThread m_ListenThread;
	CThread m_SendThread;
	unsigned int m_nNextClientIndex;
	std::vector<IExtendedSocket*> m_Clients;
	IServerListenerTCP* m_pListener;
	CCriticalSection* m_pCriticalSection;

	std::vector<WSAPOLLFD> m_fds;
};