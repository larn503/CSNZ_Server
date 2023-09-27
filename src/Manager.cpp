#include "Manager.h"

CManager& Manager()
{
	static CManager x;
	return x;
}

CBaseManager::CBaseManager(bool secondTick, bool minuteTick)
{
	m_bSecondTick = secondTick;
	m_bMinuteTick = minuteTick;

	Manager().AddManager(this);
}

CBaseManager::~CBaseManager()
{
	printf("~CBaseManager called, 0x%p\n\n", this);
	Manager().RemoveManager(this);
}

bool CBaseManager::Init()
{
	return true;
}

void CBaseManager::Shutdown()
{
	printf("Shutdown called, 0x%p\n", this);
}

void CBaseManager::OnSecondTick(time_t curTime)
{
}

void CBaseManager::OnMinuteTick(time_t curTime)
{
}

bool CBaseManager::ShouldDoSecondTick()
{
	return m_bSecondTick;
}

bool CBaseManager::ShouldDoMinuteTick()
{
	return m_bMinuteTick;
}

void CBaseManager::SetMinuteTick(bool tick)
{
	m_bMinuteTick = tick;
}

void CBaseManager::SetSecondTick(bool tick)
{
	m_bSecondTick = tick;
}

bool CManager::InitAll()
{
	for (auto p : m_List)
	{
		printf("InitAll: 0x%p\n", p);
		if (!p->Init())
			return false;
	}

	return true;
}

void CManager::ShutdownAll()
{
	for (auto p : m_List)
	{
		p->Shutdown();
	}
}

void CManager::AddManager(IBaseManager* pElem)
{
	m_List.push_back(pElem);
}

void CManager::RemoveManager(IBaseManager* pElem)
{
	m_List.erase(std::remove(m_List.begin(), m_List.end(), pElem), m_List.end());
}

void CManager::SecondTick(time_t curTime)
{
	for (auto p : m_List)
	{
		if (p->ShouldDoSecondTick())
			p->OnSecondTick(curTime);
	}
}

void CManager::MinuteTick(time_t curTime)
{
	for (auto p : m_List)
	{
		if (p->ShouldDoMinuteTick())
			p->OnMinuteTick(curTime);
	}
}