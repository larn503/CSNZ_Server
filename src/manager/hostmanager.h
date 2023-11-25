#pragma once

#include "interface/ihostmanager.h"
#include "user/user.h"

class CHostManager : public CBaseManager<IHostManager>
{
public:
	CHostManager();

	bool OnPacket(CReceivePacket* msg, IExtendedSocket* socket);

	void OnHostChanged(IUser* gameMatchUser, IUser* newHost, CGameMatch* match);
private:
	bool OnSaveData(CReceivePacket* msg, CGameMatch* gamematch);
	bool OnSetUserInventory(CReceivePacket* msg, IExtendedSocket* socket);
	bool OnUseInGameItem(CReceivePacket* msg, IExtendedSocket* socket);
	bool OnFlyerFlockRequest(CReceivePacket* msg, IExtendedSocket* socket);
	bool OnUpdateUserStatus(CReceivePacket* msg, IExtendedSocket* socket);
	bool OnKillEvent(CReceivePacket* msg, IExtendedSocket* socket);
	bool OnUpdateKillCounter(CReceivePacket* msg, IExtendedSocket* socket);
	bool OnUpdateDeathCounter(CReceivePacket* msg, IExtendedSocket* socket);
	bool OnUpdateWinCounter(CReceivePacket* msg, IExtendedSocket* socket);
	bool OnUpdateScore(CReceivePacket* msg, IExtendedSocket* socket);
	bool OnGameEvent(CReceivePacket* msg, IExtendedSocket* socket);
	bool OnUpdateClass(CReceivePacket* msg, IExtendedSocket* socket);
	bool OnZbsResult(CReceivePacket* msg, IExtendedSocket* socket);
	bool OnGameEnd(IExtendedSocket* socket);
	bool OnUserWeapon(CReceivePacket* msg, IExtendedSocket* socket);
	bool OnUserSpawn(CReceivePacket* msg, IExtendedSocket* socket);
	bool OnRoundStart(CReceivePacket* msg, IExtendedSocket* socket);
};