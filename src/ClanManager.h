#pragma once

#include "IClanManager.h"

class CClanManager : public CBaseManager<IClanManager>
{
public:
	CClanManager();

	bool OnPacket(CReceivePacket* msg, CExtendedSocket* socket);
	bool OnClanListRequest(CReceivePacket* msg, CUser* user);
	bool OnClanInfoRequest(CReceivePacket* msg, CUser* user);
	bool OnClanCreateRequest(CReceivePacket* msg, CUser* user);
	bool OnClanJoinRequest(CReceivePacket* msg, CUser* user);
	bool OnClanCancelJoinRequest(CReceivePacket* msg, CUser* user);
	bool OnClanJoinApproveRequest(CReceivePacket* msg, CUser* user);
	bool OnClanJoinResultRequest(CReceivePacket* msg, CUser* user);
	bool OnClanLeaveRequest(CReceivePacket* msg, CUser* user);
	bool OnClanInviteRequest(CReceivePacket* msg, CUser* user);
	bool OnClanChangeMemberGradeRequest(CReceivePacket* msg, CUser* user);
	bool OnClanKickMemberRequest(CReceivePacket* msg, CUser* user);
	bool OnClanUpdateMarkRequest(CReceivePacket* msg, CUser* user);
	bool OnClanUpdateConfigRequest(CReceivePacket* msg, CUser* user);
	bool OnClanSetNoticeRequest(CReceivePacket* msg, CUser* user);
	bool OnClanStorageGiveItemRequest(CReceivePacket* msg, CUser* user);
	bool OnClanStorageGetItemRequest(CReceivePacket* msg, CUser* user);
	bool OnClanStorageRequest(CReceivePacket* msg, CUser* user);
	bool OnClanStorageDeleteItem(CReceivePacket* msg, CUser* user);
	bool OnClanDissolveRequest(CReceivePacket* msg, CUser* user);
	bool OnClanDelegateMasterRequest(CReceivePacket* msg, CUser* user);
	bool OnClanMemberUserListRequest(CReceivePacket* msg, CUser* user);
	bool OnClanJoinUserListRequest(CReceivePacket* msg, CUser* user);
	bool OnClanChatMessage(CReceivePacket* msg, CUser* user);

	void OnUserLogin(CUser* user);
};
