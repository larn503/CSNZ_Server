#ifndef Manager_H
#define Manager_H

#include "main.h"

// load config method

class CManagerBase
{
public:
	int m_type;

	CManagerBase();
	virtual ~CManagerBase() {}
	virtual void Init() { return; }
	virtual void LoadConfig() { return; }

	CManagerBase(const CManagerBase& other) = delete;
	CManagerBase(CManagerBase&& other) = delete;
	bool operator<(const CManagerBase& other) const // for sorting
	{
		return m_type < other.m_type;
	}
};

class CManager
{
private:
	std::vector <std::reference_wrapper<CManagerBase>> m_List;

public:
	void Init();
	inline void AddElem(CManagerBase* pElem) { m_List.push_back(*pElem); }
};

extern CManager& Manager();

#endif