#pragma once

#include "UserManager.h"

struct ItemBoxRate
{
	float rate;
	int grade;
	std::vector<int> duration;
	std::vector<int> items;
};

struct ItemBox
{
	int itemId;
	std::vector<ItemBoxRate> rates;
};

struct ItemBoxItem
{
	int itemBoxItemID;
	int itemId;
	int count;
	int duration;
	int grade;
};

struct ItemBoxOpenResult
{
	int itemBoxItemId;
	std::vector<ItemBoxItem> items;
};

enum ItemBoxError
{
	FAIL_INVENTORY_FULL = 1,
	FAIL_USEITEM = 2,
	FAIL_PAUSED = 3,
	NOT_KEY = 4,
};

enum ItemBoxGrades
{
	DEFAULT = 1,
	NORMAL = 2,
	ADVANCED = 3,
	PREMIUM = 4,
};

class CLuckyItemManager
{
public:
	CLuckyItemManager();
	~CLuckyItemManager();

	void Init();
	void Shutdown();
	void LoadLuckyItems();
	bool KVToJson();
	float GetItemDropChance(int grade);
	int OpenItemBox(CUser* user, int itemBox, int itemBoxOpenCount);
	std::vector<ItemBox*>& GetItemBoxes();
	std::vector<ItemBoxItem>& GetItems();

private:
	ItemBox* GetItemBoxByItemId(int itemId);

	std::vector<ItemBoxItem> m_Items;
	std::vector<ItemBox*> m_ItemBoxes;
};
