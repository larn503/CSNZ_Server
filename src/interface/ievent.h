#pragma once

#include <string>
#include <vector>
#include <functional>

enum ServerEvent
{
	SERVER_EVENT_CONSOLE_COMMAND = 0,
	SERVER_EVENT_TCP_PACKET = 1,
	SERVER_EVENT_SECOND_TICK = 2,
	SERVER_EVENT_FUNCTION = 3,
};

class IExtendedSocket;
class CReceivePacket;

struct Event_s
{
	int type;
	std::string cmd;
	IExtendedSocket* socket;
	CReceivePacket* msg;
	std::function<void()> func;
};

class IEvent
{
public:
	virtual void AddEvent(const Event_s& ev) = 0;
	virtual void AddEventConsoleCommand(const std::string& cmd) = 0;
	virtual void AddEventPacket(IExtendedSocket* socket, CReceivePacket* packet) = 0;
	virtual void AddEventSecondTick() = 0;
	virtual void AddEventFunction(const std::function<void()>& func) = 0;
};
