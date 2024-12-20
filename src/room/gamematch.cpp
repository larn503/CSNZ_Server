#include "gamematch.h"
#include "serverconfig.h"
#include "manager/questmanager.h"
#include "manager/packetmanager.h"
#include "manager/minigamemanager.h"
#include "manager/itemmanager.h"
#include "manager/hostmanager.h"
#include "manager/userdatabase.h"

#include "user/userinventoryitem.h"

using namespace std;

CGameMatchUserStat::CGameMatchUserStat()
{
	m_pUser = NULL;
	m_nScore = 0;
	m_nKills = 0;
	m_nDeaths = 0;
	m_nPointsEarned = 0;
	m_nBonusPointsEarned = 0;
	m_nExpEarned = 0;
	m_nBonusExpEarned = 0;
	m_nItemBonusPoints = 0;
	m_nItemBonusExp = 0;
	m_nClassBonusPoints = 0;
	m_nClassBonusExp = 0;
	m_nClanBonusPoints = 0;
	m_nClanBonusExp = 0;
	m_nEventBonusPoints = 0;
	m_nEventBonusExp = 0;
	m_nPlayerCoopBonusPoints = 0;
	m_nPlayerCoopBonusExp = 0;
	m_nClassItemID = 0;
	m_nIsLevelUp = false;
	m_nRank = 0;
	m_nElites = 0;
}

void CGameMatchUserStat::IncrementScore(int score)
{
	m_nScore += score;
}

void CGameMatchUserStat::UpdateKillsCount(int kills)
{
	m_nKills = kills;
}

void CGameMatchUserStat::UpdateDeathsCount(int deaths)
{
	m_nDeaths = deaths;
}

void CGameMatchUserStat::UpdateClass(int classItemID)
{
	m_nClassItemID = classItemID;
}

void CGameMatchUserStat::UpdateItems(vector<CUserInventoryItem> items)
{
	m_Items = items;
}

void CGameMatchUserStat::UpdateZbsRank(int rankPoints)
{
	if (rankPoints < 10)
	{
		m_nRank = 0;
	}
	else if (rankPoints < 25)
	{
		m_nRank = 1;
	}
	else if (rankPoints < 40)
	{
		m_nRank = 2;
	}
	else if (rankPoints < 50)
	{
		m_nRank = 3;
	}
	else if (rankPoints >= 50)
	{
		m_nRank = 4;
	}
}

void CGameMatchUserStat::IncrementKillCount()
{
	m_nKills++;
}

UserQuestProgress& CGameMatchUserStat::GetTempQuestProgress(int questID)
{
	UserQuestProgress tempQuestProgress = {};
	auto tempQuestTaskProgressIt = find_if(m_TempQuestProgress.begin(), m_TempQuestProgress.end(),
		[questID](const UserQuestProgress& tempQuestProgress) { return tempQuestProgress.questID == questID; });

	if (tempQuestTaskProgressIt != m_TempQuestProgress.end())
	{
		return *tempQuestTaskProgressIt;
	}
	else
	{
		g_UserDatabase.GetQuestProgress(m_pUser->GetID(), questID, tempQuestProgress);
		m_TempQuestProgress.push_back(tempQuestProgress);
	}

	return m_TempQuestProgress[m_TempQuestProgress.size() - 1];
}

UserQuestTaskProgress& CGameMatchUserStat::GetTempQuestTaskProgress(int questID, int taskID)
{
	UserQuestTaskProgress tempQuestTaskProgress = {};
	UserQuestProgress& tempQuestProgress = GetTempQuestProgress(questID);

	auto tempTaskProgressIt = find_if(tempQuestProgress.tasks.begin(), tempQuestProgress.tasks.end(),
		[taskID](const UserQuestTaskProgress& taskProgress) { return taskProgress.taskID == taskID; });

	if (tempTaskProgressIt != tempQuestProgress.tasks.end())
	{
		return *tempTaskProgressIt;
	}
	else
	{
		g_UserDatabase.GetQuestTaskProgress(m_pUser->GetID(), questID, taskID, tempQuestTaskProgress);
		tempQuestProgress.tasks.push_back(tempQuestTaskProgress);
	}

	return tempQuestProgress.tasks[tempQuestProgress.tasks.size() - 1];
}

