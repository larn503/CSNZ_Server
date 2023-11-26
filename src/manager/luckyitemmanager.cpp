#include "luckyitemmanager.h"
#include "itemmanager.h"
#include "csvtable.h"
#include "packetmanager.h"
#include "userdatabase.h"

#include "nlohmann/json.hpp"
#include "keyvalues.hpp"

using namespace std;
using json = nlohmann::json;
using ordered_json = nlohmann::ordered_json;

#define ITEM_BOX_VERSION 1

CLuckyItemManager::CLuckyItemManager() : CBaseManager("LuckyItemManager")
{
}

CLuckyItemManager::~CLuckyItemManager()
{
	printf("~CLuckyItemManager\n");
	Shutdown();
}

bool CLuckyItemManager::Init()
{
	if (!KVToJson())
		LoadLuckyItems();

	return true;
}

void CLuckyItemManager::Shutdown()
{
	for (auto itemBox : m_ItemBoxes)
		delete itemBox;

	m_ItemBoxes.clear();
}

void CLuckyItemManager::LoadLuckyItems()
{
	try
	{
		ifstream f("ItemBox.json");
		ordered_json jItemBox = ordered_json::parse(f, nullptr, false, true);

		if (jItemBox.is_discarded())
		{
			g_pConsole->Error("CLuckyItemManager::LoadLuckyItems: couldn't load ItemBox.json.\n");
			return;
		}

		int version = jItemBox.value("Version", 0);
		if (version != ITEM_BOX_VERSION)
		{
			g_pConsole->Error("CLuckyItemManager::LoadLuckyItems: %d != ITEM_BOX_VERSION(%d)\n", version, ITEM_BOX_VERSION);
			return;
		}

		for (auto& iItem : jItemBox.items())
		{
			ordered_json jItem = iItem.value();
			if (!jItem.is_object())
				continue;

			ItemBox* itemBox = new ItemBox;
			itemBox->itemId = stoi(iItem.key());
			if (!jItem.contains("Rate"))
				continue;

			ordered_json jRates = jItem["Rate"];
			for (auto& iRate : jRates.items())
			{
				ordered_json jRate = iRate.value();

				ItemBoxRate rate = {};
				rate.rate = stof(iRate.key());
				rate.grade = jRate.value("Grade", 0);

				rate.duration = jRate.value("Duration", rate.duration);
				rate.items = jRate.value("Items", rate.items);

				for (auto i : rate.items)
				{
					ItemBoxItem item;
					item.itemBoxItemID = itemBox->itemId;
					item.itemId = i;

					m_Items.push_back(item);
				}

				itemBox->rates.push_back(rate);
			}

			m_ItemBoxes.push_back(itemBox);
		}
	}
	catch (exception& ex)
	{
		g_pConsole->Error("CLuckyItemManager::LoadLuckyItems: an error occured while parsing ItemBox.json: %s\n", ex.what());
	}

	// sort rate groups in descending order and check if sum of the all rate more than 1
	for (auto& itemBox : m_ItemBoxes)
	{
		int totalRate = 0;
		for (auto& rateGroup : itemBox->rates)
		{
			totalRate += rateGroup.rate;
			if (totalRate > 100)
			{
				g_pConsole->Warn("CLuckyItemManager::LoadLuckyItems: total rate for %d is more than 100\n", itemBox->itemId);
				break;
			}
		}

		sort(itemBox->rates.begin(), itemBox->rates.end(), [](const ItemBoxRate& a, const ItemBoxRate& b) { return a.rate > b.rate; });
	}
}

bool CLuckyItemManager::KVToJson()
{
	ofstream f("ItemBox.json", ios::in);
	if (f)
		return false;

	f.close();

	{
		KV::KeyValues root = KV::KeyValues::parseFromFile("ItemBox.txt");
		if (root.isEmpty())
			return false;
		
		KV::KeyValues& kvItemBox = root["ItemBox"];
		if (kvItemBox.isEmpty())
			return false;

		istringstream iss(string(""));
		for (auto& item : kvItemBox)
		{
			if (!item.isSection())
				continue;

			ItemBox *itemBox = new ItemBox;

			iss.clear();
			iss.str(item.getKey());
			iss >> itemBox->itemId;

			KV::KeyValues& kvRates = item["Rate"];
			if (kvRates.isEmpty())
				continue;

			for (auto& kvRate : kvRates)
			{
				if (!kvRate.isSection())
					return false;

				ItemBoxRate rate;

				rate.rate = stof(kvRate.getKey());
				rate.grade = kvRate["Grade"].getValueAsInt();

				KV::KeyValues& kvDuration = kvRate["Duration"];
				iss.clear();
				iss.str(kvDuration.getValue());
				vector<int> results((istream_iterator<int>(iss)), istream_iterator<int>());

				for (auto i : results)
				{
					rate.duration.push_back(i);
				}

				KV::KeyValues& kvItems = kvRate["Items"];
				iss.clear();
				iss.str(kvItems.getValue());
				results.clear();
				results.assign((istream_iterator<int>(iss)), istream_iterator<int>());

				for (auto i : results)
				{
					ItemBoxItem item;
					item.itemBoxItemID = itemBox->itemId;
					item.itemId = i;

					m_Items.push_back(item);

					rate.items.push_back(i);
				}

				itemBox->rates.push_back(rate);
			}

			m_ItemBoxes.push_back(itemBox);
		}
	}

	if (m_ItemBoxes.empty())
		return false;

	ordered_json cfg;
	cfg["Version"] = ITEM_BOX_VERSION;
	for (size_t i = 0; i < m_ItemBoxes.size(); i++)
	{
		ItemBox* item = m_ItemBoxes[i];
		for (size_t j = 0; j < item->rates.size(); j++)
		{
			ItemBoxRate rate = item->rates[j];

			cfg[to_string(item->itemId)]["Rate"][to_string(rate.rate)]["Grade"] = rate.grade;
			cfg[to_string(item->itemId)]["Rate"][to_string(rate.rate)]["Duration"] = rate.duration;
			cfg[to_string(item->itemId)]["Rate"][to_string(rate.rate)]["Items"] = rate.items;
		}
	}

	f.open("ItemBox.json");
	f << setfill('\t') << setw(1) << cfg << endl;

	return true;
}

