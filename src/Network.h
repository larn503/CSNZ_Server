#pragma once

#ifdef WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#endif
#include <vector>

#include "ExtendedSocket.h"

class CNetwork
{
public:
	CNetwork();
	~CNetwork();

	bool ServerInit();
	bool UDPInit();

	static int SendMessage(SOCKET curSocket, const char* message, int messageSize);
	static int ReceiveMessage(SOCKET curSocket, char* buffer, int bufSize);
	IExtendedSocket* AcceptNewClient(unsigned int& id);
	IExtendedSocket* GetExSocketBySocket(SOCKET socket);
	void RemoveSocket(IExtendedSocket* socket);

	SOCKET m_TCPSocket;
	SOCKET m_UDPSocket;

	fd_set m_Read_fds;
	fd_set m_Write_fds;
	int m_nFDmax;

	fd_set m_Master_u;
	fd_set m_Read_fds_u;
	int m_nFDmax_u;

	int m_iResult;

	std::vector<IExtendedSocket*> m_Sessions;
};