UserQuestProgress& CGameMatchUserStat::GetTempQuestEventProgress(int questID)
{
	UserQuestProgress tempQuestProgress = {};
	auto tempQuestTaskProgressIt = find_if(m_TempQuestEventProgress.begin(), m_TempQuestEventProgress.end(),
		[questID](const UserQuestProgress& tempQuestProgress) { return tempQuestProgress.questID == questID; });

	if (tempQuestTaskProgressIt != m_TempQuestEventProgress.end())
	{
		return *tempQuestTaskProgressIt;
	}
	else
	{
		g_UserDatabase.GetQuestEventProgress(m_pUser->GetID(), questID, tempQuestProgress);
		m_TempQuestEventProgress.push_back(tempQuestProgress);
	}

	return m_TempQuestEventProgress[m_TempQuestEventProgress.size() - 1];
}

UserQuestTaskProgress& CGameMatchUserStat::GetTempQuestEventTaskProgress(int questID, int taskID)
{
	UserQuestTaskProgress tempQuestTaskProgress = {};
	UserQuestProgress& tempQuestEventProgress = GetTempQuestEventProgress(questID);

	auto tempTaskProgressIt = find_if(tempQuestEventProgress.tasks.begin(), tempQuestEventProgress.tasks.end(),
		[taskID](const UserQuestTaskProgress& taskProgress) { return taskProgress.taskID == taskID; });

	if (tempTaskProgressIt != tempQuestEventProgress.tasks.end())
	{
		return *tempTaskProgressIt;
	}
	else
	{
		g_UserDatabase.GetQuestEventTaskProgress(m_pUser->GetID(), questID, taskID, tempQuestTaskProgress);
		tempQuestEventProgress.tasks.push_back(tempQuestTaskProgress);
	}

	return tempQuestEventProgress.tasks[tempQuestEventProgress.tasks.size() - 1];
}

CUserInventoryItem* CGameMatchUserStat::GetItem(int itemID)
{
	auto itemIt = find_if(m_Items.begin(), m_Items.begin(),
		[itemID](const CUserInventoryItem& item) { return item.m_nItemID == itemID; });

	if (itemIt != m_Items.end())
	{
		return &*itemIt;
	}

	return NULL;
}

CGameMatch::CGameMatch(IRoom* room, int gameMode, int mapID)
{
	m_pParentRoom = room;
	m_nGameMode = gameMode;
	m_nMapID = mapID;
	m_nCtWinCount = 0;
	m_nTerWinCount = 0;
	m_nFirstPlaceUserId = 0;
	m_nSecondCounter = 0;
	m_nZbsWin = false;
}

CGameMatch::~CGameMatch()
{
	for (auto u : m_UserStats)
	{
		g_QuestManager.OnGameMatchLeave(u->m_pUser, u->m_TempQuestProgress, u->m_TempQuestEventProgress);
		delete u;
	}
}

void CGameMatch::Connect(IUser* user)
{
	if (GetGameUserStat(user) == NULL)
	{
		CGameMatchUserStat* userStat = new CGameMatchUserStat();
		userStat->m_pUser = user;

		m_UserStats.push_back(userStat);
	}
}

void CGameMatch::Disconnect(IUser* user)
{
	CGameMatchUserStat* stat = GetGameUserStat(user);
	if (stat)
	{
		g_QuestManager.OnGameMatchLeave(user, stat->m_TempQuestProgress, stat->m_TempQuestEventProgress);

		delete stat;
		m_UserStats.erase(remove(begin(m_UserStats), end(m_UserStats), stat), end(m_UserStats));
	}
}

bool GameMatchUserStatCompare(CGameMatchUserStat* gameStat, IUser* user)
{
	return gameStat->m_pUser == user;
}

CGameMatchUserStat* CGameMatch::GetGameUserStat(IUser* user)
{
	auto it = find_if(m_UserStats.begin(), m_UserStats.end(), bind(GameMatchUserStatCompare, placeholders::_1, user));

	if (it != m_UserStats.end())
	{
		CGameMatchUserStat* stat = *it;
		if (stat)
		{
			return stat;
		}
		else
		{
			Logger().Warn("CGameMatch::GetGameUserStat: user stat does not exist\n");
			return NULL;
		}
	}
	else
	{
		return NULL;
	}
}

void CGameMatch::OnUpdateScore(IUser* user, int score)
{
	CGameMatchUserStat* stat = GetGameUserStat(user);

	if (stat)
	{
		stat->IncrementScore(score);
	}
}

void CGameMatch::OnUpdateKillCounter(IUser* user, int kills)
{
	CGameMatchUserStat* stat = GetGameUserStat(user);

	if (stat)
	{
		stat->UpdateKillsCount(kills);
	}
}

