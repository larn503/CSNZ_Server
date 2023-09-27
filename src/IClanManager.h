#pragma once

class IClanManager : public IBaseManager
{
public:
	virtual bool OnPacket(CReceivePacket* msg, CExtendedSocket* socket) = 0;
	virtual bool OnClanListRequest(CReceivePacket* msg, CUser* user) = 0;
	virtual bool OnClanInfoRequest(CReceivePacket* msg, CUser* user) = 0;
	virtual bool OnClanCreateRequest(CReceivePacket* msg, CUser* user) = 0;
	virtual bool OnClanJoinRequest(CReceivePacket* msg, CUser* user) = 0;
	virtual bool OnClanCancelJoinRequest(CReceivePacket* msg, CUser* user) = 0;
	virtual bool OnClanJoinApproveRequest(CReceivePacket* msg, CUser* user) = 0;
	virtual bool OnClanJoinResultRequest(CReceivePacket* msg, CUser* user) = 0;
	virtual bool OnClanLeaveRequest(CReceivePacket* msg, CUser* user) = 0;
	virtual bool OnClanInviteRequest(CReceivePacket* msg, CUser* user) = 0;
	virtual bool OnClanChangeMemberGradeRequest(CReceivePacket* msg, CUser* user) = 0;
	virtual bool OnClanKickMemberRequest(CReceivePacket* msg, CUser* user) = 0;
	virtual bool OnClanUpdateMarkRequest(CReceivePacket* msg, CUser* user) = 0;
	virtual bool OnClanUpdateConfigRequest(CReceivePacket* msg, CUser* user) = 0;
	virtual bool OnClanSetNoticeRequest(CReceivePacket* msg, CUser* user) = 0;
	virtual bool OnClanStorageGiveItemRequest(CReceivePacket* msg, CUser* user) = 0;
	virtual bool OnClanStorageGetItemRequest(CReceivePacket* msg, CUser* user) = 0;
	virtual bool OnClanStorageRequest(CReceivePacket* msg, CUser* user) = 0;
	virtual bool OnClanStorageDeleteItem(CReceivePacket* msg, CUser* user) = 0;
	virtual bool OnClanDissolveRequest(CReceivePacket* msg, CUser* user) = 0;
	virtual bool OnClanDelegateMasterRequest(CReceivePacket* msg, CUser* user) = 0;
	virtual bool OnClanMemberUserListRequest(CReceivePacket* msg, CUser* user) = 0;
	virtual bool OnClanJoinUserListRequest(CReceivePacket* msg, CUser* user) = 0;
	virtual bool OnClanChatMessage(CReceivePacket* msg, CUser* user) = 0;

	virtual void OnUserLogin(CUser* user) = 0;
};
