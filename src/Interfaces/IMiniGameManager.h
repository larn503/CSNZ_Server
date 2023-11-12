#pragma once

class IMiniGameManager : public IBaseManager
{
public:
	virtual void OnPacket(CReceivePacket* msg, IExtendedSocket* socket) = 0;
	virtual void WeaponReleaseAddCharacter(IUser* user, char charID, int count) = 0;
	virtual void SendWeaponReleaseUpdate(IUser* user) = 0;
	virtual void OnBingoUpdateRequest(IUser* user) = 0;
};
