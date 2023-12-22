#include "serverinstance.h"

#include "manager/packetmanager.h"
#include "manager/usermanager.h"
#include "manager/userdatabase.h"
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

#include "net/receivepacket.h"
#include "common/buildnum.h"
#include "csvtable.h"
#include "serverconfig.h"
#include "consolecommands.h"
#ifdef USE_GUI
#include "gui/igui.h"
#endif

using namespace std;

CServerConfig* g_pServerConfig;
CNetwork* g_pNetwork;
CPacketManager* g_pPacketManager;
#ifdef DB_SQLITE
CUserDatabaseSQLite* g_pUserDatabase;
#elif defined DB_MYSQL
CUserDatabaseMySQL* g_pUserDatabase;
#elif defined DB_POSTGRESQL
CUserDatabasePostgreSQL* g_pUserDatabase;
#else
CUserDatabase* g_pUserDatabase;
#endif
CUserManager* g_pUserManager;
CHostManager* g_pHostManager;
CChannelManager* g_pChannelManager;
CItemManager* g_pItemManager;
CShopManager* g_pShopManager;
CLuckyItemManager* g_pLuckyItemManager;
CQuestManager* g_pQuestManager;
CMiniGameManager* g_pMiniGameManager;
CClanManager* g_pClanManager;
CCSVTable* g_pItemTable;
CCSVTable* g_pMapListTable;
CCSVTable* g_pGameModeListTable;
CDedicatedServerManager* g_pDedicatedServerManager;
CRankManager* g_pRankManager;

CServerInstance::CServerInstance()
{
	m_nNextClientIndex = 1;
	m_bIsServerActive = true;
	m_CurrentTime = 0;
	m_pCurrentLocalTime = NULL;
	m_nUptime = 0;
}

bool CServerInstance::Init()
{
	if (g_pServerConfig)
	{
		UnloadConfigs();
		if (!LoadConfigs())
		{
			g_pConsole->Error("Server initialization failed.\n");
			m_bIsServerActive = false;
			return false;
		}

		return true;
	}

	if (!LoadConfigs())
	{
		g_pConsole->Error("Server initialization failed.\n");
		m_bIsServerActive = false;
		return false;
	}

	g_pNetwork = new CNetwork();
	g_pPacketManager = new CPacketManager();
	g_pUserManager = new CUserManager(g_pServerConfig->maxPlayers);
	g_pHostManager = new CHostManager();
	g_pChannelManager = new CChannelManager();
	g_pItemManager = new CItemManager();
	g_pShopManager = new CShopManager();
	g_pLuckyItemManager = new CLuckyItemManager();
	g_pQuestManager = new CQuestManager();
	g_pMiniGameManager = new CMiniGameManager();
	g_pClanManager = new CClanManager();
	g_pRankManager = new CRankManager();

	g_pItemTable = new CCSVTable("Data/Item.csv", rapidcsv::LabelParams(0, 0), rapidcsv::SeparatorParams(), rapidcsv::ConverterParams(true), rapidcsv::LineReaderParams(), true);
	g_pMapListTable = new CCSVTable("Data/MapList.csv", rapidcsv::LabelParams(), rapidcsv::SeparatorParams(), rapidcsv::ConverterParams(true), rapidcsv::LineReaderParams(), true);
	g_pGameModeListTable = new CCSVTable("Data/GameModeList.csv", rapidcsv::LabelParams(), rapidcsv::SeparatorParams(), rapidcsv::ConverterParams(true), rapidcsv::LineReaderParams(), true);
#ifdef DB_SQLITE
	g_pUserDatabase = new CUserDatabaseSQLite();
#elif defined DB_MYSQL
	g_pUserDatabase = new CUserDatabaseMySQL();
#elif defined DB_POSTGRESQL
	g_pUserDatabase = new CUserDatabasePostgreSQL();
#else
	g_pUserDatabase = new CUserDatabase();
#endif

	g_pDedicatedServerManager = new CDedicatedServerManager();

	if (!Manager().InitAll() || !g_pNetwork->ServerInit() || !g_pNetwork->UDPInit())
	{
		g_pConsole->Error("Server initialization failed.\n");
		m_bIsServerActive = false;
		return false;
	}
	else if (g_pItemTable->IsLoadFailed())
	{
		g_pConsole->Error("Server initialization failed. Couldn't load Item.csv.\n");
		m_bIsServerActive = false;
		return false;
	}
	else if (g_pMapListTable->IsLoadFailed())
	{
		g_pConsole->Error("Server initialization failed. Couldn't load MapList.csv.\n");
		m_bIsServerActive = false;
		return false;
	}
	else if (g_pGameModeListTable->IsLoadFailed())
	{
		g_pConsole->Error("Server initialization failed. Couldn't load GameModeList.csv.\n");
		m_bIsServerActive = false;
		return false;
	}

	g_pConsole->Log("Server starts listening. Server developers: Jusic, Hardee, NekoMeow. Thx to Ochii for CSO2 server.\nFor more information visit discord.gg/EvUAY6D\n");
	g_pConsole->Log("Server build: %s, %s\n", build_number(),
#ifdef PUBLIC_RELEASE
		"Public Release");
#else
		"Private Release");
