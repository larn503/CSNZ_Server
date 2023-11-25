#pragma once

class IClanManager : public IBaseManager
{
public:
	virtual bool OnPacket(CReceivePacket* msg, IExtendedSocket* socket) = 0;
	virtual bool OnClanListRequest(CReceivePacket* msg, IUser* user) = 0;
	virtual bool OnClanInfoRequest(CReceivePacket* msg, IUser* user) = 0;
	virtual bool OnClanCreateRequest(CReceivePacket* msg, IUser* user) = 0;
	virtual bool OnClanJoinRequest(CReceivePacket* msg, IUser* user) = 0;
	virtual bool OnClanCancelJoinRequest(CReceivePacket* msg, IUser* user) = 0;
	virtual bool OnClanJoinApproveRequest(CReceivePacket* msg, IUser* user) = 0;
	virtual bool OnClanJoinResultRequest(CReceivePacket* msg, IUser* user) = 0;
	virtual bool OnClanLeaveRequest(CReceivePacket* msg, IUser* user) = 0;
	virtual bool OnClanInviteRequest(CReceivePacket* msg, IUser* user) = 0;
	virtual bool OnClanChangeMemberGradeRequest(CReceivePacket* msg, IUser* user) = 0;
	virtual bool OnClanKickMemberRequest(CReceivePacket* msg, IUser* user) = 0;
	virtual bool OnClanUpdateMarkRequest(CReceivePacket* msg, IUser* user) = 0;
	virtual bool OnClanUpdateConfigRequest(CReceivePacket* msg, IUser* user) = 0;
	virtual bool OnClanSetNoticeRequest(CReceivePacket* msg, IUser* user) = 0;
	virtual bool OnClanStorageGiveItemRequest(CReceivePacket* msg, IUser* user) = 0;
	virtual bool OnClanStorageGetItemRequest(CReceivePacket* msg, IUser* user) = 0;
	virtual bool OnClanStorageRequest(CReceivePacket* msg, IUser* user) = 0;
	virtual bool OnClanStorageDeleteItem(CReceivePacket* msg, IUser* user) = 0;
	virtual bool OnClanDissolveRequest(CReceivePacket* msg, IUser* user) = 0;
	virtual bool OnClanDelegateMasterRequest(CReceivePacket* msg, IUser* user) = 0;
	virtual bool OnClanMemberUserListRequest(CReceivePacket* msg, IUser* user) = 0;
	virtual bool OnClanJoinUserListRequest(CReceivePacket* msg, IUser* user) = 0;
	virtual bool OnClanChatMessage(CReceivePacket* msg, IUser* user) = 0;

	virtual void OnUserLogin(IUser* user) = 0;
};
