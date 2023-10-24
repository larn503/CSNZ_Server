#pragma once

#include "User.h"
#include "Room.h"

class CGameMatchUserStat
{
public:
	CGameMatchUserStat();

	void IncrementScore(int score);
	void UpdateKillsCount(int kills);
	void UpdateDeathsCount(int deaths);
	void UpdateClass(int classItemID);

	// zbs stuff
	void UpdateZbsRank(int rankPoints);

	void IncrementKillCount();

	UserQuestProgress& GetTempQuestProgress(int questID);
	UserQuestTaskProgress& GetTempQuestTaskProgress(int questID, int taskID);

	UserQuestProgress& GetTempQuestEventProgress(int questID);
	UserQuestTaskProgress& GetTempQuestEventTaskProgress(int questID, int taskID);

	CUserInventoryItem* GetItem(int itemID);

	CUser* m_pUser;
	int m_nScore;
	int m_nKills;
	int m_nDeaths;
	int m_nPointsEarned;
	int m_nBonusPointsEarned;
	int m_nExpEarned;
	int m_nBonusExpEarned;
	int m_nItemBonusPoints;
	int m_nItemBonusExp;
	int m_nClassBonusPoints;
	int m_nClassBonusExp;
	int m_nClanBonusPoints;
	int m_nClanBonusExp;
	int m_nEventBonusPoints;
	int m_nEventBonusExp;
	int m_nPlayerCoopBonusPoints;
	int m_nPlayerCoopBonusExp;
	int m_nClassItemID;
	bool m_nIsLevelUp;
	std::vector<UserQuestProgress> m_TempQuestProgress;
	std::vector<UserQuestProgress> m_TempQuestEventProgress;
	std::vector<CUserInventoryItem> m_Items;

	// zbs
	int m_nRank;
};

class CGameMatch
{
public:
	CGameMatch(CRoom* room, int gameMode, int mapID);
	~CGameMatch();

	void Connect(CUser* user);
	void Disconnect(CUser* user);
	CGameMatchUserStat* GetGameUserStat(CUser* user);

	// ingame events
	void OnUpdateScore(CUser* user, int score);
	void OnUpdateKillCounter(CUser* user, int kills);
	void OnUpdateDeathCounter(CUser* user, int deaths);
	void OnUpdateWinCounter(int ctWinCount, int tWinCount);
	void OnUpdateClass(CUser* user, int classItemID);
	void OnGameMatchEnd();
	void OnKillEvent(CUser* user, GameMatch_KillEvent& killEvent);
	void OnBombExplode(CUser* user);
	void OnBombDefuse(CUser* user);
	void OnHostageEscape(CUser* user);
	void OnMonsterKill(CUser* user, int monsterType);
	void OnDropBoxPickup(CUser* user, int rewardID);
	void OnMosquitoKill(CUser* user);
	void OnKiteKill(CUser* user);

	int GetExpCoefficient();
	int GetPointsCoefficient();
	bool IsZombieMode();
	void SetSaveData(const std::vector<unsigned char>& saveData);
	std::vector<unsigned char>& GetSaveData();
	void OnHostChanged(CUser* newHost);
	void CalculateFirstPlace();
	void CalculateGameResult();
	void ApplyGameResult();
	void PrintGameResult();

	void OnZBSWin();

	std::vector<CGameMatchUserStat*> m_UserStats;

	int m_nGameMode;
	int m_nMapID;
	int m_nCtWinCount;
	int m_nTerWinCount;
	int m_nFirstPlaceUserId;

	std::vector<unsigned char> m_SaveData;

private:
	int m_nSecondCounter;

	CRoom* m_pParentRoom;
};