#pragma once

#include "Channel.h"

class CChannelServer
{
public:
	CChannelServer(std::string serverName, int serverIndex, int totalServers, int numOfChannels);
	CChannel* GetChannelByIndex(int index);

	std::string name;
	int index;
	std::vector<CChannel*> channels;

private:
	std::string FormatServerName(std::string serverName, int serverIndex, int totalServers);
	std::string FormatChannelName(std::string serverName, int serverIndex, int channelNumber);
	int nextChannelId; //228
};