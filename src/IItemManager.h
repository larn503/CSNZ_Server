#pragma once

class IItemManager : public IBaseManager
{
public:
	virtual void LoadRewards() = 0;
	virtual bool KVToJson() = 0;
	virtual bool OnItemPacket(CReceivePacket* msg, CExtendedSocket* socket) = 0;
	virtual int AddItem(int userID, CUser* user, int itemId, int count, int duration) = 0;
	virtual int AddItems(int userID, CUser* user, std::vector<RewardItem>& item) = 0;
	virtual bool RemoveItem(int userID, CUser* user, CUserInventoryItem& item) = 0;
	virtual int UseItem(CUser* user, int slot, int additionalArg = 0, int additionalArg2 = 0) = 0;
	virtual bool CanUseItem(const CUserInventoryItem& item) = 0;
	virtual bool OpenDecoder(CUser* user, int count, int slot) = 0;
	virtual int ExtendItem(int userID, CUser* user, CUserInventoryItem& item, int newExpiryDate, bool duration = false) = 0;
	virtual bool OnDisassembleRequest(CUser* user, CReceivePacket* msg) = 0;
	virtual RewardNotice GiveReward(int userID, CUser* user, int rewardID, int rewardSelectID = 0, bool ignoreClient = false, int randomRepeatCount = 0) = 0;

	virtual void OnUserLogin(CUser* user) = 0;
	virtual void OnNicknameChangeUse(CUser* user, std::string newNickname) = 0;
	virtual void OnRewardSelect(CReceivePacket* msg, CUser* user) = 0;
	virtual void OnCostumeEquip(CUser* user, int slot) = 0;
	virtual bool OnItemUse(CUser* user, CUserInventoryItem& item, int count = 1) = 0;

	virtual Reward* GetRewardByID(int rewardID) = 0;
};