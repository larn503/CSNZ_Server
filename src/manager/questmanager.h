#pragma once

#include "interface/iquestmanager.h"
#include "manager.h"

#include "definitions.h"
#include "quest/quest.h"
#include "quest/questevent.h"

class CQuestManager : public CBaseManager<IQuestManager>
{
public:
	CQuestManager();
	~CQuestManager();

	virtual bool Init();
	virtual void Shutdown();

	void LoadQuests();
	void LoadEventQuests();
	void LoadClanQuests();
	std::vector<CQuest*>& GetQuests();
	void OnPacket(CReceivePacket* msg, IExtendedSocket* socket);
	void OnTitlePacket(CReceivePacket* msg, IExtendedSocket* socket);

	void OnMatchMinuteTick(CGameMatchUserStat* userStat, CGameMatch* gameMatch);

	// ingame events
	void OnKillEvent(CGameMatchUserStat* userStat, CGameMatch* gameMatch, GameMatch_KillEvent& killEvent);
	void OnBombExplode(CGameMatchUserStat* userStat, CGameMatch* gameMatch);
	void OnBombDefuse(CGameMatchUserStat* userStat, CGameMatch* gameMatch);
	void OnHostageEscape(CGameMatchUserStat* userStat, CGameMatch* gameMatch);
	void OnMonsterKill(CGameMatchUserStat* userStat, CGameMatch* gameMatch, int monsterType);
	void OnMosquitoKill(CGameMatchUserStat* userStat, CGameMatch* gameMatch);
	void OnKiteKill(CGameMatchUserStat* userStat, CGameMatch* gameMatch);

	void OnLevelUpEvent(IUser* user, int level, int newLevel);
	void OnMatchEndEvent(CGameMatchUserStat* userStat, CGameMatch* gameMatch, int userTeam);
	void OnGameMatchLeave(IUser* user, std::vector<UserQuestProgress>& questProgress, std::vector<UserQuestProgress>& questsEventsProgress);
	void OnUserLogin(IUser* user);

	void OnQuestTaskFinished(IUser* user, UserQuestTaskProgress& taskProgress, CQuestTask* task, CQuest* quest);
	void OnQuestEventTaskFinished(IUser* user, UserQuestTaskProgress& taskProgress, CQuestEventTask* task, CQuestEvent* quest);
	void OnQuestFinished(IUser* user, CQuest* quest, UserQuestProgress& questProgress);
	void OnReceiveReward(IUser* user, int rewardID, int questID);
	void OnSpecialMissionRequest(IUser* user, CReceivePacket* msg);
	void OnSetFavouriteRequest(IUser* user, CReceivePacket* msg);
	void OnTitleSetPrefixRequest(IUser* user, CReceivePacket* msg);
	void OnTitleListSetRequest(IUser* user, CReceivePacket* msg);
	void OnTitleListRemoveRequest(IUser* user, CReceivePacket* msg);

	CQuest* GetQuest(int questID);
	CQuestEvent* GetEventQuest(int questID);
	QuestReward_s GetQuestReward(CQuest* quest, int rewardID);
	UserQuestProgress GetUserQuestProgress(CQuest* quest, int userID);
	UserQuestTaskProgress GetUserQuestTaskProgress(CQuest* quest, int userID, int taskID);

private:
	std::vector<CQuest*> m_Quests;
	std::vector<CQuestEvent*> m_EventQuests;
	std::vector<CQuest*> m_ClanQuests;
};

extern class CQuestManager* g_pQuestManager;