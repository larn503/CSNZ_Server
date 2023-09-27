#pragma once

class IUserManager : public IBaseManager
{
public:
	virtual bool OnLoginPacket(CReceivePacket* msg, CExtendedSocket* socket) = 0;
	virtual bool OnUdpPacket(CReceivePacket* msg, CExtendedSocket* socket) = 0;
	virtual bool OnOptionPacket(CReceivePacket* msg, CExtendedSocket* socket) = 0;
	virtual bool OnVersionPacket(CReceivePacket* msg, CExtendedSocket* socket) = 0;
	virtual bool OnFavoritePacket(CReceivePacket* msg, CExtendedSocket* socket) = 0;
	virtual bool OnCharacterPacket(CReceivePacket* msg, CExtendedSocket* socket) = 0;
	virtual bool OnUserMessage(CReceivePacket* msg, CExtendedSocket* socket) = 0;
	virtual bool OnUpdateInfoPacket(CReceivePacket* msg, CExtendedSocket* socket) = 0;
	virtual bool OnReportPacket(CReceivePacket* msg, CExtendedSocket* socket) = 0;
	virtual bool OnAlarmPacket(CReceivePacket* msg, CExtendedSocket* socket) = 0;
	virtual bool OnUserSurveyPacket(CReceivePacket* msg, CExtendedSocket* socket) = 0;
	virtual bool OnBanPacket(CReceivePacket* msg, CExtendedSocket* socket) = 0;
	virtual bool OnMessengerPacket(CReceivePacket* msg, CExtendedSocket* socket) = 0;
	virtual bool OnAddonPacket(CReceivePacket* msg, CExtendedSocket* socket) = 0;
	virtual bool OnLeaguePacket(CReceivePacket* msg, CExtendedSocket* socket) = 0;

	virtual void SendNoticeMessageToAll(std::string msg) = 0;
	virtual void SendNoticeMsgBoxToAll(std::string msg) = 0;

	virtual int LoginUser(CExtendedSocket* socket, std::string userName, std::string password) = 0;
	virtual int RegisterUser(CExtendedSocket* socket, std::string userName, std::string password) = 0;
	virtual void DisconnectUser(CUser* user) = 0;
	virtual void DisconnectAllFromServer() = 0;
	virtual CUser* AddUser(CExtendedSocket* socket, int userID, std::string userName) = 0;
	virtual CUser* GetUserById(int userId) = 0;
	virtual CUser* GetUserBySocket(CExtendedSocket* socket) = 0;
	virtual CUser* GetUserByUsername(std::string username) = 0;
	virtual CUser* GetUserByNickname(std::string userName) = 0;
	virtual void RemoveUser(CUser* user) = 0;
	virtual void RemoveUserById(int userId) = 0;
	virtual void RemoveUserBySocket(CExtendedSocket* socket) = 0;
	virtual void CleanUpUser(CUser* user) = 0;

	virtual std::vector<CUser*> GetUsers() = 0;

	virtual int ChangeUserNickname(CUser* user, std::string newNickname, bool createCharacter = false) = 0;

	virtual std::vector<CUserInventoryItem>& GetDefaultInventoryItems() = 0;

	virtual void SendMetadata(CExtendedSocket* socket) = 0;
};
