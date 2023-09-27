#pragma once

class IServerInstance
{
public:
	virtual bool LoadConfigs() = 0;
	virtual void UnloadConfigs() = 0;
	virtual void ListenTCP() = 0;
	virtual void ListenUDP() = 0;
	virtual void SetServerActive(bool active) = 0;
	virtual bool IsServerActive() = 0;
	virtual void OnCommand(std::string command) = 0;
	virtual void OnEvent() = 0;
	virtual void OnPackets(CExtendedSocket* s, std::vector<CReceivePacket*>& msgs) = 0;
	virtual void OnSecondTick() = 0;
	virtual void OnMinuteTick() = 0;
	virtual void UpdateConsoleStatus() = 0;
	virtual time_t GetCurrentTime() = 0;
	virtual tm* GetCurrentLocalTime() = 0;
	virtual const char* GetMemoryInfo() = 0;
	virtual void DisconnectClient(CExtendedSocket* socket) = 0;
};