#pragma once

#include "Channel.h"

class CChannelServer
{
public:
	CChannelServer(std::string serverName, int serverIndex, int totalServers, int numOfChannels);
	~CChannelServer();

	CChannel* GetChannelByIndex(int index);
	std::vector<CChannel*> GetChannels();
	std::string GetName();
	int GetID();

private:
	std::string FormatServerName(std::string serverName, int serverIndex, int totalServers);
	std::string FormatChannelName(std::string serverName, int serverIndex, int channelNumber);

	int m_nIndex;
	int m_nNextChannelID;
	std::vector<CChannel*> m_Channels;
	std::string m_Name;
};