void CGameMatch::OnUpdateDeathCounter(IUser* user, int deaths)
{
	CGameMatchUserStat* stat = GetGameUserStat(user);

	if (stat)
	{
		stat->UpdateDeathsCount(deaths);
	}
}

void CGameMatch::OnKillEvent(IUser* user, GameMatch_KillEvent& killEvent)
{
	CGameMatchUserStat* stat = GetGameUserStat(user);
	if (!stat)
		return;

	// don't count player suicide
	if (killEvent.killerUserID != killEvent.victimUserID)
		stat->IncrementKillCount();

	g_QuestManager.OnKillEvent(stat, this, killEvent);

	// random letters for weapon release event
	if (g_pServerConfig->activeMiniGamesFlag & kEventFlag_WeaponRelease)
	{
		if (yesOrNo(2.0f))
		{
			Randomer rand(g_pServerConfig->weaponRelease.characters.size() - 1);
			char character = g_pServerConfig->weaponRelease.characters[rand()];
			g_MiniGameManager.WeaponReleaseAddCharacter(user, character, 1);
			g_PacketManager.SendUMsgNoticeMessageInChat(user->GetExtendedSocket(), va(OBFUSCATE("[Weapon Release] You have obtained '%s' character."), character == '~' ? OBFUSCATE("Joker") : va("%c", character)));
			g_PacketManager.SendMiniGameWeaponReleaseIGNotice(user->GetExtendedSocket(), character);
		}
	}

	return;
	// No longer need
	// TODO: rewrite
	// random gachapon item for each 100 kills

	CUserCharacterExtended character = {};
	character.flag = EXT_UFLAG_KILLSTOGETGACHAPONITEM;
	g_UserDatabase.GetCharacterExtended(user->GetID(), character);

	character.killsToGetGachaponItem--;
	if (character.killsToGetGachaponItem <= 0)
	{
		character.killsToGetGachaponItem = 100;

		Randomer rand(1);
		int randomIndex = rand();
		int randomItemID = randomIndex ? 166 : 5101;

		g_PacketManager.SendItemGachapon(user->GetExtendedSocket(), randomIndex);

		g_ItemManager.AddItem(user->GetID(), user, randomItemID, 1, 0);
	}

	g_UserDatabase.UpdateCharacterExtended(user->GetID(), character);
}

void CGameMatch::OnUpdateWinCounter(int ctWinCount, int tWinCount)
{
	m_nCtWinCount = ctWinCount;
	m_nTerWinCount = tWinCount;

	//UpdateRoundWinCount(ctWinCount, tWinCount);
}

void CGameMatch::OnUpdateClass(IUser* user, int classItemID)
{
	CGameMatchUserStat* stat = GetGameUserStat(user);

	if (stat)
	{
		stat->UpdateClass(classItemID);
	}
}

void CGameMatch::OnBombExplode(IUser* user)
{
	CGameMatchUserStat* stat = GetGameUserStat(user);
	if (!stat)
		return;

	g_QuestManager.OnBombExplode(stat, this);
}

void CGameMatch::OnBombDefuse(IUser* user)
{
	CGameMatchUserStat* stat = GetGameUserStat(user);
	if (!stat)
		return;

	g_QuestManager.OnBombDefuse(stat, this);
}

void CGameMatch::OnHostageEscape(IUser* user)
{
	CGameMatchUserStat* stat = GetGameUserStat(user);
	if (!stat)
		return;

	g_QuestManager.OnHostageEscape(stat, this);
}

void CGameMatch::OnMonsterKill(IUser* user, int monsterType)
{
	CGameMatchUserStat* stat = GetGameUserStat(user);
	if (!stat)
		return;	

	g_QuestManager.OnMonsterKill(stat, this, monsterType);
}

void CGameMatch::OnDropBoxPickup(IUser* user, int rewardID)
{
	CGameMatchUserStat* stat = GetGameUserStat(user);
	if (!stat)
		return;

//unnessary
#ifndef PUBLIC_RELEASE
	if (m_nGameMode != 15 && m_nGameMode != 17 && m_nGameMode != 26 && m_nGameMode != 28)
	{
		Logger().Info("[SuspectNotice] detected suspect User '%s', reason: %d, %s, %d, %d\n", user->GetLogName(), 227, "DROPBOXABUSE", rewardID, m_nGameMode);
		return;
	}

	if (rewardID < 3000 || rewardID > 3023)
	{
		Logger().Info("[SuspectNotice] detected suspect User '%s', reason: %d, %s, %d\n", user->GetLogName(), 228, "DROPBOXABUSE", rewardID);
		return;
	}
#endif	
	if (m_nGameMode == GAMEMODE_ZOMBI_SURVIVAL) {
		rewardID = ChangeRewardOnZSDifficulty(rewardID);
	}
	
	RewardNotice notice = g_ItemManager.GiveReward(user->GetID(), user, rewardID, 0, true, 0);
	if (notice.status)
		g_PacketManager.SendUMsgSystemReply(user->GetExtendedSocket(), SystemReply_Red, OBFUSCATE("EVT_SCENARIO_ITEM_POINT_REWARD"), vector<string> {to_string(notice.points)});
}

