#pragma once

#include "main.h"

class CUserFastBuy
{
public:
	CUserFastBuy()
	{
	}

	CUserFastBuy(std::string name, std::vector<int> items)
	{
		m_Name = name;
		m_Items = items;
	}

	std::string m_Name;
	std::vector<int> m_Items;
};