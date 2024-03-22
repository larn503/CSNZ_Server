#include "manager.h"

using namespace std;

// singleton
CManager& Manager()
{
	return CManager::GetInstance();
}

CManager& CManager::GetInstance()
{
	static CManager mgr;
	return mgr;
}

bool CManager::InitAll()
{
	for (auto p : m_Managers)
	{
		if (!p->Init())
			return false;
	}

	return true;
}

void CManager::ShutdownAll()
{
	for (auto p : m_Managers)
	{
		p->Shutdown();
	}
}

void CManager::AddManager(IBaseManager* pElem)
{
	// check for duplicate by manager name
	/// @fixme: should duplicate pointers be checked?
	for (auto p : m_Managers)
	{
		if (p->GetName() == pElem->GetName())
		{
			Console().Error("CManager::AddManager: %s duplicate!!", pElem->GetName());
			return;
		}
	}

	m_Managers.push_back(pElem);
}

void CManager::RemoveManager(IBaseManager* pElem)
{
	m_Managers.erase(remove(m_Managers.begin(), m_Managers.end(), pElem), m_Managers.end());
}

IBaseManager* CManager::GetManager(const string& name)
{
	for (auto p : m_Managers)
	{
		if (p->GetName() == name)
			return p;
	}

	return NULL;
}

void CManager::SecondTick(time_t curTime)
{
	for (auto p : m_Managers)
	{
		if (p->ShouldDoSecondTick())
			p->OnSecondTick(curTime);
	}
}

void CManager::MinuteTick(time_t curTime)
{
	for (auto p : m_Managers)
	{
		if (p->ShouldDoMinuteTick())
			p->OnMinuteTick(curTime);
	}
}