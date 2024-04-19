#include "net/tcpserver.h"
#include "net/extendedsocket.h"
#include "interface/net/iserverlistener.h"

#include "common/net/netdefs.h"
#include "common/utils.h"
#include "common/console.h"

using namespace std;

/** 
 * Constructor.
 */
CTCPServer::CTCPServer() : m_ListenThread(ListenThread, this)
{
	m_Socket = INVALID_SOCKET;
	m_bIsRunning = false;
	m_pListener = NULL;
	m_pCriticalSection = NULL;
 	m_nNextClientIndex = 0;
	m_nResult = 0;

#ifdef WIN32
	WSADATA wsaData;
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0)
	{
		Console().FatalError("WSAStartup() failed with error: %d\n%s\n", m_nResult, WSAGetLastErrorString());
	}
#endif
}

/** 
 * Destructor. Stop the server on destructor
 */
CTCPServer::~CTCPServer()
{
	Stop();
}

/** 
 * Create socket and start listening.
 * @param port Server's port
 * @return True on success, false on error
 */
bool CTCPServer::Start(const string& port, int tcpSendBufferSize)
{
	if (IsRunning())
		return false;
	
	struct addrinfo* result = NULL;
	struct addrinfo hints;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	m_nResult = getaddrinfo(NULL, port.c_str(), &hints, &result);
	if (m_nResult != 0)
	{
		Console().FatalError("getaddrinfo() failed with error: %d\n%s\n", m_nResult, WSAGetLastErrorString());
		return false;
	}

	// Create a SOCKET for connecting to server
	m_Socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (m_Socket == INVALID_SOCKET)
	{
		Console().FatalError("socket() failed with error: %ld\n%s\n", GetNetworkError(), WSAGetLastErrorString());
		freeaddrinfo(result);
		return false;
	}
	
	// Set the mode of the socket to be nonblocking
	u_long iMode = 1;
	m_nResult = ioctlsocket(m_Socket, FIONBIO, &iMode);
	if (m_nResult == SOCKET_ERROR)
	{
		Console().FatalError("ioctlsocket() failed with error: %d\n%s\n", GetNetworkError(), WSAGetLastErrorString());
		closesocket(m_Socket);
		return false;
	}

	// Setup the TCP listening socket
	m_nResult = ::bind(m_Socket, result->ai_addr, (int)result->ai_addrlen);
	if (m_nResult == SOCKET_ERROR)
	{
		Console().FatalError("bind failed with error: %d\n%s\n", GetNetworkError(), WSAGetLastErrorString());
		freeaddrinfo(result);
		closesocket(m_Socket);
		return false;
	}

	freeaddrinfo(result);

	// start listening for new clients attempting to connect
	m_nResult = listen(m_Socket, SOMAXCONN);
	if (m_nResult == SOCKET_ERROR)
	{
		Console().FatalError("listen() failed with error: %d\n%s\n", GetNetworkError(), WSAGetLastErrorString());
		closesocket(m_Socket);
		return false;
	}

	m_nResult = setsockopt(m_Socket, SOL_SOCKET, SO_SNDBUF, (char*)&tcpSendBufferSize, sizeof(tcpSendBufferSize));
	if (m_nResult == SOCKET_ERROR)
	{
		Console().FatalError("setsockopt failed with error %d\n%s\n", GetNetworkError(), WSAGetLastErrorString());
		closesocket(m_Socket);
		return false;
	}

	WSAPOLLFD fd{};
	fd.fd = m_Socket;
	fd.events = POLLRDNORM;
	m_fds.push_back(fd);

	m_bIsRunning = true;
	
	m_ListenThread.Start();

	return true;
}

/** 
 * Stop the server
 */
void CTCPServer::Stop()
{
	if (IsRunning())
	{
		m_bIsRunning = false;

		m_ListenThread.Join();

		for (auto client : m_Clients)
		{
			delete client;
		}

		m_fds.clear();

		closesocket(m_Socket);
	}
}

/**
 * Listen and wait for incoming data
 */
