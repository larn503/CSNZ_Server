#include "DedicatedServerManager.h"

CDedicatedServer::CDedicatedServer(CExtendedSocket* socket, int ip, int port)
{
	m_pSocket = socket;
	m_iIP = ip;
	m_iPort = port; // dedi client/server port
	m_pRoom = NULL;
	m_iLastMemory = 0;

	g_pUserManager->SendMetadata(socket);

	// I don't think dedi needs all of these data
	////g_pPacketManager->SendMetadataEncyclopedia(socket);
	//g_pPacketManager->SendMetadataMaplist(socket);
	//g_pPacketManager->SendMetadataMatchOption(socket);
	//g_pPacketManager->SendMetadataClientTable(socket);
	//g_pPacketManager->SendMetadataWeaponParts(socket);
	//g_pPacketManager->SendMetadataItemBox(socket, g_pLuckyItemManager->GetItems());
	//g_pPacketManager->SendMetadataGameModeList(socket);
	//g_pPacketManager->SendMetadataProgressUnlock(socket);
	//g_pPacketManager->SendMetadataUnk8(socket);
	//g_pPacketManager->SendMetadataWeaponPaint(socket);
	////g_pPacketManager->SendMetadataUnk3(socket);
	//g_pPacketManager->SendMetadataReinforceItemsExp(socket);
	//g_pPacketManager->SendMetadataReinforceMaxLvl(socket);
	//g_pPacketManager->SendMetadataReinforceMaxEXP(socket);
	//g_pPacketManager->SendMetadataUnk20(socket);
	//g_pPacketManager->SendMetadataUnk15(socket);
	//g_pPacketManager->SendMetadataItemExpireTime(socket);
	////g_pPacketManager->SendMetadataHonorMoneyShop(socket);
	//g_pPacketManager->SendMetadataScenarioTX_Common(socket);
	//g_pPacketManager->SendMetadataScenarioTX_Dedi(socket);
	////g_pPacketManager->SendMetadataShopItemList_Dedi(socket);
	//g_pPacketManager->SendMetadataZBCompetitive(socket);
}

void CDedicatedServer::SetRoom(CRoom* room)
{
	m_pRoom = room;
}

void CDedicatedServer::SetMemoryUsage(int memShift)
{
	m_iLastMemory = memShift << 20;
}

CExtendedSocket* CDedicatedServer::GetSocket()
{
	return m_pSocket;
}

CRoom* CDedicatedServer::GetRoom()
{
	return m_pRoom;
}

int CDedicatedServer::GetMemoryUsage()
{
	return m_iLastMemory;
}

int CDedicatedServer::GetIP()
{
	return m_iIP;
}

int CDedicatedServer::GetPort()
{
	return m_iPort;
}

bool CDedicatedServerManager::OnPacket(CReceivePacket* msg, CExtendedSocket* socket)
{
	LOG_PACKET;

	int type = msg->ReadUInt8();
	switch (type)
	{
	case 0:
	{
		int port = msg->ReadUInt16();
		int ip = msg->ReadUInt32(false);

		g_pDedicatedServerManager->AddServer(new CDedicatedServer(socket, ip, port));

		break;
	}
	case 1:
	{
		CDedicatedServer* server = g_pDedicatedServerManager->GetServerBySocket(socket);
		if (server)
			server->SetMemoryUsage(msg->ReadUInt16());

		break;
	}
	case 2:
	{
		int unk = msg->ReadUInt32();
		g_pConsole->Warn("CDedicatedServerManager::OnPacket(2): %d\n", unk);

		break;
	}
	default:
		g_pConsole->Warn("Unknown host server request %d\n", type);
		break;
	}

	return true;
}

CDedicatedServerManager::CDedicatedServerManager()
{
}

CDedicatedServerManager::~CDedicatedServerManager()
{
	Shutdown();
}

void CDedicatedServerManager::Shutdown()
{
	std::lock_guard<std::mutex> lg(hMutex);

	for (auto s : vServerPools)
		g_pPacketManager->SendHostServerStop(s->GetSocket());
}

CDedicatedServer* CDedicatedServerManager::GetAvailableServerFromPools(CRoom* room)
{
	std::lock_guard<std::mutex> lg(hMutex);

	for (auto s : vServerPools)
	{
		if (!s->GetRoom())
		{
			s->SetRoom(room);
			return s;
		}
	}

	return NULL;
}

bool CDedicatedServerManager::IsPoolAvailable()
{
	std::lock_guard<std::mutex> lg(hMutex);

	for (auto s : vServerPools)
		if (!s->GetRoom())
			return true;

	return false;
}

void CDedicatedServerManager::AddServer(CDedicatedServer* server)
{
	std::lock_guard<std::mutex> lg(hMutex);
	vServerPools.push_back(server);
}

CDedicatedServer* CDedicatedServerManager::GetServerBySocket(CExtendedSocket* socket)
{
	std::lock_guard<std::mutex> lg(hMutex);

	for (auto s : vServerPools)
		if (s->GetSocket() == socket)
			return s;

	return NULL;
}

void CDedicatedServerManager::RemoveServer(CExtendedSocket* socket)
{
	CDedicatedServer* server = GetServerBySocket(socket);
	if (!server)
		return;

	CRoom* room = server->GetRoom();
	if (room)
		room->SetServer(NULL);

	vServerPools.erase(remove(vServerPools.begin(), vServerPools.end(), server), vServerPools.end());
}
