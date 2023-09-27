#pragma once

#include "main.h"

class IBaseManager
{
public:
	virtual bool Init() = 0;
	virtual void Shutdown() = 0;
	virtual void OnSecondTick(time_t curTime) = 0;
	virtual void OnMinuteTick(time_t curTime) = 0;
	virtual bool ShouldDoSecondTick() = 0;
	virtual bool ShouldDoMinuteTick() = 0;
};

// Base manager implmentation
template<class IInterface>
class CBaseManager : public IInterface
{
public:
	CBaseManager(bool secondTick = false, bool minuteTick = false)
	{
		m_bSecondTick = secondTick;
		m_bMinuteTick = minuteTick;

		Manager().AddManager(this);
	}

	virtual ~CBaseManager()
	{
		printf("~CBaseManager called, 0x%p\n\n", this);
		Manager().RemoveManager(this);
	}

	// stub methods
	virtual bool Init() { return true; }
	virtual void Shutdown() { printf("Shutdown called, 0x%p\n", this); }
	virtual void OnSecondTick(time_t curTime) {}
	virtual void OnMinuteTick(time_t curTime) {}
	virtual bool ShouldDoSecondTick() { return m_bSecondTick; }
	virtual bool ShouldDoMinuteTick() { return m_bMinuteTick; }

	void SetMinuteTick(bool tick) { m_bMinuteTick = tick; }
	void SetSecondTick(bool tick) { m_bSecondTick = tick; }

private:
	bool m_bSecondTick;
	bool m_bMinuteTick;
};

class CManager
{
public:
	bool InitAll();
	void ShutdownAll();
	void AddManager(IBaseManager* pElem);
	void RemoveManager(IBaseManager* pElem);
	void SecondTick(time_t curTime);
	void MinuteTick(time_t curTime);

private:
	std::vector<IBaseManager*> m_List;
};

extern CManager& Manager();
