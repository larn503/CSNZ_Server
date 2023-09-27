#pragma once

class IPacketManager
{
public:
	virtual CSendPacket* CreatePacket(CExtendedSocket* socket, int msgID) = 0;

	virtual void SendUMsgNoticeMsgBoxToUuid(CExtendedSocket* socket, std::string text) = 0;
	virtual void SendUMsgNoticeMessageInChat(CExtendedSocket* socket, std::string text) = 0;
	virtual void SendUMsgChatMessage(CExtendedSocket* socket, int type, std::string gameName, std::string text, bool from = false) = 0;
	virtual void SendUMsgWhisperMessage(CExtendedSocket* socket, std::string msg, std::string destName, CUser* user, int type) = 0; // use SendUMsgChatMessage
	virtual void SendUMsgRoomMessage(CExtendedSocket* socket, std::string senderName, std::string text) = 0; // use SendUMsgChatMessage
	virtual void SendUMsgRoomTeamMessage(CExtendedSocket* socket, std::string senderName, std::string text) = 0; // use SendUMsgChatMessage
	virtual void SendUMsgSystemReply(CExtendedSocket* socket, int type, std::string msg, std::vector<std::string> additionalText = {}) = 0;
	virtual void SendUMsgLobbyMessage(CExtendedSocket* socket, std::string senderName, std::string text) = 0; // use SendUMsgChatMessage
	virtual void SendUMsgNotice(CExtendedSocket* socket, Notice_s& notice, bool unk = 1) = 0;
	virtual void SendUMsgExpiryNotice(CExtendedSocket* socket, std::vector<int>& expiryItems) = 0;
	virtual void SendUMsgRewardNotice(CExtendedSocket* socket, RewardNotice& reward, std::string title = "", std::string description = "", bool inGame = false, bool scen = false) = 0;
	virtual void SendUMsgRewardSelect(CExtendedSocket* socket, Reward* reward) = 0;

	virtual void SendServerList(CExtendedSocket* socket) = 0;

	virtual void SendStatistic(CExtendedSocket* socket) = 0;

	virtual void SendInventoryAdd(CExtendedSocket* socket, std::vector<CUserInventoryItem>& items, int curSlot = 0) = 0;
	virtual void SendInventoryRemove(CExtendedSocket* socket, std::vector<CUserInventoryItem>& items, bool gameSlot = true) = 0;

	virtual void SendDefaultItems(CExtendedSocket* socket, std::vector<CUserInventoryItem>& items) = 0;

	virtual void SendVersion(CExtendedSocket* socket, int result) = 0;

	virtual void SendUserStart(CExtendedSocket* socket, int userID, std::string userName, std::string gameName, bool firstConnect) = 0;
	virtual void SendUserUpdateInfo(CExtendedSocket* socket, CUser* user, const CUserCharacter& character) = 0;
	virtual void SendUserSurvey(CExtendedSocket* socket, Survey& survey) = 0;
	virtual void SendUserSurveyReply(CExtendedSocket* socket, int result) = 0;

	virtual void SendOption(CExtendedSocket* socket, std::vector<unsigned char>& config) = 0;
	virtual void SendOptionUnk(CExtendedSocket* socket) = 0;
	virtual void SendOptionUnk2(CExtendedSocket* socket) = 0;
	virtual void SendOptionUnk3(CExtendedSocket* socket) = 0;

	virtual void SendMetadataMaplist(CExtendedSocket* socket) = 0;
	virtual void SendMetadataClientTable(CExtendedSocket* socket) = 0;
	virtual void SendMetadataWeaponParts(CExtendedSocket* socket) = 0;
	virtual void SendMetadataModelist(CExtendedSocket* socket) = 0;
	virtual void SendMetadataMatchOption(CExtendedSocket* socket) = 0;
	virtual void SendMetadataItemBox(CExtendedSocket* socket, std::vector<ItemBoxItem>& items) = 0;
	virtual void SendMetadataEncyclopedia(CExtendedSocket* socket) = 0;
	virtual void SendMetadataGameModeList(CExtendedSocket* socket) = 0;
	virtual void SendMetadataReinforceMaxLvl(CExtendedSocket* socket) = 0;
	virtual void SendMetadataReinforceMaxEXP(CExtendedSocket* socket) = 0;
	virtual void SendMetadataUnk8(CExtendedSocket* socket) = 0;
	virtual void SendMetadataProgressUnlock(CExtendedSocket* socket) = 0;
	virtual void SendMetadataWeaponPaint(CExtendedSocket* socket) = 0;
	virtual void SendMetadataUnk3(CExtendedSocket* socket) = 0;
	virtual void SendMetadataReinforceItemsExp(CExtendedSocket* socket) = 0;
	virtual void SendMetadataItemExpireTime(CExtendedSocket* socket) = 0;
	virtual void SendMetadataUnk20(CExtendedSocket* socket) = 0;
	virtual void SendMetadataUnk15(CExtendedSocket* socket) = 0;
	virtual void SendMetadataRandomWeaponList(CExtendedSocket* socket) = 0;
	virtual void SendMetadataHash(CExtendedSocket* socket) = 0;
	virtual void SendMetadataUnk31(CExtendedSocket* socket) = 0;
	virtual void SendMetadataHonorMoneyShop(CExtendedSocket* socket) = 0;
	virtual void SendMetadataScenarioTX_Common(CExtendedSocket* socket) = 0;
	virtual void SendMetadataScenarioTX_Dedi(CExtendedSocket* socket) = 0;
	virtual void SendMetadataShopItemList_Dedi(CExtendedSocket* socket) = 0;
	virtual void SendMetadataZBCompetitive(CExtendedSocket* socket) = 0;
	virtual void SendMetadataUnk43(CExtendedSocket* socket) = 0;
	virtual void SendMetadataUnk49(CExtendedSocket* socket) = 0;
	virtual void SendMetadataWeaponProp(CExtendedSocket* socket) = 0;
	virtual void SendMetadataPPSystem(CExtendedSocket* socket) = 0;
	virtual void SendMetadataCodisData(CExtendedSocket* socket) = 0;
	virtual void SendMetadataItem(CExtendedSocket* socket) = 0;