void CGameMatch::OnMosquitoKill(IUser* user)
{
	CGameMatchUserStat* stat = GetGameUserStat(user);
	if (!stat)
		return;

	g_QuestManager.OnMosquitoKill(stat, this);
}

void CGameMatch::OnKiteKill(IUser* user)
{
	CGameMatchUserStat* stat = GetGameUserStat(user);
	if (!stat)
		return;

	g_QuestManager.OnKiteKill(stat, this);
}

void CGameMatch::OnGameMatchEnd()
{
	CalculateGameResult();
	ApplyGameResult();

	//Logger().Info("CGameMatch::OnGameMatchEnd: gamemode: %d, CT rounds win count: %d, T rounds win count: %d\n", m_nGameMode, m_nCtWinCount, m_nTerWinCount);
}

int CGameMatch::GetExpCoefficient()
{
	auto it = find_if(g_pServerConfig->gameMatch.gameModeCoefficients.begin(), g_pServerConfig->gameMatch.gameModeCoefficients.end(),
		[this](const GameMatchCoefficients_s& gameMatchCoef) { return gameMatchCoef.gameMode == this->m_nGameMode; });
	if (it != g_pServerConfig->gameMatch.gameModeCoefficients.end())
	{
		return it->exp;
	}
	else
	{
		auto it = find_if(g_pServerConfig->gameMatch.gameModeCoefficients.begin(), g_pServerConfig->gameMatch.gameModeCoefficients.end(),
			[](const GameMatchCoefficients_s& gameMatchCoef) { return gameMatchCoef.gameMode == 0; });
		if (it != g_pServerConfig->gameMatch.gameModeCoefficients.end())
		{
			return it->exp;
		}
		else
		{
			return 0;
		}
	}
}

int CGameMatch::GetPointsCoefficient()
{
	auto it = find_if(g_pServerConfig->gameMatch.gameModeCoefficients.begin(), g_pServerConfig->gameMatch.gameModeCoefficients.end(),
		[this](const GameMatchCoefficients_s& gameMatchCoef) { return gameMatchCoef.gameMode == this->m_nGameMode; });
	if (it != g_pServerConfig->gameMatch.gameModeCoefficients.end())
	{
		return it->points;
	}
	else
	{
		auto it = find_if(g_pServerConfig->gameMatch.gameModeCoefficients.begin(), g_pServerConfig->gameMatch.gameModeCoefficients.end(),
			[](const GameMatchCoefficients_s& gameMatchCoef) { return gameMatchCoef.gameMode == 0; });
		if (it != g_pServerConfig->gameMatch.gameModeCoefficients.end())
		{
			return it->points;
		}
		else
		{
			return 0;
		}
	}
}

bool CGameMatch::IsZombieMode()
{
	switch (m_nGameMode)
	{
	case 8:
	case 9:
	case 14:
	case 29:
	case 45:
	case 54:
		return true;
	};

	return false;
}
	

void CGameMatch::SetSaveData(const vector<unsigned char>& saveData)
{
	m_SaveData = saveData;
}

vector<unsigned char>& CGameMatch::GetSaveData()
{
	return m_SaveData;
}

void CGameMatch::OnHostChanged(IUser* newHost)
{
	for (auto userStat : m_UserStats)
	{
		g_HostManager.OnHostChanged(userStat->m_pUser, newHost, this);
	}
}

//const char* Чайник() { return "С функцией жопа"; }

bool GameMatchUserStatSortCompare(CGameMatchUserStat* a1, CGameMatchUserStat* a2)
{
	return a1->m_nKills > a2->m_nKills;
}

void CGameMatch::CalculateFirstPlace()
{
	sort(m_UserStats.begin(), m_UserStats.end(), GameMatchUserStatSortCompare);
}

