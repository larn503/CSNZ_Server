#pragma once

#include "UserManager.h"
#include "Definitions.h"
#include "Quest.h"
#include "QuestEvent.h"
#include "IQuestManager.h"

class CQuestManager : public CBaseManager, public IQuestManager
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
	void OnPacket(CReceivePacket* msg, CExtendedSocket* socket);
	void OnTitlePacket(CReceivePacket* msg, CExtendedSocket* socket);

	void OnMatchMinuteTick(CGameMatchUserStat* userStat, CGameMatch* gameMatch);

	// ingame events
	void OnKillEvent(CGameMatchUserStat* userStat, CGameMatch* gameMatch, GameMatch_KillEvent& killEvent);
	void OnBombExplode(CGameMatchUserStat* userStat, CGameMatch* gameMatch);
	void OnBombDefuse(CGameMatchUserStat* userStat, CGameMatch* gameMatch);
	void OnHostageEscape(CGameMatchUserStat* userStat, CGameMatch* gameMatch);
	void OnMonsterKill(CGameMatchUserStat* userStat, CGameMatch* gameMatch, int monsterType);
	void OnMosquitoKill(CGameMatchUserStat* userStat, CGameMatch* gameMatch);
	void OnKiteKill(CGameMatchUserStat* userStat, CGameMatch* gameMatch);

	void OnLevelUpEvent(CUser* user, int level, int newLevel);
	void OnMatchEndEvent(CGameMatchUserStat* userStat, CGameMatch* gameMatch, int userTeam);
	void OnGameMatchLeave(CUser* user, std::vector<UserQuestProgress>& questProgress, std::vector<UserQuestProgress>& questsEventsProgress);
	void OnUserLogin(CUser* user);

	void OnQuestTaskFinished(CUser* user, UserQuestTaskProgress& taskProgress, CQuestTask* task, CQuest* quest);
	void OnQuestEventTaskFinished(CUser* user, UserQuestTaskProgress& taskProgress, CQuestEventTask* task, CQuestEvent* quest);
	void OnQuestFinished(CUser* user, CQuest* quest, UserQuestProgress& questProgress);
	void OnReceiveReward(CUser* user, int rewardID, int questID);
	void OnSpecialMissionRequest(CUser* user, CReceivePacket* msg);
	void OnSetFavouriteRequest(CUser* user, CReceivePacket* msg);
	void OnTitleSetPrefixRequest(CUser* user, CReceivePacket* msg);
	void OnTitleListSetRequest(CUser* user, CReceivePacket* msg);
	void OnTitleListRemoveRequest(CUser* user, CReceivePacket* msg);

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