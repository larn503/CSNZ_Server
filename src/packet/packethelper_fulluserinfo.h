#pragma once

#include "net/receivepacket.h"
#include "user/user.h"

class CPacketHelper_FullUserInfo
{
public:
	CPacketHelper_FullUserInfo();

	void Build(Buffer& buf, int userID, const CUserCharacter& character);
};