#endif

	// TODO: why?
	OnSecondTick();

	return true;
}

CServerInstance::~CServerInstance()
{
	delete g_pNetwork;
	delete g_pPacketManager;
	delete g_pUserDatabase;
	delete g_pUserManager;
	delete g_pChannelManager;
	delete g_pItemManager;
	delete g_pShopManager;
	delete g_pLuckyItemManager;
	delete g_pQuestManager;
	delete g_pItemTable;
	delete g_pServerConfig;
	delete g_pDedicatedServerManager;
	delete g_pMiniGameManager;
	delete g_pClanManager;
	delete g_pRankManager;
	delete g_pHostManager;
}

bool CServerInstance::LoadConfigs()
{
	g_pServerConfig = new CServerConfig();
	return g_pServerConfig->Load();
}

void CServerInstance::UnloadConfigs()
{
	delete g_pServerConfig;
}

void CServerInstance::OnCommand(const string& command)
{
	g_pConsole->Log("Command: %s\n", command.c_str());

	istringstream iss(command);
	vector<string> args((istream_iterator<string>(iss)), istream_iterator<string>());

	if (args.empty())
		return;

	CCommand* cmd = CmdList()->GetCommand(command);
	if (cmd)
	{
		cmd->Exec(args);
	}
}

void* EventThread(void*)
{
	while (g_pServerInstance->IsServerActive())
	{
		g_Event.WaitForSignal();

		if (g_pServerInstance->IsServerActive())
			g_pServerInstance->OnEvent();
	}
	
	return NULL;
}

void* ReadConsoleThread(void*)
{
	while (g_pServerInstance->IsServerActive())
	{
		string cmd;
		getline(cin, cmd);

		// TODO: ignore empty line?

		g_Event.AddEventConsoleCommand(cmd);
	}
	
	return NULL;
}

void* ListenThread(void*)
{
	while (g_pServerInstance->IsServerActive())
	{
		g_pServerInstance->ListenTCP();
	}
	
	return NULL;
}

void* ListenThreadUDP(void*)
{
	while (g_pServerInstance->IsServerActive())
	{
		g_pServerInstance->ListenUDP();
	}
	
	return NULL;
}

