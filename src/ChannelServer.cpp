#include "ChannelServer.h"

using namespace std;

CChannelServer::CChannelServer(string serverName, int serverIndex, int totalServers, int numOfChannels)
{
	name = FormatServerName(serverName, serverIndex, totalServers);
	index = serverIndex;
	nextChannelId = 1;

	for (index = 0; index < numOfChannels; index++)
	{
		int newChannelIndex = nextChannelId;
		string newChannelName = FormatChannelName(serverName, serverIndex, newChannelIndex);
		channels.push_back(new CChannel(this, newChannelIndex, newChannelName, 100, ""));
		nextChannelId++;
	}
}
string CChannelServer::FormatServerName(string serverName, int serverIndex, int totalServers)
{
	/*
	//иновационный подход
	//(почему-то std::operator+ для std::string не перегружен для работы с int,
	//будем делать с СИ хуйней пока что):

	string temp = "";
	char formatted[128];
	sprintf(formatted, "%s [%i / %i]", serverName.c_str(), serverIndex, totalServers);
	temp += formatted;
	return temp;
	*/

	string name(va("%s [%i / %i]", serverName.c_str(), serverIndex, totalServers));
	return name;
}

string CChannelServer::FormatChannelName(string serverName, int serverIndex, int channedlIndex)
{
	/*
	//иновационный подход
	//(почему-то std::operator+ для std::string не перегружен для работы с int,
	//будем делать с СИ хуйней пока что):

	string temp = "";
	char formatted[128];
	sprintf(formatted, "%s %i-%i", serverName.c_str(), serverIndex, channedlIndex);
	temp += formatted;
	return temp;
	*/

	string name(va("%s %i-%i", serverName.c_str(), serverIndex, channedlIndex));
	return name;
}

CChannel* CChannelServer::GetChannelByIndex(int index)
{
	for (auto channel : channels)
	{
		if (channel->m_nIndex == index)
			return channel;
	}

	return NULL;
}