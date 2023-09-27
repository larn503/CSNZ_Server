#pragma once

#include "UserManager.h"
#include "IMiniGameManager.h"

class CMiniGameManager : public CBaseManager, public IMiniGameManager
{
public:
	CMiniGameManager();

	void OnPacket(CReceivePacket* msg, CExtendedSocket* socket);

	void WeaponReleaseAddCharacter(CUser* user, char charID, int count);

	void SendWeaponReleaseUpdate(CUser* user);

	void OnBingoUpdateRequest(CUser* user);

private:
	// bingo
	void OnBingoRequest(CReceivePacket* msg, CUser* user);
	void OnBingoResetRequest(CUser* user);
	void OnBingoShuffleRequest(CUser* user);

	bool BingoInitDesk(CUser* user, UserBingo& bingo);
	bool BingoOpenRandomNumber(CUser* user, UserBingo& bingo);

	// weapon release
	void OnWeaponReleaseRequest(CReceivePacket* msg, CUser* user);
	void OnWeaponReleaseSetCharacterRequest(CReceivePacket* msg, CUser* user);
	void OnWeaponReleaseGetJokerRequest(CUser* user);
};
