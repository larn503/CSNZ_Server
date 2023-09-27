#pragma once

class IHostManager : public IBaseManager
{
public:
	virtual bool OnPacket(CReceivePacket* msg, CExtendedSocket* socket) = 0;
	virtual void OnHostChanged(CUser* gameMatchUser, CUser* newHost, CGameMatch* match) = 0;
};