void CGameMatch::CalculateGameResult()
{
	CalculateFirstPlace();

	int expCoef = GetExpCoefficient();
	int pointsCoef = GetPointsCoefficient();
	int isSuper = m_pParentRoom->GetSettings()->superRoom;
	Logger().Debug("Match ended at room %d\n", m_pParentRoom->GetID());
	Logger().Debug("========================================================\n");
	Logger().Debug("Gamemode: %d\n", m_nGameMode);
	for (auto stat : m_UserStats)
	{
		IUser* user = stat->m_pUser;

		Logger().Debug("UserID: %d\n", stat->m_pUser->GetID());
		Logger().Debug("Kills: %d\n", stat->m_nKills);
		Logger().Debug("Deaths: %d\n", stat->m_nDeaths);
		Logger().Debug("Score: %d\n", stat->m_nScore);
		Logger().Debug("Bot Setting: %d\n", m_pParentRoom->GetSettings()->botDifficulty);

		int playerTeamRounds; int enemyTeamRounds; double botDifficultyCoef; double winnerCoef;
		switch (m_nGameMode) {
		case 2:
		case 6:
		case 7:
		case 23:
		case 0:
			// For modes where you have 2 teams (No bots)
			playerTeamRounds = user->GetRoomData()->m_Team == 2 ? m_nCtWinCount : m_nTerWinCount;
			enemyTeamRounds = user->GetRoomData()->m_Team == 2 ? m_nTerWinCount : m_nCtWinCount;
			winnerCoef = 1.0;
			if (enemyTeamRounds > playerTeamRounds) {
				winnerCoef = 0.5;
			}
			else if (enemyTeamRounds == playerTeamRounds) {
				winnerCoef = 0.75;
			}
			stat->m_nExpEarned = ((stat->m_nKills + playerTeamRounds) * expCoef * winnerCoef);
			stat->m_nPointsEarned = ((stat->m_nKills + playerTeamRounds) * pointsCoef * winnerCoef);
			break;
		case 3:
			// For modes where you have 2 teams and bots (Bot Original)
			playerTeamRounds = user->GetRoomData()->m_Team == 2 ? m_nCtWinCount : m_nTerWinCount;
			enemyTeamRounds = user->GetRoomData()->m_Team == 2 ? m_nTerWinCount : m_nCtWinCount;
			winnerCoef = 1.0;
			if (enemyTeamRounds > playerTeamRounds) {
				winnerCoef = 0.5;
			}
			else if (enemyTeamRounds == playerTeamRounds) {
				winnerCoef = 0.75;
			}
			botDifficultyCoef = 0.2 + m_pParentRoom->GetSettings()->botDifficulty * 0.2;
			stat->m_nExpEarned = ((stat->m_nKills + playerTeamRounds) * expCoef * winnerCoef * botDifficultyCoef);
			stat->m_nPointsEarned = ((stat->m_nKills + playerTeamRounds) * pointsCoef * winnerCoef * botDifficultyCoef);
			break;
		case 4:
			// For Bot Deathmatch 
			botDifficultyCoef = 0.2 + m_pParentRoom->GetSettings()->botDifficulty * 0.2;
			stat->m_nExpEarned = stat->m_nKills > 0 ? expCoef * stat->m_nKills * botDifficultyCoef : expCoef;
			stat->m_nPointsEarned = stat->m_nKills > 0 ? pointsCoef * stat->m_nKills * botDifficultyCoef : pointsCoef;
			break;
		case 5:
			// For Bot Team Deathmatch 
			playerTeamRounds = user->GetRoomData()->m_Team == 2 ? m_nCtWinCount : m_nTerWinCount;
			enemyTeamRounds = user->GetRoomData()->m_Team == 2 ? m_nTerWinCount : m_nCtWinCount;
			winnerCoef = 1.0;
			if (enemyTeamRounds > playerTeamRounds) {
				winnerCoef = 0.5;
			}
			else if (enemyTeamRounds == playerTeamRounds) {
				winnerCoef = 0.75;
			}
			botDifficultyCoef = 0.2 + m_pParentRoom->GetSettings()->botDifficulty * 0.2;
			stat->m_nExpEarned = stat->m_nKills > 0 ? expCoef * stat->m_nKills * winnerCoef * botDifficultyCoef : expCoef;
			stat->m_nPointsEarned = stat->m_nKills > 0 ? pointsCoef * stat->m_nKills * winnerCoef * botDifficultyCoef : pointsCoef;
			break;
		case 1:
		case 22:
			// Gun Deathmatch and modes with 1st place winner
			// need test
			if (m_nFirstPlaceUserId == user->GetID()) {
				winnerCoef = 1;
			} else {
				winnerCoef = 0.5;
			}
			botDifficultyCoef = 1.0;
			if (m_pParentRoom->GetSettings()->botAdd == 1) {
				botDifficultyCoef = 0.5 + m_pParentRoom->GetSettings()->botDifficulty * 0.5;
			}
			stat->m_nExpEarned = stat->m_nKills > 0 ? expCoef * stat->m_nKills * botDifficultyCoef * winnerCoef : expCoef;
			stat->m_nPointsEarned = stat->m_nKills > 0 ? pointsCoef * stat->m_nKills * botDifficultyCoef * winnerCoef : pointsCoef;
			break;
		case 24:
			//Bot zombie
			botDifficultyCoef = 0.5 + m_pParentRoom->GetSettings()->botDifficulty * 0.25;
			stat->m_nExpEarned = stat->m_nKills > 0 ? expCoef * stat->m_nKills * botDifficultyCoef : expCoef;
			stat->m_nPointsEarned = stat->m_nKills > 0 ? pointsCoef * stat->m_nKills * botDifficultyCoef : pointsCoef;
			break;
		case 45:
		case 14:
			//Zombie Hero and Zombie Z
			botDifficultyCoef = 1.0;
			if (m_pParentRoom->GetSettings()->botAdd == 1) {
				botDifficultyCoef = 1.0 + m_pParentRoom->GetSettings()->botDifficulty * 0.25;
			}
			stat->m_nExpEarned = stat->m_nKills > 0 ? expCoef * stat->m_nKills * botDifficultyCoef : expCoef;
			stat->m_nPointsEarned = stat->m_nKills > 0 ? pointsCoef * stat->m_nKills * botDifficultyCoef : pointsCoef;
			break;
		case 15:
			//Zombie scenario
			botDifficultyCoef = 0.5;
			if (m_nZbsWin) {
				botDifficultyCoef = GetZSDifficultyCoef(m_pParentRoom->GetSettings()->zsDifficulty);
			}
			stat->m_nExpEarned = stat->m_nKills > 0 ? expCoef * stat->m_nKills * botDifficultyCoef : expCoef;
			stat->m_nPointsEarned = stat->m_nKills > 0 ? pointsCoef * stat->m_nKills * botDifficultyCoef : pointsCoef;
			break;
		default:
			stat->m_nExpEarned = stat->m_nKills > 0 ? expCoef * stat->m_nKills : expCoef;
			stat->m_nPointsEarned = stat->m_nKills > 0 ? pointsCoef * stat->m_nKills : pointsCoef;
			break;
		}
		Logger().Debug("XP Earned: %d\n", stat->m_nExpEarned);
		Logger().Debug("Points Earned: %d\n", stat->m_nPointsEarned);

		// calc bonus exp/points
		// not works correct if you was zombie at the end of the match
		
		/*auto bonusPercentageClass = find_if(g_pServerConfig->gameMatch.bonusPercentageClasses.begin(), g_pServerConfig->gameMatch.bonusPercentageClasses.end(),
			[stat](const BonusPercentage_s& bonus) { return bonus.itemID == stat->m_nClassItemID; });
		if (bonusPercentageClass != g_pServerConfig->gameMatch.bonusPercentageClasses.end())
		{
			BonusPercentage_s bonus = *bonusPercentageClass;
			CUserInventoryItem item;
			g_UserDatabase.GetFirstActiveItemByItemID(user->GetID(), bonus.itemID, item);
			if (item.m_nItemID)
			{
				stat->m_nBonusExpEarned += stat->m_nExpEarned * bonus.exp / 100;
				stat->m_nBonusPointsEarned += stat->m_nPointsEarned * bonus.points / 100;
				stat->m_nClassBonusExp += bonus.exp;
				stat->m_nClassBonusPoints += bonus.points;
			}
		}*/

		Logger().Debug("Bonus Class XP Earned: %d\n", stat->m_nBonusExpEarned);
		Logger().Debug("Bonus Class Points Earned: %d\n", stat->m_nBonusPointsEarned);
		int bonusExpClass = stat->m_nBonusExpEarned;
		int bonusPointsClass = stat->m_nBonusPointsEarned;

		// Get bonus for every active item with bonus xp and points
		for (auto bonusItem : g_pServerConfig->gameMatch.bonusPercentageItems)
		{
			//Logger().Debug("Bonus Item %d with %d exp and %d points\n", bonusItem.itemID, bonusItem.exp, bonusItem.points);
			CUserInventoryItem item;
			g_UserDatabase.GetFirstActiveItemByItemID(user->GetID(), bonusItem.itemID, item);
			if (item.m_nItemID)
			{
				//Logger().Debug("Yes we have that item, trying to add %d exp and %d points\n", stat->m_nExpEarned * bonusItem.exp / 100, stat->m_nPointsEarned * bonusItem.points / 100);
				stat->m_nBonusExpEarned += stat->m_nExpEarned * bonusItem.exp / 100;
				stat->m_nBonusPointsEarned += stat->m_nPointsEarned * bonusItem.points / 100;
			}
		}

		

		ServerConfigGameMatch_s gameMatch = g_pServerConfig->gameMatch;


		// bonus for players
		if (m_UserStats.size() >= 2)
		{
			auto bonusPlayerCoop = find_if(gameMatch.bonusPlayerCoop.begin(), gameMatch.bonusPlayerCoop.end(), [this](const BonusPercentage_s& bonus) { return bonus.itemID == this->m_nGameMode; });
			if (bonusPlayerCoop != gameMatch.bonusPlayerCoop.end())
			{
				BonusPercentage_s bonus = *bonusPlayerCoop;

				int percentageExp = bonus.exp * bonus.coef * m_UserStats.size();
				int percentagePoints = bonus.points * bonus.coef * m_UserStats.size();

				stat->m_nBonusExpEarned += stat->m_nExpEarned * percentageExp / 100;
				stat->m_nBonusPointsEarned += stat->m_nPointsEarned * percentagePoints / 100;

				stat->m_nPlayerCoopBonusExp += percentageExp;
				stat->m_nPlayerCoopBonusPoints += percentagePoints;
			}
		}

		//Also if we have premium room we need to add other users 10% if they don't have premium
		if (isSuper == 1) {
			CUserInventoryItem item;
			g_UserDatabase.GetFirstActiveItemByItemID(user->GetID(), 8357, item);
			if (!item.m_nItemID) {
				stat->m_nBonusExpEarned += stat->m_nExpEarned * 10 / 100;
				stat->m_nBonusPointsEarned += stat->m_nPointsEarned * 10 / 100;
			}
		}

		Logger().Debug("Bonus Item XP Earned: %d\n", stat->m_nBonusExpEarned - bonusExpClass);
		Logger().Debug("Bonus Item Points Earned: %d\n\n", stat->m_nBonusPointsEarned - bonusPointsClass);

		//if (stat->m_nExpEarned > 9999)
		//{
		//	stat->m_nExpEarned = 9999;
		//}
		//if (stat->m_nPointsEarned > 9999)
		//{
		//	stat->m_nPointsEarned = 9999;
		//}
	}
	Logger().Debug("========================================================\n");
}