	virtual void SendGameMatchInfo(CExtendedSocket* socket) = 0;
	virtual void SendGameMatchUnk(CExtendedSocket* socket) = 0;
	virtual void SendGameMatchUnk9(CExtendedSocket* socket) = 0;

	virtual void SendReply(CExtendedSocket* socket, int type) = 0;

	virtual void SendItemUnk1(CExtendedSocket* socket) = 0;
	virtual void SendItemUnk3(CExtendedSocket* socket) = 0;
	virtual void SendItemEquipTattoo(CExtendedSocket* socket) = 0;
	virtual void SendItemDailyRewardsUpdate(CExtendedSocket* socket, UserDailyRewards& dailyRewards) = 0;
	virtual void SendItemDailyRewardsSpinResult(CExtendedSocket* socket, RewardItem& item) = 0;
	virtual void SendItemOpenDecoderResult(CExtendedSocket* socket, ItemBoxOpenResult& result) = 0;
	virtual void SendItemOpenDecoderErrorReply(CExtendedSocket* socket, ItemBoxError code) = 0;
	virtual void SendItemEnhanceResult(CExtendedSocket* socket, EnhResult& result) = 0;
	virtual void SendItemWeaponPaintReply(CExtendedSocket* socket) = 0;
	virtual void SendItemPartCheck(CExtendedSocket* socket, int slot, int partNum) = 0;
	virtual void SendItemGachapon(CExtendedSocket* socket, int gachaponItem) = 0;

	virtual void SendLobbyJoin(CExtendedSocket* socket, CChannel* channel) = 0;
	virtual void SendLobbyUserJoin(CExtendedSocket* socket, CUser* joinedUser) = 0;
	virtual void SendLobbyUserLeft(CExtendedSocket* socket, CUser* user) = 0;

	virtual void SendRoomListFull(CExtendedSocket* socket, std::vector<CRoom*>& rooms) = 0;
	virtual void SendRoomListAdd(CExtendedSocket* socket, CRoom* room) = 0;
	virtual void SendRoomListUpdate(CExtendedSocket* socket, CRoom* room) = 0;
	virtual void SendRoomListRemove(CExtendedSocket* socket, int roomID) = 0;

	virtual void SendShopUpdate(CExtendedSocket* socket, const std::vector<Product>& products) = 0;
	virtual void SendShopBuyProductReply(CExtendedSocket* socket, int replyCode) = 0;
	virtual void SendShopReply(CExtendedSocket* socket, int replyCode) = 0;
	virtual void SendShopRecommendedProducts(CExtendedSocket* socket, const std::vector<std::vector<int>>& products) = 0;
	virtual void SendShopPopularProducts(CExtendedSocket* socket, const std::vector<int>& products) = 0;
	
	virtual void SendSearchRoomNotice(CExtendedSocket* socket, CRoom* room, std::string invitersGameName, std::string inviteMsg) = 0;

