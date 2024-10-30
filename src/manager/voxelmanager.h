#pragma once

#include "manager.h"
#include "interface/ivoxelmanager.h"

class IExtendedSocket;
class CReceivePacket;
class CRoomSettings;

class CVoxelManager : public CBaseManager<IVoxelManager>
{
public:
	CVoxelManager();
	~CVoxelManager();

	bool OnPacket(CReceivePacket* msg, IExtendedSocket* socket);
	std::string GetSlotDetails(const std::string& slotId);
};

extern CVoxelManager g_VoxelManager;