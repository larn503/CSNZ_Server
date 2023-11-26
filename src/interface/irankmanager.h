#pragma once

#include "imanager.h"

class IUser;

class IRankManager : public IBaseManager
{
public:
	virtual bool OnRankPacket(CReceivePacket* msg, IExtendedSocket* socket) = 0;
};