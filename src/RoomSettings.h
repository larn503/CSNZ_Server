#pragma once

struct unk50_data
{
	int unk1;
	int unk2;
};

class CRoomSettings
{
public:
	CRoomSettings();
	CRoomSettings(struct IRoomOptions_s options);

	std::string roomName;
	int unk00;
	int unk01;
	int unk02;
	int unk03;
	int unk04;
	std::string password;
	int unk06;
	int unk07;
	int gameMode;
	int mapId;
	int maxPlayers;
	int winLimit;
	int neededKills;
	int gameTime;
	int roundTime;
	int armsRestriction;
	int unk16;
	int freezeTime;
	int buyTime;
	int displayNickname;
	int teamBalance;
	int unk21;
	int friendlyFire;
	int flashlight;
	int footsteps;
	int unk25;
	int unk26;
	int unk27;
	int unk28;
	int unk29;
	int unk30;
	int voiceChat;
	int status; // isIngame
	int unk33;
	std::string unk34;
	int unk35;
	int unk36;
	int unk37;
	int unk38;
	int botDifficulty;
	int friendlyBots;
	int enemyBots;
	int botBalance;
	int botAdd;
	int kdRule;
	int startingCash;
	int unk46;
	int unk47;
	int unk48;
	int unk49;
	int unk50;
	std::vector<unk50_data> unk50_vec;
	int unk51;
	int enhancement;
	int unk53;
	int zsDifficulty;
	int unk55;
	int unk56;
	int league;
	int mannerLimit;
	int unk59;
	int unk60;
	std::vector<int> zbLimit;
	int unk61;
	int unk62;
	std::vector<int> unk62_vec;
	int unk63;
	int teamSwitch;
	int unk65;
	int unk66;
	int unk67;
	int unk68;
	int unk69;
	int isZbCompetitive;
	int unk70;
	int unk71;
	int unk72;
	int unk73;
	int unk74;
	std::vector<int> unk74_vec;
	int unk75;
	int unk76;
};
