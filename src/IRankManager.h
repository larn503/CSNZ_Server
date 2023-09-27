#pragma once

class IRankManager
{
public:
	virtual bool OnRankPacket(CReceivePacket* msg, CExtendedSocket* socket) = 0;
};