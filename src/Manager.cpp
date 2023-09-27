#include "Manager.h"

CManager& Manager()
{
	static CManager x;
	return x;
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