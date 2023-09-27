#pragma once

class IQuestManager : public IBaseManager
{
public:
	virtual void LoadQuests() = 0;
	virtual void LoadEventQuests() = 0;
	virtual void LoadClanQuests() = 0;
	virtual std::vector<CQuest*>& GetQuests() = 0;
	virtual void OnPacket(CReceivePacket* msg, CExtendedSocket* socket) = 0;
	virtual void OnTitlePacket(CReceivePacket* msg, CExtendedSocket* socket) = 0;

	virtual void OnMatchMinuteTick(CGameMatchUserStat* userStat, CGameMatch* gameMatch) = 0;

	// ingame events
	virtual void OnKillEvent(CGameMatchUserStat* userStat, CGameMatch* gameMatch, GameMatch_KillEvent& killEvent) = 0;
	virtual void OnBombExplode(CGameMatchUserStat* userStat, CGameMatch* gameMatch) = 0;
	virtual void OnBombDefuse(CGameMatchUserStat* userStat, CGameMatch* gameMatch) = 0;
	virtual void OnHostageEscape(CGameMatchUserStat* userStat, CGameMatch* gameMatch) = 0;
	virtual void OnMonsterKill(CGameMatchUserStat* userStat, CGameMatch* gameMatch, int monsterType) = 0;
	virtual void OnMosquitoKill(CGameMatchUserStat* userStat, CGameMatch* gameMatch) = 0;
	virtual void OnKiteKill(CGameMatchUserStat* userStat, CGameMatch* gameMatch) = 0;

	virtual void OnLevelUpEvent(CUser* user, int level, int newLevel) = 0;
	virtual void OnMatchEndEvent(CGameMatchUserStat* userStat, CGameMatch* gameMatch, int userTeam) = 0;
	virtual void OnGameMatchLeave(CUser* user, std::vector<UserQuestProgress>& questProgress, std::vector<UserQuestProgress>& questsEventsProgress) = 0;
	virtual void OnUserLogin(CUser* user) = 0;

	virtual void OnQuestTaskFinished(CUser* user, UserQuestTaskProgress& taskProgress, CQuestTask* task, CQuest* quest) = 0;
	virtual void OnQuestEventTaskFinished(CUser* user, UserQuestTaskProgress& taskProgress, CQuestEventTask* task, CQuestEvent* quest) = 0;
	virtual void OnQuestFinished(CUser* user, CQuest* quest, UserQuestProgress& questProgress) = 0;
	virtual void OnReceiveReward(CUser* user, int rewardID, int questID) = 0;
	virtual void OnSpecialMissionRequest(CUser* user, CReceivePacket* msg) = 0;
	virtual void OnSetFavouriteRequest(CUser* user, CReceivePacket* msg) = 0;
	virtual void OnTitleSetPrefixRequest(CUser* user, CReceivePacket* msg) = 0;
	virtual void OnTitleListSetRequest(CUser* user, CReceivePacket* msg) = 0;
	virtual void OnTitleListRemoveRequest(CUser* user, CReceivePacket* msg) = 0;

	virtual CQuest* GetQuest(int questID) = 0;
	virtual CQuestEvent* GetEventQuest(int questID) = 0;
	virtual QuestReward_s GetQuestReward(CQuest* quest, int rewardID) = 0;
	virtual UserQuestProgress GetUserQuestProgress(CQuest* quest, int userID) = 0;
	virtual UserQuestTaskProgress GetUserQuestTaskProgress(CQuest* quest, int userID, int taskID) = 0;
};