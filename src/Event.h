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

		m_Events.push_back(ev);
		m_Object.Signal();

		m_Mutex.Leave();
	}

	virtual void AddEventFunction(const std::function<void()>& func)
	{
		m_Mutex.Enter();

		Event_s ev;
		ev.type = SERVER_EVENT_FUNCTION;
		ev.funcs.push_back(func);
		m_Object.Signal();

		m_Mutex.Leave();
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