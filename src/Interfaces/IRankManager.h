#pragma once

class IRankManager : public IBaseManager
{
public:
	virtual bool OnRankPacket(CReceivePacket* msg, IExtendedSocket* socket) = 0;
};