#pragma once

#include "imanager.h"

class IVoxelManager : public IBaseManager
{
public:
	virtual bool OnPacket(CReceivePacket* msg, IExtendedSocket* socket) = 0;
	virtual std::string GetSlotDetails(const std::string& slotId) = 0;
};