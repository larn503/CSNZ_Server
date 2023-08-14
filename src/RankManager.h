#pragma once

class CRankManager
{
public:
	bool OnRankPacket(CReceivePacket* msg, CExtendedSocket* socket);
	
private:
	bool OnRankInfoRequest(CReceivePacket* msg, CUser* user);
	bool OnRankInRoomRequest(CReceivePacket* msg, CUser* user);
	bool OnRankSearchNicknameRequest(CReceivePacket* msg, CUser* user);
	bool OnRankLeagueRequest(CReceivePacket* msg, CUser* user);
	bool OnRankUserInfoRequest(CReceivePacket* msg, CUser* user);
};