#pragma once

#include <vector>
#include <string>
#include "IManager.h"

class CManager : public IManager
{
public:
	bool InitAll();
	void ShutdownAll();
	void AddManager(IBaseManager* pElem);
	void RemoveManager(IBaseManager* pElem);
	IBaseManager* GetManager(const std::string& name);
	void SecondTick(time_t curTime);
	void MinuteTick(time_t curTime);

private:
	std::vector<IBaseManager*> m_List;
};

extern CManager& Manager();

// Base manager implmentation
template<class IInterface>
class CBaseManager : public IInterface
{
public:
	CBaseManager(const std::string& name, bool secondTick = false, bool minuteTick = false)
	{
		m_Name = name;
		m_bSecondTick = secondTick;
		m_bMinuteTick = minuteTick;

		Manager().AddManager(this);
	}

	virtual ~CBaseManager()
	{
		printf("~CBaseManager called, %p\n\n", this);
		Manager().RemoveManager(this);
	}

	// stub methods
	virtual bool Init() { return true; }
	virtual void Shutdown() { printf("Shutdown called, %p\n", this); }
	virtual std::string GetName() { return m_Name; }
	virtual void OnSecondTick(time_t curTime) {}
	virtual void OnMinuteTick(time_t curTime) {}
	virtual bool ShouldDoSecondTick() { return m_bSecondTick; }
	virtual bool ShouldDoMinuteTick() { return m_bMinuteTick; }

	void SetMinuteTick(bool tick) { m_bMinuteTick = tick; }
	void SetSecondTick(bool tick) { m_bSecondTick = tick; }

private:
	std::string m_Name;
	bool m_bSecondTick;
	bool m_bMinuteTick;
};
