#pragma once

#include "IDedicatedServerManager.h"

class IDedicatedServerManager : public IBaseManager
{
public:
	virtual bool OnPacket(CReceivePacket* msg, CExtendedSocket* socket) = 0;

	virtual void AddServer(CDedicatedServer* server) = 0;

	virtual CDedicatedServer* GetAvailableServerFromPools(CRoom* room) = 0;
	virtual bool IsPoolAvailable() = 0;
	virtual CDedicatedServer* GetServerBySocket(CExtendedSocket* socket) = 0;
	virtual void RemoveServer(CExtendedSocket* socket) = 0;
};