#include "Manager.h"

CManager& Manager()
{
	static CManager x;
	return x;
}

CManagerBase::CManagerBase()
{
	Manager().AddElem(this);
}

void CManager::Init()
{
	// init
	for (CManagerBase& p : m_List)
	{
		p.Init();
	}
}