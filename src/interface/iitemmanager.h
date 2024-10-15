#pragma once

#include "imanager.h"

class IExtendedSocket;
class IUser;
class CUserInventoryItem;
struct RewardItem;
struct RewardNotice;
struct Reward;
struct WeaponPaint;

class IItemManager : public IBaseManager
{
public:
	virtual bool LoadRewards() = 0;
	virtual bool LoadWeaponPaints() = 0;
	virtual bool KVToJson() = 0;
	virtual bool OnItemPacket(CReceivePacket* msg, IExtendedSocket* socket) = 0;
	virtual int AddItem(int userID, IUser* user, int itemId, int count, int duration, int lockStatus = 0) = 0;
	virtual int AddItems(int userID, IUser* user, std::vector<RewardItem>& item) = 0;
	virtual bool RemoveItem(int userID, IUser* user, CUserInventoryItem& item) = 0;
	virtual int UseItem(IUser* user, int slot, int additionalArg = 0, int additionalArg2 = 0) = 0;
	virtual bool CanUseItem(const CUserInventoryItem& item) = 0;
	virtual bool OpenDecoder(IUser* user, int count, int slot) = 0;
	virtual int ExtendItem(int userID, IUser* user, CUserInventoryItem& item, int newExpiryDate, bool duration = false) = 0;
	virtual bool OnDisassembleRequest(IUser* user, CReceivePacket* msg) = 0;
	virtual RewardNotice GiveReward(int userID, IUser* user, int rewardID, int rewardSelectID = 0, bool ignoreClient = false, int randomRepeatCount = 0) = 0;

	virtual void OnUserLogin(IUser* user) = 0;
	virtual void OnNicknameChangeUse(IUser* user, std::string newNickname) = 0;
	virtual void OnRewardSelect(CReceivePacket* msg, IUser* user) = 0;
	virtual void OnCostumeEquip(IUser* user, int slot) = 0;
	virtual bool OnItemUse(IUser* user, CUserInventoryItem& item, int count = 1) = 0;

	virtual Reward* GetRewardByID(int rewardID) = 0;
	virtual std::vector<WeaponPaint> GetWeaponPaints() = 0;
};