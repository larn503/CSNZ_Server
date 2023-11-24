#pragma once

#include "IEvent.h"
#include "Common/Thread.h"

class CEvent : public IEvent
{
public:
	CEvent()
	{
	}

	~CEvent()
	{
	}

	virtual void AddEvent(const Event_s& ev)
	{
		m_Mutex.Enter();

		if (m_Events.size() >= 50)
			printf("m_Events.size() >= 50\n");

		m_Events.push_back(ev);
		m_Object.Signal();

		m_Mutex.Leave();
	}

	virtual void AddEventConsoleCommand(const std::string& cmd)
	{
		Event_s ev;
		ev.type = SERVER_EVENT_CONSOLE_COMMAND;
		ev.cmd = cmd;

		AddEvent(ev);
	}

	virtual void AddEventPacket(IExtendedSocket* socket, CReceivePacket* packet)
	{
		Event_s ev;
		ev.type = SERVER_EVENT_TCP_PACKET;
		ev.msg = packet;
		ev.socket = socket;

		AddEvent(ev);
	}

	virtual void AddEventSecondTick()
	{
		Event_s ev;
		ev.type = SERVER_EVENT_SECOND_TICK;

		AddEvent(ev);
	}

	virtual void AddEventFunction(const std::function<void()>& func)
	{
		Event_s ev;
		ev.type = SERVER_EVENT_FUNCTION;
		ev.func = func;

		AddEvent(ev);
	}

	Event_s GetNextEvent(bool& empty)
	{
		Event_s ev;

		m_Mutex.Enter();

		empty = m_Events.empty();
		if (!empty)
		{
			ev = m_Events.front();
			m_Events.erase(m_Events.begin());
		}

		m_Mutex.Leave();

		return ev;
	}

	void WaitForSignal()
	{
		m_Object.WaitForSignal();
	}

	void Signal()
	{
		m_Object.Signal();
	}

private:
	std::vector<Event_s> m_Events;
	CObjectSync m_Object;
	CCriticalSection m_Mutex;
};