void CGameMatch::ApplyGameResult()
{
	for (auto stat : m_UserStats)
	{
		IUser* user = stat->m_pUser;

		int totalExp = stat->m_nExpEarned + stat->m_nBonusExpEarned;
		user->UpdateExp(totalExp);

		int totalPoints = stat->m_nPointsEarned + stat->m_nBonusPointsEarned;
		user->UpdatePoints(totalPoints);

		if (m_nGameMode == 0 || m_nGameMode == 1 || m_nGameMode == 2 || m_nGameMode == 6 || m_nGameMode == 22)
			user->UpdateStat(0, 0, stat->m_nKills, stat->m_nDeaths);
	}
}

void CGameMatch::PrintGameResult()
{
	// use ostringstream to reduce cpu time when printing
	char msg[2048];

	ostringstream log;
	snprintf(msg, sizeof(msg), OBFUSCATE("\nRoom (%d) game result, host time: %s, GameModeID: %d, MapID: %d\n%-6s|%-32s|%-10s|%-10s|%-10s|%-10s|%-10s\n"), m_pParentRoom->GetID(), FormatSeconds(m_nSecondCounter), m_nGameMode, m_nMapID,
		(const char*)OBFUSCATE("UserID"), (const char*)OBFUSCATE("Username"), (const char*)OBFUSCATE("Kills"), (const char*)OBFUSCATE("Deaths"), (const char*)OBFUSCATE("Score"), (const char*)OBFUSCATE("Exp"), (const char*)OBFUSCATE("Points"));
	log << msg;

	for (auto stat : m_UserStats)
	{
		snprintf(msg, sizeof(msg), OBFUSCATE("%-6d|%-32s|%-10d|%-10d|%-10d|+%-10d|+%-10d\n"), stat->m_pUser ? stat->m_pUser->GetID() : 0, stat->m_pUser ? stat->m_pUser->GetUsername().c_str() : "NONE", stat->m_nKills, stat->m_nDeaths,
			stat->m_nScore, stat->m_nExpEarned, stat->m_nPointsEarned);
		log << msg;
	}

	Logger().Info(log.str().c_str());
}