void CServerInstance::ListenTCP()
{
	FD_ZERO(&g_pNetwork->m_Write_fds);
	FD_ZERO(&g_pNetwork->m_Read_fds);

	FD_SET(g_pNetwork->m_TCPSocket, &g_pNetwork->m_Read_fds);

	for (auto socket : g_pNetwork->m_Sessions)
	{
		if (socket->GetPacketsToSend().size())
			FD_SET(socket->GetSocket(), &g_pNetwork->m_Write_fds);

		FD_SET(socket->GetSocket(), &g_pNetwork->m_Read_fds);

		if ((int)socket->GetSocket() > g_pNetwork->m_nFDmax)
			g_pNetwork->m_nFDmax = socket->GetSocket();
	}

	struct timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 0;

	int activity = select(g_pNetwork->m_nFDmax + 1, &g_pNetwork->m_Read_fds, &g_pNetwork->m_Write_fds, NULL, &tv);
	if (activity == SOCKET_ERROR)
	{
		g_pConsole->Error("select() failed with error: %d\n", GetNetworkError());
		g_pChannelManager->EndAllGames();
		SetServerActive(false);
		return;
	}
	else if (!activity) // timeout
	{
		return;
	}

	g_ServerCriticalSection.Enter();

	for (int i = 0; i <= g_pNetwork->m_nFDmax; i++)
	{
		if (FD_ISSET(i, &g_pNetwork->m_Read_fds))
		{
			if (i == g_pNetwork->m_TCPSocket) // accept new client
			{
				IExtendedSocket* sock = g_pNetwork->AcceptNewClient(m_nNextClientIndex);
				if (!sock)
				{
					SleepMS(100);
					break;
				}

				g_pConsole->Log(OBFUSCATE("Client (%d, %s) has been connected to the server\n"), m_nNextClientIndex, sock->GetIP().c_str());

				if (g_pUserDatabase->IsIPBanned(sock->GetIP()))
				{
					g_pConsole->Log(OBFUSCATE("Client (%d, %s) disconnected from the server due to banned ip\n"), m_nNextClientIndex, sock->GetIP().c_str());
					DisconnectClient(sock);
					break;
				}

				m_nNextClientIndex++;
			}
			else // data from client
			{
				IExtendedSocket* s = g_pNetwork->GetExSocketBySocket(i);
				if (!s)
				{
					continue;
				}

				CReceivePacket* msg = s->Read();
				int readResult = s->GetReadResult();
				if (readResult == 0 || readResult == -1)
				{
					int bytesSent = s->GetBytesSent();
					int bytesReceived = s->GetBytesReceived();
					int socket = s->GetSocket();

					// clean up user
					IUser* user = g_pUserManager->GetUserBySocket(s);
					int userID = 0;
					string userName = "NULL";
					if (user)
					{
						userID = user->GetID();
						userName = user->GetUsername();
						g_pUserManager->DisconnectUser(user);
					}
					else
					{
						g_pDedicatedServerManager->RemoveServer(s);
						g_pNetwork->RemoveSocket(s);
					}

					g_pConsole->Log(OBFUSCATE("User logged out (%d, '%s', sent: %d, received: %d, %d, %d, 0x%X)\n"), userID, userName.c_str(), bytesSent, bytesReceived, socket, GetNetworkError(), user);
				}
				else if (readResult == SOCKET_ERROR)
				{
					int error = GetNetworkError();
					if (error == WSAECONNABORTED || error == WSAECONNRESET)
					{
						int bytesSent = s->GetBytesSent();
						int bytesReceived = s->GetBytesReceived();
						int socket = s->GetSocket();

						// clean up user
						IUser* user = g_pUserManager->GetUserBySocket(s);
						int userID = 0;
						string userName = "NULL";
						if (user)
						{
							userID = user->GetID();
							userName = user->GetUsername();
							g_pUserManager->DisconnectUser(user);
						}
						else
						{
							g_pDedicatedServerManager->RemoveServer(s);
							g_pNetwork->RemoveSocket(s);
						}

						g_pConsole->Log(OBFUSCATE("User logged out (%d, '%s', sent: %d, received: %d, %d, %d, 0x%X)\n"), userID, userName.c_str(), bytesSent, bytesReceived, socket, GetNetworkError(), user);
					}
					else
					{
						g_pConsole->Log(OBFUSCATE("Unhandled WSA error: %d, user object will remain...\n"), error);
						g_pDedicatedServerManager->RemoveServer(s);
						g_pNetwork->RemoveSocket(s);
					}
				}
				else if (!msg)
				{
					continue;
				}
				else
				{
					g_Event.AddEventPacket(s, s->GetMsg());
					s->SetMsg(NULL);
				}
			}
		}

		if (FD_ISSET(i, &g_pNetwork->m_Write_fds))
		{
			IExtendedSocket* sock = g_pNetwork->GetExSocketBySocket(i);
			if (sock && sock->GetPacketsToSend().size())
			{
				CSendPacket* msg = sock->GetPacketsToSend()[0]; // send only one packet from vector
				if (sock->Send(msg, true) <= 0)
				{
					g_pConsole->Warn("An error occurred while sending packet from queue: WSAGetLastError: %d, queue.size: %d\n", GetNetworkError(), sock->GetPacketsToSend().size());
					
					DisconnectClient(sock);
				}
				else
				{
					sock->GetPacketsToSend().erase(sock->GetPacketsToSend().begin());
				}
			}
		}
	}

	g_ServerCriticalSection.Leave();
}

