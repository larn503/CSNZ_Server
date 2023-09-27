#pragma once

class IMiniGameManager : public IBaseManager
{
public:
	virtual void OnPacket(CReceivePacket* msg, CExtendedSocket* socket) = 0;
	virtual void WeaponReleaseAddCharacter(CUser* user, char charID, int count) = 0;
	virtual void SendWeaponReleaseUpdate(CUser* user) = 0;
	virtual void OnBingoUpdateRequest(CUser* user) = 0;
};