	virtual void SendRoomCreateAndJoin(CExtendedSocket* socket, CRoom* roomInfo) = 0;
	virtual void SendRoomPlayerJoin(CExtendedSocket* socket, CUser* user, RoomTeamNum num) = 0;
	virtual void SendRoomUpdateSettings(CExtendedSocket* socket, CRoomSettings* newSettings, int low = 0, int lowMid = 0, int highMid = 0, int high = 0) = 0;
	virtual void SendRoomSetUserTeam(CExtendedSocket* socket, CUser* user, int teamNum) = 0;
	virtual void SendRoomSetPlayerReady(CExtendedSocket* socket, CUser* user, RoomReadyStatus readyStatus) = 0;
	virtual void SendRoomSetHost(CExtendedSocket* socket, CUser* user) = 0;
	virtual void SendRoomPlayerLeave(CExtendedSocket* socket, int userId) = 0;
	virtual void SendRoomPlayerLeaveIngame(CExtendedSocket* socket) = 0;
	virtual void SendRoomInviteUserList(CExtendedSocket* socket, CUser* user) = 0;
	virtual void SendRoomGameResult(CExtendedSocket* socket, CRoom* room, CGameMatch* match) = 0;
	virtual void SendRoomKick(CExtendedSocket* socket, int userID) = 0;
	virtual void SendRoomInitiateVoteKick(CExtendedSocket* socket, int userID, int destUserID, int reason) = 0;
	virtual void SendRoomVoteKickResult(CExtendedSocket* socket, bool kick, int userID, int reason) = 0;
	virtual void SendRoomWeaponSurvey(CExtendedSocket* socket, std::vector<int>& weapons) = 0;
	
	virtual void SendHostOnItemUse(CExtendedSocket* socket, int userId, int itemId) = 0;
	virtual void SendHostServerJoin(CExtendedSocket* socket, int ipAddress, bool bigEndian, int port, int userId) = 0;
	virtual void SendHostStop(CExtendedSocket* socket) = 0;
	virtual void SendHostLeaveResultWindow(CExtendedSocket* socket) = 0;
	virtual void SendHostUserInventory(CExtendedSocket* socket, int userId, std::vector<CUserInventoryItem>& items) = 0;
	virtual void SendHostGameStart(CExtendedSocket* socket, int userId) = 0;
	virtual void SendHostZBAddon(CExtendedSocket* socket, int userID, std::vector<int>& addons) = 0;
	virtual void SendHostJoin(CExtendedSocket* socket, int hostID) = 0;
	virtual void SendHostFlyerFlock(CExtendedSocket* socket, int type) = 0;
	virtual void SendHostAdBalloon(CExtendedSocket* socket) = 0;
	virtual void SendHostRestart(CExtendedSocket* socket, int newHostUserID, bool host, CGameMatch* match) = 0;

	virtual void SendCharacter(CExtendedSocket* socket) = 0;

	virtual void SendEventAdd(CExtendedSocket* socket, int eventsFlag) = 0;
	virtual void SendEventUnk(CExtendedSocket* socket) = 0;
	virtual void SendEventUnk2(CExtendedSocket* socket) = 0;

	virtual void SendMiniGameBingoUpdate(CExtendedSocket* socket, UserBingo& bingo, std::vector<UserBingoSlot>& slots, std::vector<UserBingoPrizeSlot>& prizes) = 0;
	virtual void SendMiniGameWeaponReleaseUpdate(CExtendedSocket* socket, WeaponReleaseConfig& cfg, std::vector<UserWeaponReleaseRow>& rows, std::vector<UserWeaponReleaseCharacter>& characters, int totalCount) = 0;
	virtual void SendMiniGameWeaponReleaseSetCharacter(CExtendedSocket* socket, int status, int weaponSlot, int slot, int character, int charLeft) = 0;
	virtual void SendMiniGameWeaponReleaseUnk2(CExtendedSocket* socket) = 0;
	virtual void SendMiniGameWeaponReleaseIGNotice(CExtendedSocket* socket, char character) = 0;

	virtual void SendQuests(CExtendedSocket* socket, int userID, std::vector<CQuest*>& quests, std::vector<UserQuestProgress>& questsProgress, int infoFlag = 0xFFFF, int taskFlag = 0xFF, int rewardFlag = 0xFF, int statFlag = 0xFFFF) = 0;
	//void SendQuestUnk(CExtendedSocket* socket) = 0;
	virtual void SendQuestUpdateMainInfo(CExtendedSocket* socket, int flag, CQuest* quest, UserQuestProgress& questProgress) = 0;
	virtual void SendQuestUpdateTaskInfo(CExtendedSocket* socket, int flag, int questID, CQuestTask* task, UserQuestTaskProgress& taskProgress) = 0;
	virtual void SendQuestUpdateRewardInfo(CExtendedSocket* socket, int flag, int questID, QuestReward_s& reward) = 0;
	virtual void SendQuestUpdateQuestStat(CExtendedSocket* socket, int flag, int honorPoints, UserQuestStat& stat) = 0;

