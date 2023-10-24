#pragma once

#include "Room.h"

class CChannel
{
public:
	CChannel(CChannelServer* server, int id, std::string channelName, int maxPlayers, std::string loginMsg);

	bool UserJoin(CUser* user, bool unhide = false);
	void UserLeft(CUser* user, bool hide = false);
	void SendFullUpdateRoomList();
	void SendFullUpdateRoomList(CUser* user);
	void SendUpdateRoomList(CRoom* room);
	void SendAddRoomToRoomList(CRoom* room);
	void SendRemoveFromRoomList(int roomId);
	void OnEmptyRoomCallback(CRoom* room);
	void SendLobbyMessageToAllUser(std::string senderName, std::string msg);
	void UpdateUserInfo(CUser* user, const CUserCharacter& character);
	CRoom* CreateRoom(CUser* host, CRoomSettings* settings);
	CRoom* GetRoomById(int id);
	CUser* GetUserById(int userID);
	bool RemoveRoomById(int roomID);
	bool RemoveUserById(int userID);

	int GetID();
	std::string GetName();
	std::vector<CRoom*> GetRooms();
	std::vector<CUser*> GetUsers();

	CChannelServer* GetParentChannelServer();

private:
	CChannelServer* m_pParentChannelServer;

	int m_nID;
	int m_nNextRoomID;
	int m_nMaxPlayers;
	std::string m_szName;
	std::string m_LoginMsg;

	std::vector<CRoom*> m_Rooms;
	std::vector<CUser*> m_Users;
};