ItemBox* CLuckyItemManager::GetItemBoxByItemId(int itemId)
{
	for (auto itemBox : m_ItemBoxes)
	{
		if (itemBox->itemId == itemId)
			return itemBox;
	}

	return NULL;
}

int CLuckyItemManager::OpenItemBox(IUser* user, int itemBoxID, int itemBoxOpenCount)
{
	ItemBox* itemBox = GetItemBoxByItemId(itemBoxID);
	if (!itemBox || itemBox->itemId == 0)
	{
		g_pConsole->Warn("User '%d, %s' tried to open item box with unknown ID: %d\n", user->GetID(), user->GetUsername().c_str(), itemBoxID);
		g_pPacketManager->SendItemOpenDecoderErrorReply(user->GetExtendedSocket(), ItemBoxError::FAIL_USEITEM);
		return 0;
	}

	CUserCharacter character = user->GetCharacter(UFLAG_PASSWORDBOXES | UFLAG_GAMENAME);
	if (!character.flag)
	{
		// TODO: send reply and rewrite condition
		printf("character.gameName.empty()!\n");
		return 0;
	}

	int openedDecoders = 0;
	ItemBoxOpenResult result;
	result.itemBoxItemId = itemBoxID;

	for (openedDecoders = 0; openedDecoders < itemBoxOpenCount; openedDecoders++)
	{
		if (g_pUserDatabase->IsInventoryFull(user->GetID()))
		{
			g_pPacketManager->SendItemOpenDecoderErrorReply(user->GetExtendedSocket(), ItemBoxError::FAIL_INVENTORY_FULL);
			break;
		}

		// generate random number from 1 to 100
		int rateNum = rand() % 100 + 1;
		int randRateIdx = -1;
		int tempRate = 0;

		// rates must be sorted in descending order!
		// go through rates and add to tempRate current rate value
		for (size_t i = 0; i < itemBox->rates.size(); i++)
		{
			tempRate += itemBox->rates[i].rate;

			// check if random number less or equal to tempRate
			if (rateNum <= tempRate)
			{
				// if yes, then you found your rate group
				randRateIdx = i;
				break;
			}
		}

		// looks like itembox config is wrong
		if (randRateIdx == -1)
		{
			g_pPacketManager->SendItemOpenDecoderErrorReply(user->GetExtendedSocket(), ItemBoxError::FAIL_USEITEM);
			return 0;
		}

		ItemBoxRate rate = itemBox->rates[randRateIdx];
		
		// get random item and duration from rate group (probability is the same for each item)
		Randomer randomItem{ rate.items.size() - 1 };
		Randomer randomDuration{ rate.duration.size() - 1 };
		int itemID = rate.items[randomItem()];
		int duration = rate.duration[randomDuration()];

		int status = g_pItemManager->AddItem(user->GetID(), user, itemID, 1, duration);
		if (status < 0)
		{
			g_pPacketManager->SendItemOpenDecoderErrorReply(user->GetExtendedSocket(), ItemBoxError::FAIL_USEITEM);
			break;
		}

		ItemBoxItem item;
		item.itemId = itemID;
		item.grade = rate.grade;
		item.duration = duration;
		result.items.push_back(item);

		// send notification in lobby chat to all users
		if (item.grade == ItemBoxGrades::PREMIUM || item.grade == ItemBoxGrades::ADVANCED)
		{
			// TODO: make method in manager for this
			for (auto u : g_pUserManager->GetUsers())
			{
				g_pPacketManager->SendUMsgSystemReply(u->GetExtendedSocket(), 2, item.grade == ItemBoxGrades::PREMIUM ? "LOTTERY_WIN_PREMIUM" : "LOTTERY_WIN_PREMIUM_NUM", vector<string>{ character.gameName, to_string(item.itemId) });
			}
		}
	}

	if (result.items.size())
		g_pPacketManager->SendItemOpenDecoderResult(user->GetExtendedSocket(), result);

	return openedDecoders;
}

vector<ItemBox*>& CLuckyItemManager::GetItemBoxes()
{
	return m_ItemBoxes;
}

vector<ItemBoxItem>& CLuckyItemManager::GetItems()
{
	return m_Items;
}