void CTCPServer::Listen()
{
	printf("[CTCPServer::Listen] %.4fms | m_fds.size(): %d\n", std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - m_lastExecuteTime), m_fds.size());
	m_lastExecuteTime = std::chrono::high_resolution_clock::now();

	int result = WSAPoll(m_fds.data(), m_fds.size(), 1000);
	if (result == SOCKET_ERROR)
	{
		Console().Error("WSAPoll() failed with error: %d\n", GetNetworkError());

		if (m_pListener)
			m_pListener->OnTCPError(0);

		return;
	}
	// nothing happens
	if (!result)
		return;

	if (m_pCriticalSection)
		m_pCriticalSection->Enter();

	for (auto it = m_fds.begin(); it != m_fds.end(); it++) {
		if (it->revents & POLLRDNORM) {
			if (it->fd == m_Socket) {
				IExtendedSocket* socket = Accept(m_nNextClientIndex);
				if (!socket)
					return;

				Console().Log("Client (%d, %s) has been connected to the server\n", m_nNextClientIndex, socket->GetIP().c_str());

				if (m_pListener)
					m_pListener->OnTCPConnectionCreated(socket);

				m_nNextClientIndex++;

				WSAPOLLFD fd{};
				fd.fd = socket->GetSocket();
				fd.events = POLLRDNORM;
				m_fds.push_back(fd);

				// prevent crash
				it = m_fds.begin();
			}
			// read from client
			else {
				IExtendedSocket* socket = GetExSocketBySocket(it->fd);
				if (!socket)
					return;

				CReceivePacket* msg = socket->Read();
				int readResult = socket->GetReadResult();
				if (readResult == 0)
				{
					it->revents |= POLLHUP;
					// connection closed
					DisconnectClient(socket);
				}
				else if (readResult == SOCKET_ERROR)
				{
					it->revents |= POLLERR;
					// error, close connection
					DisconnectClient(socket);

					if (m_pListener)
						m_pListener->OnTCPError(0);
				}
				else if (!msg)
				{
					it->revents |= POLLERR;

					// packet is not valid or wrong sequence or decryption failed
					/// @fixme should we disconnect here? (in official, they kick incorrect seq)

					// exclude case when message is not fully read
					if (!socket->GetMsg())
						DisconnectClient(socket);
				}
				else
				{
					// Call this to mark that socket is ready to receive a new message
					socket->SetMsg(NULL);

					// Important: responsibility for deleting the message is assumed by the listener
					/// @todo use shared_ptr for messages?
					if (m_pListener)
						m_pListener->OnTCPMessage(socket, msg);
					else
						delete msg;
				}
			}
		}

		// since disconnect called, I just log double confirm issues
		if (it->revents & (POLLERR | POLLHUP)) {
			if (it->fd != m_Socket) {
				m_fds.erase(it);
				it = m_fds.begin();
				continue;
			}
		}
	}

	for (auto& socket : m_Clients) {
		if (socket->GetPacketsToSend().size()) {
			CSendPacket* msg = socket->GetPacketsToSend().at(0);
			if (socket->Send(msg, true) <= 0)
			{
				Console().Warn("An error occurred while sending packet from queue: WSAGetLastError: %d, queue.size: %d\n", GetNetworkError(), socket->GetPacketsToSend().size());
				DisconnectClient(socket);
			}
			else
			{
				socket->GetPacketsToSend().erase(socket->GetPacketsToSend().begin());
			}
		}
	}

	if (m_pCriticalSection)
		m_pCriticalSection->Leave();
}

/**
 * Accepts connection on a socket
 * @param id Number 
 * @return Pointer to CExtendedSocket, NULL on error
 */
IExtendedSocket* CTCPServer::Accept(unsigned int id)
{
	sockaddr_in addr;
	socklen_t addrlen = sizeof(addr);

	SOCKET clientSocket = accept(m_Socket, (sockaddr*)&addr, &addrlen);
	if (clientSocket == INVALID_SOCKET)
	{
		Console().FatalError("accept() failed with error: %d\n%s\n", GetNetworkError(), WSAGetLastErrorString());
		return NULL;
	}

	// set SO_KEEPALIVE for socket
	char value = 1;
	setsockopt(clientSocket, SOL_SOCKET, SO_KEEPALIVE, &value, sizeof(value));

	// get IP
	char ip[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &addr.sin_addr, ip, INET_ADDRSTRLEN);

	CExtendedSocket* newSocket = new CExtendedSocket(clientSocket, id);
	newSocket->SetIP(ip);

	m_Clients.push_back(newSocket);

	// send server connected message
	static const string connectedMsg = TCP_CONNECTED_MESSAGE;
	static vector<unsigned char> msg(connectedMsg.begin(), connectedMsg.end());
	newSocket->Send(msg, true);

	return newSocket;
}

/**
 * Gets extended socket by socket descriptor value
 * @param socket
 * @return Pointer to CExtendedSocket, NULL if not exists
 */
IExtendedSocket* CTCPServer::GetExSocketBySocket(SOCKET socket)
{
	for (auto s : m_Clients)
	{
		if (s->GetSocket() == socket)
			return s;
	}

	return NULL;
}

/**
 * Disconnects client by extended socket object
 * @param socket
 */
void CTCPServer::DisconnectClient(IExtendedSocket* socket)
{
	// connection closed
	if (m_pListener)
		m_pListener->OnTCPConnectionClosed(socket);

	Console().Log("Client (%d, %s) has been disconnected from the server\n", socket->GetID(), socket->GetIP().c_str());

	delete socket;
	m_Clients.erase(remove(m_Clients.begin(), m_Clients.end(), socket), m_Clients.end());
}

/**
 * Gets all clients connected to the server
 * @return vector of extended socket
 */
vector<IExtendedSocket*>& CTCPServer::GetClients()
{
	return m_Clients;
}

/**
 * Checks if server is running
 * @return Running status
 */
bool CTCPServer::IsRunning()
{
	return m_bIsRunning;
}

/**
 * Sets listen object
 * @param listener Listener object
 */
void CTCPServer::SetListener(IServerListenerTCP* listener)
{
	m_pListener = listener;
}

/**
 * Sets critical section object. Critical section is used for interacting with the listener object or extended socket
 * @param criticalSection Pointer to critical section object
 */
void CTCPServer::SetCriticalSection(CCriticalSection* criticalSection)
{
	m_pCriticalSection = criticalSection;
}