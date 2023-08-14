#pragma once

#include "ReceivePacket.h"
#include "User.h"

class CPacketHelper_FullUserInfo
{
public:
	CPacketHelper_FullUserInfo();

	void Build(Buffer& buf, int userID, const CUserCharacter& character);
};
