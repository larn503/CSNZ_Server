#pragma once

#include "imanager.h"

class IUser;
struct ItemBox;
struct ItemBoxItem;

class ILuckyItemManager : public IBaseManager
{
public:
	virtual void LoadLuckyItems() = 0;
	virtual bool KVToJson() = 0; // TODO: delete sometime 
	virtual int OpenItemBox(IUser* user, int itemBox, int itemBoxOpenCount) = 0;
	virtual std::vector<ItemBox*>& GetItemBoxes() = 0;
	virtual std::vector<ItemBoxItem>& GetItems() = 0;
	virtual ItemBox* GetItemBoxByItemId(int itemId) = 0;
};
