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
class CBaseManager : public IBaseManager
{
public:
	CBaseManager(bool secondTick = false, bool minuteTick = false);
	virtual ~CBaseManager();

	// stub methods
	virtual bool Init();
	virtual void Shutdown();
	virtual void OnSecondTick(time_t curTime);
	virtual void OnMinuteTick(time_t curTime);

	virtual bool ShouldDoSecondTick();
	virtual bool ShouldDoMinuteTick();

	void SetSecondTick(bool tick);
	void SetMinuteTick(bool tick);

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