	virtual void SendFavoriteLoadout(CExtendedSocket* socket, int characterItemID, int currentLoadout, CUserLoadout& loadouts) = 0;
	virtual void SendFavoriteFastBuy(CExtendedSocket* socket, std::vector<CUserFastBuy>& fastbuy) = 0;
	virtual void SendFavoriteBuyMenu(CExtendedSocket* socket, std::vector<CUserBuyMenu>& buyMenu) = 0;
	virtual void SendFavoriteBookmark(CExtendedSocket* socket, std::vector<int>& bookmark) = 0;

	virtual void SendAlarm(CExtendedSocket* socket, std::vector<Notice_s>& notices) = 0;

	virtual void SendQuestUnk1(CExtendedSocket* socket) = 0;
	virtual void SendQuestUnk11(CExtendedSocket* socket) = 0;
	virtual void SendQuestUnk12(CExtendedSocket* socket) = 0;
	virtual void SendQuestUnk13(CExtendedSocket* socket) = 0;

	virtual void SendUpdateInfoNicknameChangeReply(CExtendedSocket* socket, int replyCode) = 0;

	virtual void SendTitle(CExtendedSocket* socket, int id) = 0;

	virtual void SendUDPHostData(CExtendedSocket* socket, bool host, int userID, std::string ipAddress, int port) = 0;

	virtual void SendHostServerStop(CExtendedSocket* socket) = 0;

	virtual void SendClanList(CExtendedSocket* socket, std::vector<ClanList_s>& clans, int pageID, int pageMax) = 0;
	virtual void SendClanInfo(CExtendedSocket* socket, Clan_s& clan) = 0;
	virtual void SendClanReply(CExtendedSocket* socket, int replyID, int replyCode, const char* errStr) = 0;
	virtual void SendClanJoinReply(CExtendedSocket* socket, int replyCode, const char* errStr) = 0;
	virtual void SendClanCreateUserList(CExtendedSocket* socket, std::vector<ClanUser>& users) = 0;
	virtual void SendClanUpdateUserList(CExtendedSocket* socket, ClanUser& user, bool remove = false) = 0;
	virtual void SendClanStoragePage(CExtendedSocket* socket, ClanStoragePage& clanStoragePage) = 0;
	virtual void SendClanStorageHistory(CExtendedSocket* socket) = 0;
	virtual void SendClanStorageAccessGrade(CExtendedSocket* socket, std::vector<int>& accessGrade) = 0;
	virtual void SendClanStorageReply(CExtendedSocket* socket, int replyCode, const char* errStr) = 0;
	virtual void SendClanCreateMemberUserList(CExtendedSocket* socket, std::vector<ClanUser>& users) = 0;
	virtual void SendClanUpdateMemberUserList(CExtendedSocket* socket, ClanUser& user, bool remove = false) = 0;
	virtual void SendClanCreateJoinUserList(CExtendedSocket* socket, std::vector<ClanUserJoinRequest>& users) = 0;
	virtual void SendClanUpdateJoinUserList(CExtendedSocket* socket, ClanUserJoinRequest& user, bool remove = false) = 0;
	virtual void SendClanDeleteJoinUserList(CExtendedSocket* socket) = 0;
	virtual void SendClanUpdate(CExtendedSocket* socket, int type, int memberGrade, Clan_s& clan) = 0;
	virtual void SendClanUpdateNotice(CExtendedSocket* socket, Clan_s& clan) = 0;
	virtual void SendClanMarkColor(CExtendedSocket* socket) = 0;
	virtual void SendClanMarkReply(CExtendedSocket* socket, int replyCode, const char* errStr) = 0;
	virtual void SendClanInvite(CExtendedSocket* socket, std::string inviterGameName, int clanID) = 0;
	virtual void SendClanMasterDelegate(CExtendedSocket* socket) = 0;
	virtual void SendClanKick(CExtendedSocket* socket) = 0;
	virtual void SendClanChatMessage(CExtendedSocket* socket, std::string gameName, std::string message) = 0;

	virtual void SendBanList(CExtendedSocket* socket, std::vector<std::string>& banList) = 0;
	virtual void SendBanUpdateList(CExtendedSocket* socket, std::string gameName, bool remove = false) = 0;
	virtual void SendBanSettings(CExtendedSocket* socket, int settings) = 0;
	virtual void SendBanMaxSize(CExtendedSocket* socket, int maxSize) = 0;

	virtual void SendMessengerUserInfo(CExtendedSocket* socket, int userID, CUserCharacter& character) = 0;

	virtual void SendRankReply(CExtendedSocket* socket, int replyCode) = 0;
	virtual void SendRankUserInfo(CExtendedSocket* socket, int userID, CUserCharacter& character) = 0;

	virtual void SendAddonPacket(CExtendedSocket* socket, std::vector<int>& addons) = 0;

	virtual void SendLeaguePacket(CExtendedSocket* socket) = 0;
	virtual void SendLeagueGaugePacket(CExtendedSocket* socket, int gameModeId) = 0;
};
