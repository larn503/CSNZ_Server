#pragma once

class IChannelManager
{
public:
	virtual bool OnChannelListPacket(CExtendedSocket* socket) = 0;
	virtual bool OnRoomRequest(CReceivePacket* msg, CExtendedSocket* socket) = 0;
	virtual bool OnRoomListPacket(CReceivePacket* msg, CExtendedSocket* socket) = 0;
	virtual bool OnLobbyMessage(CReceivePacket* msg, CExtendedSocket* socket, CUser* user) = 0;
	virtual bool OnWhisperMessage(CReceivePacket* msg, CUser* user) = 0;
	virtual bool OnRoomUserMessage(CReceivePacket* msg, CUser* user) = 0;
	virtual bool OnRoomTeamUserMessage(CReceivePacket* msg, CUser* user) = 0;

	virtual class CChannelServer* GetServerByIndex(int index) = 0;
	virtual void JoinChannel(CUser* user, int channelServerID, int channelID, bool transfer) = 0;
};