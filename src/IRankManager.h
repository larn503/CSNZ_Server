#pragma once

class IRankManager : public IBaseManager
{
public:
	virtual bool OnRankPacket(CReceivePacket* msg, CExtendedSocket* socket) = 0;
};