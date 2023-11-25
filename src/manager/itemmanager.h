#pragma once

#include "interface/iitemmanager.h"
#include "manager/usermanager.h"
#include "definitions.h"
#include "csvtable.h"

#define ITEM_ADD_SUCCESS 1
#define ITEM_ADD_INVENTORY_FULL -1
#define ITEM_ADD_UNKNOWN_ITEMID -2
#define ITEM_ADD_DB_ERROR -3

#define ITEM_USE_SUCCESS 1
#define ITEM_USE_BAD_SLOT -1
#define ITEM_USE_WRONG_ITEM -2

class CItemManager : public CBaseManager<IItemManager>
{
public:
	CItemManager();
	~CItemManager();

	virtual bool Init();
	virtual void Shutdown();

	bool LoadRewards();
	bool KVToJson();
	bool OnItemPacket(CReceivePacket* msg, IExtendedSocket* socket);
	int AddItem(int userID, IUser* user, int itemId, int count, int duration);
	int AddItems(int userID, IUser* user, std::vector<RewardItem>& item);
	bool RemoveItem(int userID, IUser* user, CUserInventoryItem& item);
	int UseItem(IUser* user, int slot, int additionalArg = 0, int additionalArg2 = 0);
	bool CanUseItem(const CUserInventoryItem& item);
	bool OpenDecoder(IUser* user, int count, int slot);
	int ExtendItem(int userID, IUser* user, CUserInventoryItem& item, int newExpiryDate, bool duration = false);
	bool OnDisassembleRequest(IUser* user, CReceivePacket* msg);
	RewardNotice GiveReward(int userID, IUser* user, int rewardID, int rewardSelectID = 0, bool ignoreClient = false, int randomRepeatCount = 0);

	void OnUserLogin(IUser* user);
	void OnNicknameChangeUse(IUser* user, std::string newNickname);
	void OnRewardSelect(CReceivePacket* msg, IUser* user);
	void OnCostumeEquip(IUser* user, int slot);
	bool OnItemUse(IUser* user, CUserInventoryItem& item, int count = 1);

	Reward* GetRewardByID(int rewardID);
private:
	bool OnDailyRewardsRequest(IUser* user, int requestId);
	bool OnEnhancementRequest(IUser* user, CReceivePacket* msg);
	bool OnWeaponPaintRequest(IUser* user, CReceivePacket* msg);
	bool OnWeaponPaintSwitchRequest(IUser* user, CReceivePacket* msg);
	bool OnPartEquipRequest(IUser* user, CReceivePacket* msg);
	bool OnSwitchInUseRequest(IUser* user, CReceivePacket* msg);
	bool OnLockItemRequest(IUser* user, CReceivePacket* msg);

	// enhance funcs
	void InsertExp(IUser* user, CUserInventoryItem& targetItem, std::vector<CUserInventoryItem>& items);

	std::vector<Reward> m_Rewards;

	CCSVTable* m_pReinforceMaxLvTable;
	CCSVTable* m_pReinforceMaxExpTable;
};