#pragma once

#include "IDedicatedServerManager.h"

class IDedicatedServerManager : public IBaseManager
{
public:
	virtual bool OnPacket(CReceivePacket* msg, IExtendedSocket* socket) = 0;

	virtual void AddServer(CDedicatedServer* server) = 0;

	virtual CDedicatedServer* GetAvailableServerFromPools(IRoom* room) = 0;
	virtual bool IsPoolAvailable() = 0;
	virtual CDedicatedServer* GetServerBySocket(IExtendedSocket* socket) = 0;
	virtual void RemoveServer(IExtendedSocket* socket) = 0;
};