// TODO: find another way
int GetZbsMapSeason(int mapID)
{
	switch (mapID)
	{
	case 74:
	case 75:
	case 76:
	case 77:
	case 85:
	case 98:
	case 101:
		return 1;
	case 106:
	case 112:
	case 113:
	case 117:
		return 2;
	case 121:
	case 123:
	case 126:
	case 128:
		return 3;
	case 131:
	case 133:
	case 134:
	case 139:
	case 143:
		return 4;
	case 146:
	case 147:
	case 148:
	case 152:
	case 156:
	case 157:
	case 158:
	case 163:
	case 164:
	case 165:
	case 172:
	case 173:
	case 174:
		return 5;
	}

	return 0;
}

void CGameMatch::OnZBSWin()
{
	m_nZbsWin = true;
	int difficulty = m_pParentRoom->GetSettings()->zsDifficulty;
	for (auto stat : m_UserStats)
	{
		
		int rewardID = 3950;	// Easy
		switch (difficulty)
		{
		case 2:					// Normal
			rewardID = 3951;
			break;
		case 3:					// Hard
			rewardID = 2526;
			break;
		case 4:					// Very Hard
			rewardID = 2527;
			break;
		case 5:					// Hell
			rewardID = 2528;
			break;
		case 6:					// Inferno
			rewardID = 3954;
			break;
		}
		//int rewardID = stat->m_nRank <= 3 ? 100 + stat->m_nRank : 100 + stat->m_nRank + GetZbsMapSeason(m_nMapID) - 1;

		Logger().Info(OBFUSCATE("CGameMatch::OnZBSWin: rank: %d, rewardID: %d\n"), stat->m_nRank, rewardID);
		
		

		g_ItemManager.AddItem(stat->m_pUser->GetID(), stat->m_pUser, rewardID, 1, 0);

		

		// send notification about new item
		RewardItem rewardItem;
		rewardItem.itemID = rewardID;
		rewardItem.count = 1;
		rewardItem.duration = 0;

		RewardNotice rewardNotice;
		rewardNotice.status = true;
		rewardNotice.rewardId = 1;
		rewardNotice.exp = 0;
		rewardNotice.points = 0;
		rewardNotice.honorPoints = 0;
		rewardNotice.items.push_back(rewardItem);

		//Prize from elites
		for (int i = 0; i < stat->m_nElites; i++) {
			RewardNotice notice = g_ItemManager.GiveReward(stat->m_pUser->GetID(), stat->m_pUser, 3050, 0, true);
			for (auto item : notice.items) {
				//rewardNotice.items.push_back(item);
				rewardNotice += notice;
			}
		}

		if (rewardNotice.status) {
			g_PacketManager.SendUMsgRewardNotice(stat->m_pUser->GetExtendedSocket(), rewardNotice, "QUEST_REWARD_TITLE", "QUEST_REWARD_MSG", true);

			if (stat->m_pUser->IsPlaying())
				g_PacketManager.SendUMsgRewardNotice(stat->m_pUser->GetExtendedSocket(), rewardNotice, "QUEST_REWARD_TITLE", "QUEST_REWARD_MSG", true, true);
		}
	}
}

int CGameMatch::ChangeRewardOnZSDifficulty(int rewardID) {
	int difficulty = m_pParentRoom->GetSettings()->zsDifficulty;
	// Change to Easy reward
	if (difficulty < 3) {
		switch (rewardID) {
		case 3000:
		case 3001:
		case 3002:
			return rewardID + 7;
		}
	}
	// Change to Hard reward
	else if (difficulty > 3) {
		switch (rewardID) {
		case 3000:
		case 3001:
		case 3002:
			return rewardID + 3;
		}
	}
	// Stay in normal
	return rewardID;
}

double CGameMatch::GetZSDifficultyCoef(int difficulty) {
	switch (difficulty) {
		case 1:
			return 0.5;
		case 2:
			return 1.0;
		case 3:
			return 1.5;
		case 4:
			return 2.0;
		case 5:
			return 3.0;
		case 6:
			return 4.0;
	}
	return 1.0;
}