void CServerInstance::ListenUDP()
{
	g_pNetwork->m_Read_fds_u = g_pNetwork->m_Master_u; // reset

	struct timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 0;

	int activity = select(g_pNetwork->m_nFDmax_u + 1, &g_pNetwork->m_Read_fds_u, NULL, NULL, &tv);
	if (activity == SOCKET_ERROR)
	{
		g_pConsole->Error("select(udp) failed with error: %d.\n", GetNetworkError());
		return;
	}
	else if (!activity) // timeout
	{
		return;
	}

	g_ServerCriticalSection.Enter();

	for (int i = 0; i <= g_pNetwork->m_nFDmax_u; i++)
	{
		if (FD_ISSET(i, &g_pNetwork->m_Read_fds_u))
		{
			// got message
			struct sockaddr_in from;
			socklen_t fromlen = sizeof(from);

			int datalen = recvfrom(g_pNetwork->m_UDPSocket, network_data, 15000, 0, (sockaddr*)&from, &fromlen);
			if (datalen == 14)
			{
				Buffer buf(vector<unsigned char>(network_data, network_data + datalen));

				char signature = buf.readUInt8();
				if (signature != 'W')
				{
					g_pConsole->Log(OBFUSCATE("CPacketIn_UDP::Parse: signature error\n"));
					return;
				}

				int userID = buf.readUInt32_LE();
				int portID = buf.readUInt16_LE();
				long longAddr = buf.readUInt32_BE();
				string localIpAddress = ip_to_string(longAddr);
				short port = buf.readUInt16_LE();

				IUser* user = g_pUserManager->GetUserById(userID);
				if (!user)
				{
					return;
				}

				int result = user->UpdateHolepunch(portID, port, from.sin_port);
				if (result == -1)
				{
					g_pConsole->Warn("Unknown hole punch port\n");
				}

				Buffer replyBuffer;
				replyBuffer.writeUInt8('W');
				replyBuffer.writeUInt8(0);
				replyBuffer.writeUInt8(1);

				// send reply
				const vector<unsigned char>& buffer = replyBuffer.getBuffer();
				sendto(g_pNetwork->m_UDPSocket, reinterpret_cast<const char*>(&buffer[0]), buffer.size(), 0, (sockaddr*)&from, fromlen);
			}
		}
	}

	g_ServerCriticalSection.Leave();
}

void CServerInstance::SetServerActive(bool active)
{
	m_bIsServerActive = active;

	if (!m_bIsServerActive)
	{
		// wake up event thread
		g_Event.Signal();
	}
}

bool CServerInstance::IsServerActive()
{
	return m_bIsServerActive;
}

void CServerInstance::OnEvent()
{
	bool empty;
	Event_s ev = g_Event.GetNextEvent(empty);
	while (!empty)
	{
		g_ServerCriticalSection.Enter();

		switch (ev.type)
		{
		case SERVER_EVENT_CONSOLE_COMMAND:
			OnCommand(ev.cmd);
			break;
		case SERVER_EVENT_TCP_PACKET:
			OnPackets(ev.socket, ev.msg);
			break;
		case SERVER_EVENT_SECOND_TICK:
			OnSecondTick();
			break;
		case SERVER_EVENT_FUNCTION:
			OnFunction(ev.func);
			break; 
		}

		g_ServerCriticalSection.Leave();

		ev = g_Event.GetNextEvent(empty);
	}
}

void CServerInstance::OnPackets(IExtendedSocket* s, CReceivePacket* msg)
{
	if (find(g_pNetwork->m_Sessions.begin(), g_pNetwork->m_Sessions.end(), s) == g_pNetwork->m_Sessions.end())
	{
		// skip packets with deleted socket object
		return;
	}

	switch (msg->GetID())
	{
	case PacketId::Version:
		g_pUserManager->OnVersionPacket(msg, s);
		break;
	case PacketId::Transfer:
		g_pUserManager->OnCharacterPacket(msg, s);
		break;
	case PacketId::Login:
		g_pUserManager->OnLoginPacket(msg, s);
		break;
	case PacketId::RequestChannels:
		g_pChannelManager->OnChannelListPacket(s);
		break;
	case PacketId::RequestRoomList:
		g_pChannelManager->OnRoomListPacket(msg, s);
		break;
	case PacketId::RecvCrypt:
		g_pUserManager->OnCryptPacket(msg, s);
		break;
	case PacketId::Room:
		g_pChannelManager->OnRoomRequest(msg, s);
		break;
	case PacketId::Shop:
		g_pShopManager->OnShopPacket(msg, s);
		break;
	case PacketId::UMsg:
		g_pUserManager->OnUserMessage(msg, s);
		break;
	case PacketId::Host:
		g_pHostManager->OnPacket(msg, s);
		break;
	case PacketId::Favorite:
		g_pUserManager->OnFavoritePacket(msg, s);
		break;
	case PacketId::Option:
		g_pUserManager->OnOptionPacket(msg, s);
		break;
	case PacketId::Udp:
		g_pUserManager->OnUdpPacket(msg, s);
		break;
	case PacketId::Item:
		g_pItemManager->OnItemPacket(msg, s);
		break;
	case PacketId::MiniGame:
		g_pMiniGameManager->OnPacket(msg, s);
		break;
	case PacketId::MileageBingo:
		break;
	case PacketId::UpdateInfo:
		g_pUserManager->OnUpdateInfoPacket(msg, s);
		break;
	case PacketId::Clan:
		g_pClanManager->OnPacket(msg, s);
		break;
	case PacketId::Statistic:
		g_pPacketManager->SendStatistic(s);
		break;
	case PacketId::Rank:
		g_pRankManager->OnRankPacket(msg, s);
		break;
	case PacketId::Hack:
		//printf("shit");
		break;
	case PacketId::Report:
		g_pUserManager->OnReportPacket(msg, s);
		break;
	case PacketId::Alarm:
		g_pUserManager->OnAlarmPacket(msg, s);
		break;
	case PacketId::Quest:
		g_pQuestManager->OnPacket(msg, s);
		break;
	case PacketId::Title:
		g_pQuestManager->OnTitlePacket(msg, s);
		break;
	case PacketId::HostServer:
		g_pDedicatedServerManager->OnPacket(msg, s);
		break;
	case PacketId::Messenger:
		g_pUserManager->OnMessengerPacket(msg, s);
		break;
	case PacketId::UserSurvey:
		g_pUserManager->OnUserSurveyPacket(msg, s);
		break;
	case PacketId::Addon:
		g_pUserManager->OnAddonPacket(msg, s);
		break;
	case PacketId::Ban:
		g_pUserManager->OnBanPacket(msg, s);
		break;
	case PacketId::League:
		g_pUserManager->OnLeaguePacket(msg, s);
		break;
	default:
		g_pConsole->Warn("Unimplemented packet: %d\n", msg->GetID());
		break;
	}
}

