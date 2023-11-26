#pragma once

#include <vector>

#ifdef WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define WSAEWOULDBLOCK		EWOULDBLOCK
#define WSAECONNREFUSED     ECONNREFUSED
#define WSAECONNABORTED		ECONNABORTED
#define WSAECONNRESET		ECONNRESET

#define ioctlsocket ioctl
#define closesocket close

#define SOCKET_ERROR -1
#define INVALID_SOCKET -1
#endif

#include "interface/iextendedsocket.h"

class CNetwork
{
public:
	CNetwork();
	~CNetwork();
	
	void Cleanup();
	
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

extern class CNetwork* g_pNetwork;