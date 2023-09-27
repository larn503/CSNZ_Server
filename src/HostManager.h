#pragma once

#include "User.h"
#include "IHostManager.h"

class CHostManager : public CBaseManager<IHostManager>
{
public:
	CHostManager();

	bool OnPacket(CReceivePacket* msg, CExtendedSocket* socket);

	void OnHostChanged(CUser* gameMatchUser, CUser* newHost, CGameMatch* match);
private:
	bool OnSaveData(CReceivePacket* msg, CGameMatch* gamematch);
	bool OnSetUserInventory(CReceivePacket* msg, CExtendedSocket* socket);
	bool OnUseInGameItem(CReceivePacket* msg, CExtendedSocket* socket);
	bool OnFlyerFlockRequest(CReceivePacket* msg, CExtendedSocket* socket);
	bool OnUpdateUserStatus(CReceivePacket* msg, CExtendedSocket* socket);
	bool OnKillEvent(CReceivePacket* msg, CExtendedSocket* socket);
	bool OnUpdateKillCounter(CReceivePacket* msg, CExtendedSocket* socket);
	bool OnUpdateDeathCounter(CReceivePacket* msg, CExtendedSocket* socket);
	bool OnUpdateWinCounter(CReceivePacket* msg, CExtendedSocket* socket);
	bool OnUpdateScore(CReceivePacket* msg, CExtendedSocket* socket);
	bool OnGameEvent(CReceivePacket* msg, CExtendedSocket* socket);
	bool OnUpdateClass(CReceivePacket* msg, CExtendedSocket* socket);
	bool OnZbsResult(CReceivePacket* msg, CExtendedSocket* socket);
	bool OnGameEnd(CExtendedSocket* socket);
	bool OnUserWeapon(CReceivePacket* msg, CExtendedSocket* socket);
	bool OnUserSpawn(CReceivePacket* msg, CExtendedSocket* socket);
	bool OnRoundStart(CReceivePacket* msg, CExtendedSocket* socket);
};