void CServerInstance::OnSecondTick()
{
	// update current time
	time_t prevTime = m_CurrentTime;
	m_CurrentTime = time(NULL);
	m_pCurrentLocalTime = localtime(&m_CurrentTime);
	m_CurrentTime /= 60; // get current time in minutes(last CSO builds use timestamp in minutes)
	m_nUptime++;

	UpdateConsoleStatus();

#ifdef USE_GUI
	GUI()->UpdateInfo(m_bIsServerActive, g_pNetwork->m_Sessions.size(), m_nUptime, GetMemoryInfo());
#endif

	Manager().SecondTick(m_CurrentTime);

	if (m_CurrentTime - prevTime > 0)
	{
		OnMinuteTick();
	}
}

void CServerInstance::OnMinuteTick()
{
	g_pConsole->Log("%s\n", GetMainInfo());

	Manager().MinuteTick(m_CurrentTime);
}

void CServerInstance::OnFunction(function<void()>& func)
{
	func();
}

double CServerInstance::GetMemoryInfo()
{
#ifdef WIN32
	PROCESS_MEMORY_COUNTERS pmc;
	GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));

	SIZE_T mem = pmc.WorkingSetSize;
	if (mem >= 10e9)
		g_pConsole->Warn("[ALERT] Server is using more than 1G of memory.\n");

	return mem / (1024.0 * 1024.0);
#else
	g_pConsole->Log("CServerInstance::GetMemoryInfo: not implemented\n");
	return 0;
#endif
}

const char* CServerInstance::GetMainInfo()
{
	return va("Memory usage: %.02fmb. Connected users: %d. Logged in users: %d.", GetMemoryInfo(), static_cast<int>(g_pNetwork->m_Sessions.size()), static_cast<int>(g_pUserManager->GetUsers().size()));
}

void CServerInstance::DisconnectClient(IExtendedSocket* socket)
{
	if (!socket)
		return;

	IUser* user = g_pUserManager->GetUserBySocket(socket);
	if (user)
	{
		g_pUserManager->DisconnectUser(user);
	}
	else
	{
		g_pDedicatedServerManager->RemoveServer(socket);
		g_pNetwork->RemoveSocket(socket);
	}
}

std::vector<IExtendedSocket*> CServerInstance::GetSessions()
{
	return g_pNetwork->m_Sessions;
}

IExtendedSocket* CServerInstance::GetSocketByID(unsigned int id)
{
	for (auto s : g_pNetwork->m_Sessions)
		if (s->GetID() == id)
			return s;

	return NULL;
}

void CServerInstance::UpdateConsoleStatus()
{
	g_pConsole->SetStatus(GetMainInfo());
	g_pConsole->UpdateStatus();
}

time_t CServerInstance::GetCurrentTime()
{
	return m_CurrentTime; // timestamp in minutes
}

tm* CServerInstance::GetCurrentLocalTime()
{
	return m_pCurrentLocalTime;
}