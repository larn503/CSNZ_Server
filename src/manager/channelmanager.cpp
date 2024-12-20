#include "channelmanager.h"

#include "itemmanager.h"
#include "packetmanager.h"
#include "minigamemanager.h"
#include "userdatabase.h"
#include "serverinstance.h"
#include "dedicatedservermanager.h"

#include "user/userinventoryitem.h"

#include "csvtable.h"
#include "serverconfig.h"
#include "common/buildnum.h"
#include "common/utils.h"

#include <algorithm>

using namespace std;

CChannelManager g_ChannelManager;

// TODO: Should we implement multi channels?
CChannelManager::CChannelManager() : CBaseManager("ChannelManager")
{
}

CChannelManager::~CChannelManager()
{
}

bool CChannelManager::Init()
{
	if (channelServers.empty())
		channelServers.push_back(new CChannelServer("Channel server", 1, 1, 1));

	return true;
}

void CChannelManager::Shutdown()
{
	CBaseManager::Shutdown();

	EndAllGames();

	for (auto& cs : channelServers) {
		for (auto& c : cs->GetChannels()) {
			c->Shutdown();
		}
	}
}

bool CChannelManager::OnChannelListPacket(IExtendedSocket* socket)
{
	LOG_PACKET;

	IUser* user = g_UserManager.GetUserBySocket(socket);
	if (user == NULL)
	{
		g_PacketManager.SendServerList(socket);

		return false;
	}

	if (user->GetCurrentRoom())
	{
		user->GetCurrentRoom()->RemoveUser(user);
	}

	CChannel* channel = user->GetCurrentChannel();
	if (channel)
	{
		channel->UserLeft(user);
		user->SetCurrentChannel(NULL);

		Logger().Info("User '%s' left channel\n", user->GetLogName());
	}

	Logger().Info("User '%s' requested server list, sending...\n", user->GetLogName());

	g_PacketManager.SendServerList(socket);

	return true;
}

bool CChannelManager::OnRoomRequest(CReceivePacket* msg, IExtendedSocket* socket)
{
	LOG_PACKET;

	IUser* user = g_UserManager.GetUserBySocket(socket);
	if (user == NULL)
		return false;

	int type = msg->ReadUInt8();
	switch (type)
	{
	case InRoomType::NewRoomRequest:
		return OnNewRoomRequest(msg, user);
	case InRoomType::GameStartRequest:
		return OnGameStartRequest(user);
	case InRoomType::LeaveRoomRequest:
		return OnLeaveRoomRequest(user);
	case InRoomType::ToggleReadyRequest:
		return OnToggleReadyRequest(user);
	case InRoomType::OnConnectionFailure:
		return OnConnectionFailure(user);
	case InRoomType::UserInviteListRequest:
		g_PacketManager.SendRoomInviteUserList(socket, user);
		break;
	case InRoomType::JoinRoomRequest:
		return OnJoinRoomRequest(msg, user);
	case InRoomType::RequestUpdateSettings:
		return OnRoomUpdateSettings(msg, user);
	case InRoomType::OnCloseResultWindow:
		return OnCloseResultRequest(user);
	case InRoomType::SetUserTeamRequest:
		return OnSetTeamRequest(msg, user);
	case InRoomType::UserInviteRequest:
		return OnUserInviteRequest(msg, user);
	case InRoomType::RoomListRequest:
		g_PacketManager.SendRoomListFull(socket, channelServers[0]->GetChannels()[0]->GetRooms());
		break;
	case 27:
		break;
	case InRoomType::SetZBAddonsRequest:
		return OnRoomSetZBAddonRequest(msg, user);
	case InRoomType::KickRequest:
		return OnRoomKickRequest(msg, user);
	case InRoomType::KickClanRequest:
		return OnRoomKickClanRequest(msg, user);
	case InRoomType::NoticeClanRequest:
		return OnRoomNoticeClanRequest(msg, user);
	case 32:
		g_PacketManager.SendRoomUnk32(socket);
		break;
	case 33:
		g_PacketManager.SendRoomUnk33(socket);
		break;
	case InRoomType::VoxelRoomListRequest:
		return OnVoxelRoomListRequest(msg, user);
	default:
		Logger().Warn("Unknown room request %d\n", type);
		break;
	}

	return true;
}

bool CChannelManager::OnRoomListPacket(CReceivePacket* msg, IExtendedSocket* socket)
{
	LOG_PACKET;

	IUser* user = g_UserManager.GetUserBySocket(socket);
	if (!user)
	{
		return false;
	}

	int channelServerIndex = msg->ReadUInt8();
	int channelIndex = msg->ReadUInt8();

	JoinChannel(user, channelServerIndex, channelIndex, true);

	return true;
}

void CChannelManager::JoinChannel(IUser* user, int channelServerID, int channelID, bool transfer)
{
	CChannelServer* channelServer = GetServerByIndex(channelServerID);
	CChannel* channel = NULL;

	if (channelServer)
	{
		channel = channelServer->GetChannelByIndex(channelID);
	}

	if (channelServer == NULL || channel == NULL)
	{
		return;
	}

	if (user->GetCurrentChannel() == NULL)
	{
		/*if (transfer && user->GetLastChannelServer() != channelServer && !channelServer->ip.empty())
		{
			g_PacketManager.SendTransfer(user->GetExtendedSocket(), channelServer->ip, channelServer->port, user->GetUsername());
			g_UserDatabase.AddToRestoreList(user->GetID(), channelServer->index, channel->m_nIndex);
			return;
		}*/

		Logger().Info("User '%s' joined channel '%s'\n", user->GetLogName(), channel->GetName().c_str());

		user->SetCurrentChannel(channel);
		user->SetLastChannelServer(channelServer);

		channel->UserJoin(user);
	}
	else
	{
		//char error[256];
		//sprintf(error, "If you see this message, pls write to dev this: '0x%X (0x2)'", user->m_pCurrentChannel);
		//g_UserManager.SendNoticeMsgBoxToUuid(socket, error);
	}

	Logger().Info("User '%s' requested room list successfully, sending...\n", user->GetLogName());

	g_PacketManager.SendRoomListFull(user->GetExtendedSocket(), channel->GetRooms());
}

void CChannelManager::EndAllGames()
{
	for (auto channelServer : channelServers)
	{
		if (!channelServer)
			continue;

		for (auto channel : channelServer->GetChannels())
		{
			if (!channel)
				continue;

			for (auto room : channel->GetRooms())
			{
				if (!room)
					continue;

				if (room->GetGameMatch())
				{
					Logger().Info(OBFUSCATE("Force ending RoomID %d\n"), room->GetID());
					room->EndGame(true);
				}
			}
		}
	}
}

bool CChannelManager::OnLobbyMessage(CReceivePacket* msg, IExtendedSocket* socket, IUser* user)
{
	string message = msg->ReadString();

	string gameName = OBFUSCATE("Not logged in");
	if (user)
	{
		CUserCharacter character = user->GetCharacter(UFLAG_LOW_GAMENAME);
		gameName = character.gameName;
	}

	if (!OnCommandHandler(socket, user, message) && user)
	{
		CChannel* channel = user->GetCurrentChannel();
		if (!channel)
		{
			Logger().Info(OBFUSCATE("User '%s' tried to send message but he's not in channel\n"), user->GetLogName());
			return false;
		}

		channel->SendUserMessageToAllUser(UMsgPacketType::LobbyUserMessage, user->GetID(), gameName, message);
	}

	return true;
}

bool CChannelManager::OnWhisperMessage(CReceivePacket* msg, IUser* userSender)
{
	if (userSender == NULL)
		return false;

	IExtendedSocket* socket = userSender->GetExtendedSocket();

	string userNameDest = msg->ReadString();
	string message = msg->ReadString();

	IUser* userDest = g_UserManager.GetUserByNickname(userNameDest);
	if (!userDest)
	{
		// send no user reply
		g_PacketManager.SendUMsgSystemReply(socket, UMsgPacketType::SystemReply_Red, "MSG_TELL_USER_NOT_FOUND", vector<string>{ userNameDest });
	}
	else if (userDest->GetExtendedSocket() == socket)
	{
		g_PacketManager.SendUMsgNoticeMessageInChat(socket, "You can't send whisper to yourself.");
		// you can't send whisper message to yourself
	}
	else
	{
		CUserCharacterExtended characterExtendedSender = userSender->GetCharacterExtended(EXT_UFLAG_BANSETTINGS);

		if (characterExtendedSender.banSettings & 4)
		{
			// you can't send whisper message if you're blocking all whisper
			g_PacketManager.SendUMsgSystemReply(socket, UMsgPacketType::SystemReply_Red, "MSG_TELL_SENDER_USING_BAN_CHAT_ALL");
		}
		else
		{
			CUserCharacterExtended characterExtendedDest = userDest->GetCharacterExtended(EXT_UFLAG_BANSETTINGS);

			if (characterExtendedDest.banSettings & 4)
			{
				// you can't send whisper message if dest is blocking all whisper
				g_PacketManager.SendUMsgSystemReply(socket, UMsgPacketType::SystemReply_Red, "MSG_TELL_LISTENER_USING_BAN_CHAT_ALL");
			}
			// you can't send whisper message if you're blocking the dest's chat/whisper
			else if (~characterExtendedSender.banSettings & 2 || characterExtendedSender.banSettings & 2 && !g_UserDatabase.IsInBanList(userSender->GetID(), userDest->GetID()))
			{
				g_PacketManager.SendUMsgUserMessage(socket, UMsgPacketType::WhisperUserMessage, userNameDest, message, UMsgWhisperType::To);
				CUserCharacter character = userSender->GetCharacter(UFLAG_LOW_GAMENAME);
				g_PacketManager.SendUMsgUserMessage(userDest->GetExtendedSocket(), UMsgPacketType::WhisperUserMessage, character.gameName, message, UMsgWhisperType::From);
			}
		}
	}

	return true;
}

bool CChannelManager::OnRoomUserMessage(CReceivePacket* msg, IUser* user)
{
	if (user == NULL || user->GetCurrentRoom() == NULL)
		return false;

	user->GetCurrentRoom()->OnUserMessage(msg, user);

	return true;
}

bool CChannelManager::OnRoomTeamUserMessage(CReceivePacket* msg, IUser* user)
{
	if (user == NULL || user->GetCurrentRoom() == NULL)
		return false;

	user->GetCurrentRoom()->OnUserTeamMessage(msg, user);

	return true;
}

bool CChannelManager::OnServerYellMessage(CReceivePacket* msg, IUser* user)
{
	if (user == NULL)
		return false;

	CChannel* channel = user->GetCurrentChannel();
	if (!channel)
	{
		Logger().Info(OBFUSCATE("User '%s' tried to send message but he's not in channel\n"), user->GetLogName());
		return false;
	}

	vector<CUserInventoryItem> items;
	if (!g_UserDatabase.GetInventoryItemsByID(user->GetID(), 204 /*server yell*/, items))
		return false;
	
	g_ItemManager.OnItemUse(user, items[0]);

	CUserCharacter character = user->GetCharacter(UFLAG_LOW_GAMENAME);
	string message = msg->ReadString();

	channel->SendUserMessageToAllUser(UMsgPacketType::ServerYellUserMessage, user->GetID(), character.gameName, message);

	return true;
}

CChannelServer* CChannelManager::GetServerByIndex(int index)
{
	for (auto server : this->channelServers)
	{
		if (server->GetID() == index)
			return server;
	}

	return NULL;
}

bool CChannelManager::OnCommandHandler(IExtendedSocket* socket, IUser* user, const string& message)
{
	vector<string> args = ParseArguments(message);
	if (args.size() == 0 || args[0][0] != '/') // all lobby commands starts with '/' character
	{
		return false;
	}

	if (!user)
	{
		if (args[0] == (char*)OBFUSCATE("/login"))
		{
			if (args.size() == 3)
			{
				string login = args[1];
				string password = args[2];

				int loginResult = g_UserManager.LoginUser(socket, login, password);
				switch (loginResult)
				{
				case LOGIN_DB_ERROR:
					g_PacketManager.SendUMsgNoticeMsgBoxToUuid(socket, OBFUSCATE("DB_QUERY_FAILED"));
					break;
				case LOGIN_NO_SUCH_USER:
					g_PacketManager.SendUMsgNoticeMsgBoxToUuid(socket, OBFUSCATE("Wrong password or username."));
					break;
				case LOGIN_USER_BANNED:
					g_pServerInstance->DisconnectClient(socket);
					break;
				case LOGIN_SERVER_CANNOT_VALIDATE_CLIENT:
					g_PacketManager.SendUMsgNoticeMsgBoxToUuid(socket, OBFUSCATE("Failed to validate client. Contact administrator and try to reinstall the game."));
					break;
				}
			}
			else
			{
				g_PacketManager.SendUMsgNoticeMessageInChat(socket, OBFUSCATE("/login arguments: <username> <password>"));
			}
		}
		else if (args[0] == (char*)OBFUSCATE("/register"))
		{
			if (args.size() == 3)
			{
				string login = args[1];
				string password = args[2];

				int regResult = g_UserManager.RegisterUser(socket, login, password);
				switch (regResult)
				{
				case REGISTER_DB_ERROR:
					g_PacketManager.SendUMsgNoticeMsgBoxToUuid(socket, OBFUSCATE("DB_QUERY_FAILED"));
					break;
				case REGISTER_OK:
					g_PacketManager.SendUMsgNoticeMsgBoxToUuid(socket, OBFUSCATE("You have successfully registered."));
					break;
				case REGISTER_USERNAME_EXIST:
					g_PacketManager.SendUMsgNoticeMsgBoxToUuid(socket, OBFUSCATE("User with this username already exists."));
					break;
				case REGISTER_USERNAME_WRONG:
					g_PacketManager.SendUMsgNoticeMsgBoxToUuid(socket, OBFUSCATE("Username must contain at least 5 characters and not more than 15, English letters."));
					break;
				case REGISTER_PASSWORD_WRONG:
					g_PacketManager.SendUMsgNoticeMsgBoxToUuid(socket, OBFUSCATE("Password must contain at least 5 characters and not more than 15, not only numbers"));
					break;
				case REGISTER_IP_LIMIT:
					g_PacketManager.SendUMsgNoticeMsgBoxToUuid(socket, OBFUSCATE("You have exceeded the account limit for one IP"));
					break;
				}
			}
			else
			{
				g_PacketManager.SendUMsgNoticeMessageInChat(socket, OBFUSCATE("/register arguments: <username> <password>"));
			}
		}
	}
	else
	{
		CUserCharacterExtended characterExtended = user->GetCharacterExtended(EXT_UFLAG_GAMEMASTER);
		if (characterExtended.gameMaster)
		{
			if (args[0] == (char*)OBFUSCATE("/version"))
			{
				char buf[128];
				snprintf(buf, sizeof(buf), OBFUSCATE("Server build: %s"), build_number());
				g_PacketManager.SendUMsgNoticeMessageInChat(socket, buf);
				return true;
			}
			else if (args[0] == (char*)OBFUSCATE("/help"))
			{
				g_PacketManager.SendUMsgNoticeMessageInChat(socket, OBFUSCATE("Available commands: /help, /version, /disconnect, /getfreeslots, /additem, /giveitem, /addallitems"));
				g_PacketManager.SendUMsgNoticeMessageInChat(socket, OBFUSCATE("/removeitem, /sendnotice, /addexp, /status, /ban, /hban, /ipban"));
				g_PacketManager.SendUMsgNoticeMessageInChat(socket, OBFUSCATE("/unban, /getuid, /tournament, /givereward, /giverewardtoall, /addwpnreleasechar, /addpoints"));
				g_PacketManager.SendUMsgNoticeMessageInChat(socket, OBFUSCATE("/givepoints, /setitemstatus, /setiteminuse, /sendgmnotice, /weaponrelease, /bingo"));
				g_PacketManager.SendUMsgNoticeMessageInChat(socket, OBFUSCATE("/setprefix"));
				return true;
			}
			else if (args[0] == (char*)OBFUSCATE("/disconnect"))
			{
				g_UserManager.DisconnectUser(user);
				return true;
			}
			else if (args[0] == (char*)OBFUSCATE("/getfreeslots"))
			{
				int inventoryItemsCount = g_UserDatabase.GetInventoryItemsCount(user->GetID());

				if (inventoryItemsCount < 0)
					g_PacketManager.SendUMsgNoticeMessageInChat(socket, va("Couldn't get free slots due to database error."));
				else if (g_pServerConfig->inventorySlotMax - inventoryItemsCount > 0)
					g_PacketManager.SendUMsgNoticeMessageInChat(socket, va("You have %d free slots.", g_pServerConfig->inventorySlotMax - inventoryItemsCount));
				else
					g_PacketManager.SendUMsgNoticeMessageInChat(socket, va("You have no free slots."));

				return true;
			}
			else if (args[0] == (char*)OBFUSCATE("/additem"))
			{
				int itemID, count = 1, duration = 0;

				if (!(args.size() >= 2))
				{
					g_PacketManager.SendUMsgNoticeMessageInChat(socket, OBFUSCATE("/additem usage: /additem <itemID> <count> <duration>"));
					return true;
				}

				if (!isNumber(args[1]))
				{
					g_PacketManager.SendUMsgNoticeMessageInChat(socket, OBFUSCATE("/additem usage: /additem <itemID> <count> <duration>"));
					return true;
				}

				istringstream iss(args[1]);
				iss >> itemID;

				if (iss.fail())
					return true;

				if (args.size() >= 3 && isNumber(args[2]))
				{
					istringstream iss2(args[2]);
					iss2 >> count;

					if (iss2.fail())
						return true;
				}

				if (args.size() >= 4 && isNumber(args[3]))
				{
					iss.clear();
					iss.str(args[3]);
					iss >> duration;

					if (iss.fail())
						return true;
				}

				int status = g_ItemManager.AddItem(user->GetID(), user, itemID, count, duration); // add permanent item by default
				switch (status)
				{
				case ITEM_ADD_INVENTORY_FULL:
					g_PacketManager.SendUMsgNoticeMsgBoxToUuid(socket, OBFUSCATE("Your inventory is full"));
					break;
				case ITEM_ADD_UNKNOWN_ITEMID:
					g_PacketManager.SendUMsgNoticeMsgBoxToUuid(socket, OBFUSCATE("Item ID you wrote does not exist in the item database"));
					break;
				case ITEM_ADD_DB_ERROR:
					g_PacketManager.SendUMsgNoticeMsgBoxToUuid(socket, OBFUSCATE("Database error"));
					break;
				case ITEM_ADD_SUCCESS:
					// send notification about new item
					RewardItem rewardItem;
					rewardItem.itemID = itemID;
					rewardItem.count = count;
					rewardItem.duration = duration;

					RewardNotice rewardNotice;
					rewardNotice.rewardId = 1;
					rewardNotice.exp = 0;
					rewardNotice.points = 0;
					rewardNotice.honorPoints = 0;
					rewardNotice.items.push_back(rewardItem);
					g_PacketManager.SendUMsgRewardNotice(socket, rewardNotice, "QUEST_REWARD_TITLE", "QUEST_REWARD_MSG", true);

					break;
				}

				return true;
			}
			else if (args[0] == (char*)OBFUSCATE("/giveitem"))
			{
				if (!(args.size() >= 3))
				{
					g_PacketManager.SendUMsgNoticeMessageInChat(socket, OBFUSCATE("/giveitem usage: /giveitem <gameName/userID> <itemID> <count> <duration>"));
					return true;
				}

				int itemID, userID, count = 1, duration = 0;

				if (isNumber(args[1]))
				{
					istringstream iss(args[1]);
					iss >> userID;

					if (iss.fail())
						return true;

					if (!g_UserDatabase.IsUserExists(userID))
						userID = 0;
				}
				else
				{
					userID = g_UserDatabase.IsUserExists(args[1], false);
				}

				if (!userID)
				{
					g_PacketManager.SendUMsgNoticeMessageInChat(socket, OBFUSCATE("User not found."));
					return true;
				}

				istringstream iss(args[2]);
				iss >> itemID;

				if (iss.fail())
					return true;

				if (args.size() >= 4 && isNumber(args[3]))
				{
					istringstream iss2(args[3]);
					iss2 >> count;

					if (iss2.fail())
						return true;
				}

				if (args.size() >= 5 && isNumber(args[4]))
				{
					iss.clear();
					iss.str(args[4]);
					iss >> duration;

					if (iss.fail())
						return true;
				}

				IUser* user = g_UserManager.GetUserById(userID);
				int status = g_ItemManager.AddItem(userID, user, itemID, count, duration); // add permanent item by default
				switch (status)
				{
				case ITEM_ADD_INVENTORY_FULL:
					g_PacketManager.SendUMsgNoticeMsgBoxToUuid(socket, OBFUSCATE("User's inventory is full"));
					break;
				case ITEM_ADD_UNKNOWN_ITEMID:
					g_PacketManager.SendUMsgNoticeMsgBoxToUuid(socket, OBFUSCATE("Item ID you wrote does not exist in the item database"));
					break;
				case ITEM_ADD_DB_ERROR:
					g_PacketManager.SendUMsgNoticeMsgBoxToUuid(socket, OBFUSCATE("Database error"));
					break;
				case ITEM_ADD_SUCCESS:
				{
					// send notification about new item
					RewardItem rewardItem;
					rewardItem.itemID = itemID;
					rewardItem.count = count;
					rewardItem.duration = duration;

					RewardNotice rewardNotice;
					rewardNotice.rewardId = 1;
					rewardNotice.exp = 0;
					rewardNotice.points = 0;
					rewardNotice.honorPoints = 0;
					rewardNotice.items.push_back(rewardItem);

					if (!user)
					{
						// TODO: should we use custom rewards here?
					}
					else
					{
						g_PacketManager.SendUMsgRewardNotice(user->GetExtendedSocket(), rewardNotice, "QUEST_REWARD_TITLE", "QUEST_REWARD_MSG", true);

						if (user->IsPlaying())
							g_PacketManager.SendUMsgRewardNotice(user->GetExtendedSocket(), rewardNotice, "QUEST_REWARD_TITLE", "QUEST_REWARD_MSG", true, true);
					}

					break;
				}
				}

				return true;
			}
			else if (args[0] == (char*)OBFUSCATE("/addallitems"))
			{
				vector<string> items = g_pItemTable->GetRowNames();
				vector<RewardItem> rewardItems;
				CUserInventoryItem userItem;
				int itemID = 0;
				for (auto item : items)
				{
					itemID = stoi(item);
					if (userItem.IsItemDefaultOrPseudo(itemID))
						continue;

					RewardItem rewardItem;
					rewardItem.itemID = itemID;
					rewardItem.count = 1;
					rewardItem.duration = 0;
					rewardItem.lockStatus = 0;
					rewardItems.push_back(rewardItem);
				}

				int status = g_ItemManager.AddItems(user->GetID(), user, rewardItems);
				switch (status)
				{
				case ITEM_ADD_SUCCESS:
					g_PacketManager.SendUMsgNoticeMsgBoxToUuid(socket, OBFUSCATE("Done"));
					break;
				case ITEM_ADD_INVENTORY_FULL:
					g_PacketManager.SendUMsgNoticeMsgBoxToUuid(socket, OBFUSCATE("Your inventory is full"));
					break;
				case ITEM_ADD_DB_ERROR:
					g_PacketManager.SendUMsgNoticeMsgBoxToUuid(socket, OBFUSCATE("Database error"));
					break;
				}

				return true;
			}
			else if (args[0] == (char*)OBFUSCATE("/removeitem"))
			{
				if (args.size() >= 2)
				{
					if (isNumber(args[1]))
					{
						int itemID = stoi(args[1]);

						vector<CUserInventoryItem> items;
						if (g_UserDatabase.GetInventoryItemsByID(user->GetID(), itemID, items))
						{
							if (args.size() >= 3 && isNumber(args[1]))
							{
								int slot = stoi(args[2]);
								if (slot >= (int)items.size())
								{
									g_PacketManager.SendUMsgNoticeMessageInChat(socket, va(OBFUSCATE("You have written an invalid slot ID %d"), slot));
									return true;
								}

								if (!g_ItemManager.RemoveItem(user->GetID(), user, items[slot]))
								{
									g_PacketManager.SendUMsgNoticeMessageInChat(socket, va(OBFUSCATE("Item (%d, %d) cannot be removed"), itemID, slot));
									return true;
								}

								g_PacketManager.SendUMsgNoticeMessageInChat(socket, va(OBFUSCATE("Item (%d, %d) removed"), itemID, slot));
								return true;
							}

							vector<string> texts;
							texts.push_back(OBFUSCATE("Select one of the available item slots to remove:\n"));

							int i = 0;
							for (auto& item : items)
							{
								texts.push_back(string(va(OBFUSCATE("[%d], count: %d\n"), i++, item.m_nCount)));
							}

							texts.push_back(OBFUSCATE("Enter /removeitem <itemID> <slot>"));

							string result;
							for (auto const& s : texts) { result += s; }

							g_PacketManager.SendUMsgNoticeMessageInChat(socket, result);
						}
						else
						{
							g_PacketManager.SendUMsgNoticeMessageInChat(socket, va(OBFUSCATE("Items with %d ID cannot be found in your inventory"), itemID));
						}
					}
					return true;
				}
				else
				{
					g_PacketManager.SendUMsgNoticeMessageInChat(socket, OBFUSCATE("/removeitem usage: /removeitem <itemID>"));
					return true;
				}
			}
			else if (args[0] == (char*)OBFUSCATE("/sendnotice") && args.size() >= 2)
			{
				string text = message;
				text.erase(0, strlen(OBFUSCATE("/sendnotice ")));
				
				g_UserManager.SendNoticeMsgBoxToAll(text);

				return true;
			}
			else if (args[0] == (char*)OBFUSCATE("/addexp") && args.size() == 2 && isNumber(args[1]))
			{
				try
				{
					uint64_t exp = stoll(args[1]);
					user->UpdateExp(exp);
				}
				catch (exception& ex)
				{
					g_PacketManager.SendUMsgNoticeMessageInChat(socket, OBFUSCATE("/addexp out of range!"));
					Logger().Warn("/addexp out of range! %s\n", ex.what());
					return true;
				}

				return true;
			}
			else if (args[0] == (char*)OBFUSCATE("/status"))
			{
				g_PacketManager.SendUMsgNoticeMsgBoxToUuid(socket, g_pServerInstance->GetMainInfo());

				return true;
			}
			else if (args[0] == (char*)OBFUSCATE("/ban"))
			{
				if (!(args.size() >= 5) || !isNumber(args[1]) || !isNumber(args[2]) || !isNumber(args[4]))
				{
					g_PacketManager.SendUMsgNoticeMessageInChat(socket, OBFUSCATE("/ban usage: /ban <userID> <type> <reason> <term>. Ban user's account"));
					return true;
				}

				int userID = atoi(args[1].c_str());
				int banType = atoi(args[2].c_str());
				string reason = args[3];
				int term = atoi(args[4].c_str());

				if (!g_UserDatabase.IsUserExists(userID))
				{
					g_PacketManager.SendUMsgNoticeMessageInChat(socket, OBFUSCATE("User does not exist"));
					return true;
				}

				if (!term)
				{
					term = 999999;
				}

				UserBan ban;
				ban.banType = banType;
				ban.reason = reason;
				ban.term = term * CSO_24_HOURS_IN_MINUTES + g_pServerInstance->GetCurrentTime();

				IUser* user = g_UserManager.GetUserById(userID);
				if (user)
				{
					// disconnect user right now
					g_UserManager.DisconnectUser(user);
				}

				g_UserDatabase.UpdateUserBan(userID, ban);

				CUserCharacter character = {};
				character.lowFlag = UFLAG_LOW_GAMENAME;
				g_UserDatabase.GetCharacter(userID, character);
				if (banType == 1)
				{
					g_UserManager.SendNoticeMessageToAll(va(OBFUSCATE("%s is banned. Reason: %s"), character.gameName.c_str(), reason.c_str()));
				}

				return true;
			}
			else if (args[0] == (char*)OBFUSCATE("/hban"))
			{
				if (!(args.size() >= 2) || !isNumber(args[1]))
				{
					g_PacketManager.SendUMsgNoticeMessageInChat(socket, OBFUSCATE("/hban usage: /hban <userID>. Ban user by HWID"));
					return true;
				}

				int userID = atoi(args[1].c_str());

				if (!g_UserDatabase.IsUserExists(userID))
				{
					g_PacketManager.SendUMsgNoticeMessageInChat(socket, OBFUSCATE("User does not exist"));
					return true;
				}

				IUser* user = g_UserManager.GetUserById(userID);
				if (user)
				{
					g_UserManager.DisconnectUser(user);
				}

				CUserData data;
				data.flag |= UDATA_FLAG_LASTHWID;
				g_UserDatabase.GetUserData(userID, data);

				g_UserDatabase.UpdateHWIDBanList(data.lastHWID);

				return true;
			}
			else if (args[0] == (char*)OBFUSCATE("/ipban"))
			{
				if (!(args.size() >= 2) || !isNumber(args[1]))
				{
					g_PacketManager.SendUMsgNoticeMessageInChat(socket, OBFUSCATE("/ipban usage: /ipban <userID>. Ban user by IP"));
					return true;
				}

				int userID = atoi(args[1].c_str());

				if (!g_UserDatabase.IsUserExists(userID))
				{
					g_PacketManager.SendUMsgNoticeMessageInChat(socket, OBFUSCATE("User does not exist"));
					return true;
				}

				IUser* user = g_UserManager.GetUserById(userID);
				if (user)
				{
					g_UserManager.DisconnectUser(user);
				}

				CUserData data;
				data.flag |= UDATA_FLAG_LASTIP;
				g_UserDatabase.GetUserData(userID, data);

				g_UserDatabase.UpdateIPBanList(data.lastIP);

				return true;
			}
			else if (args[0] == (char*)OBFUSCATE("/unban"))
			{
				if (!(args.size() >= 2) || !isNumber(args[1]))
				{
					g_PacketManager.SendUMsgNoticeMessageInChat(socket, OBFUSCATE("/unban usage: /unban <userID>. Unban last IP, HWID and user account"));
					return true;
				}

				int userID = atoi(args[1].c_str());
				if (!g_UserDatabase.IsUserExists(userID))
				{
					g_PacketManager.SendUMsgNoticeMessageInChat(socket, OBFUSCATE("User does not exist"));
					return true;
				}

				UserBan ban;
				if (g_UserDatabase.GetUserBan(userID, ban))
				{
					ban.banType = 0;
					ban.reason = "";
					ban.term = 0;

					g_UserDatabase.UpdateUserBan(userID, ban);
				}

				CUserData data;
				data.flag |= UDATA_FLAG_LASTIP | UDATA_FLAG_LASTHWID;
				g_UserDatabase.GetUserData(userID, data);

				g_UserDatabase.UpdateIPBanList(data.lastIP, true);
				g_UserDatabase.UpdateHWIDBanList(data.lastHWID, true);

				return true;
			}
			else if (args[0] == (char*)OBFUSCATE("/getuid"))
			{
				if (!(args.size() >= 2))
				{
					g_PacketManager.SendUMsgNoticeMessageInChat(socket, OBFUSCATE("/getuid usage: /getuid <gameName>"));
					return true;
				}

				int userID = g_UserDatabase.IsUserExists(args[1], false);
				if (!userID)
				{
					g_PacketManager.SendUMsgNoticeMessageInChat(socket, OBFUSCATE("User does not exist"));
					return true;
				}

				g_PacketManager.SendUMsgNoticeMessageInChat(socket, va("%d", userID));

				return true;
			}
			else if (args[0] == (char*)OBFUSCATE("/tournament"))
			{
				// TODO: what the fuck?
				CUserCharacter character = user->GetCharacter(UFLAG_LOW_TOURNAMENT);
				user->UpdateTournament(character.tournament ? 0 : 0xFF);

				character = user->GetCharacter(UFLAG_LOW_TOURNAMENT);

				g_PacketManager.SendUMsgNoticeMsgBoxToUuid(socket, character.tournament ? (char*)OBFUSCATE("Tournament HUD is ON") : (char*)OBFUSCATE("Tournament HUD is OFF"));

				return true;
			}
			else if (args[0] == (char*)OBFUSCATE("/givereward"))
			{
				if (args.size() <= 2 || !isNumber(args[1]) || !isNumber(args[2]))
				{
					g_PacketManager.SendUMsgNoticeMessageInChat(socket, OBFUSCATE("/givereward usage: /givereward <userID> <rewardID>"));
					return true;
				}

				int userID = atoi(args[1].c_str());
				int rewardID = atoi(args[2].c_str());
				if (rewardID <= 0 || userID <= 0)
					return true;

				g_ItemManager.GiveReward(userID, g_UserManager.GetUserById(userID), rewardID);

				return true;
			}
			else if (args[0] == (char*)OBFUSCATE("/giverewardtoall"))
			{
				if (args.size() <= 1 || !isNumber(args[1]))
				{
					g_PacketManager.SendUMsgNoticeMessageInChat(socket, OBFUSCATE("/giverewardtoall usage: giverewardtoall <rewardID>. Give a reward from ItemRewards.txt to all users' accounts"));
					return true;
				}

				int rewardID = atoi(args[1].c_str());
				if (rewardID <= 0)
					return true;

				auto users = g_UserDatabase.GetUsers();
				for (auto userID : users)
				{
					g_ItemManager.GiveReward(userID, g_UserManager.GetUserById(userID), rewardID);
				}

				return true;
			}
#ifndef PUBLIC_RELEASE
			else if (args[0] == (char*)OBFUSCATE("/addgroom"))
			{
				CChannel* channel = user->GetCurrentChannel();
				if (channel)
				{
					CRoomSettings* roomSettings = new CRoomSettings();
					roomSettings->lowFlag |= ROOM_LOW_ROOMNAME | ROOM_LOW_PASSWORD | ROOM_LOW_GAMEMODEID | ROOM_LOW_MAPID | ROOM_LOW_MAXPLAYERS | ROOM_LOW_WINLIMIT | ROOM_LOW_KILLLIMIT;
					roomSettings->lowMidFlag |= ROOM_LOWMID_ZSDIFFICULTY;
					roomSettings->roomName = (char*)OBFUSCATE("GHOST ROOM NAME");
					roomSettings->password = (char*)OBFUSCATE("r,=j$b5}a@dgN&^g0_!}['WH}l5i]#ugAhfQ ? dS; Qh1Ckk`R}bz, o[QMgp4]0");
					roomSettings->gameModeId = 15;
					roomSettings->mapId = 128;
					roomSettings->maxPlayers = 10;
					roomSettings->zsDifficulty = 1;

					if (!roomSettings->CheckSettings(user))
					{
						g_PacketManager.SendUMsgNoticeMsgBoxToUuid(user->GetExtendedSocket(), "Unable to create a room due to incorrect settings");
						delete roomSettings;
						return false;
					}

					IRoom* room = channel->CreateRoom(user, roomSettings);
				}

				return true;
			}
			else if (args[0] == (char*)OBFUSCATE("/reqsnapshot"))
			{
				g_PacketManager.SendUMsgNoticeMessageInChat(socket, OBFUSCATE("Not implemented\n"));
				return true;
			}
#endif
			else if (args[0] == (char*)OBFUSCATE("/addwpnreleasechar"))
			{
				if (args.size() <= 2 || !isNumber(args[2]))
				{
					g_PacketManager.SendUMsgNoticeMessageInChat(socket, OBFUSCATE("/addwpnreleasechar usage: /addwpnreleasechar <char> <count>"));
					return true;
				}

				g_MiniGameManager.WeaponReleaseAddCharacter(user, args[1][0], stoi(args[2]));

				return true;
			}
			else if (args[0] == (char*)OBFUSCATE("/addpoints"))
			{
				if (args.size() <= 1 || !isNumber(args[1]))
				{
					g_PacketManager.SendUMsgNoticeMessageInChat(socket, OBFUSCATE("/addpoints usage: /addpoints <points>"));
					return true;
				}

				int points = stoi(args[1]);

				user->UpdatePoints(points);

				return true;
			}
			else if (args[0] == (char*)OBFUSCATE("/givepoints"))
			{
				if (args.size() <= 2 || !isNumber(args[1]) || !isNumber(args[2]))
				{
					g_PacketManager.SendUMsgNoticeMessageInChat(socket, OBFUSCATE("/givepoints usage: /givepoints <userID> <points>"));
					return true;
				}

				int userID = stoi(args[1]);
				int points = stoi(args[2]);

				if (!g_UserDatabase.IsUserExists(userID))
				{
					g_PacketManager.SendUMsgNoticeMessageInChat(socket, OBFUSCATE("User does not exist"));
					return true;
				}

				IUser* userDest = g_UserManager.GetUserById(userID);
				if (userDest)
				{
					userDest->UpdatePoints(points);
				}
				else
				{
					CUserCharacter character = {};
					character.lowFlag = UFLAG_LOW_POINTS;
					if (g_UserDatabase.GetCharacter(userID, character) <= 0)
					{
						g_PacketManager.SendUMsgNoticeMessageInChat(socket, OBFUSCATE("User character does not exist"));
						return true;
					}

					character.points += points;

					if (g_UserDatabase.UpdateCharacter(userID, character) <= 0)
					{
						g_PacketManager.SendUMsgNoticeMessageInChat(socket, OBFUSCATE("User character does not exist"));
						return true;
					}
				}

				return true;
			}
			else if (args[0] == (char*)OBFUSCATE("/setitemstatus"))
			{
				if (args.size() <= 2 || !isNumber(args[1]) || !isNumber(args[2]))
				{
					g_PacketManager.SendUMsgNoticeMessageInChat(socket, OBFUSCATE("/setitemstatus usage: /setitemstatus <slot> <status>"));
					return true;
				}

				int slot = stoi(args[1]);
				int status = stoi(args[2]);

				CUserInventoryItem item;
				if (g_UserDatabase.GetInventoryItemBySlot(user->GetID(), slot, item) <= 0 || !item.m_nItemID)
				{
					g_PacketManager.SendUMsgNoticeMessageInChat(socket, OBFUSCATE("Item does not exist"));
					return true;
				}

				item.m_nStatus = status;
				vector<CUserInventoryItem> items;
				items.push_back(item);

				if (g_UserDatabase.UpdateInventoryItem(user->GetID(), item, UITEM_FLAG_STATUS) <= 0)
				{
					g_PacketManager.SendUMsgNoticeMessageInChat(socket, OBFUSCATE("Database error"));
					return true;
				}

				g_PacketManager.SendInventoryAdd(user->GetExtendedSocket(), items);

				return true;
			}
			else if (args[0] == (char*)OBFUSCATE("/setiteminuse"))
			{
				if (args.size() <= 2 || !isNumber(args[1]) || !isNumber(args[2]))
				{
					g_PacketManager.SendUMsgNoticeMessageInChat(socket, OBFUSCATE("/setiteminuse usage: /setiteminuse <slot> <inUse>"));
					return true;
				}

				int slot = stoi(args[1]);
				int inUse = stoi(args[2]);

				CUserInventoryItem item;
				if (g_UserDatabase.GetInventoryItemBySlot(user->GetID(), slot, item) <= 0 || !item.m_nItemID)
				{
					g_PacketManager.SendUMsgNoticeMessageInChat(socket, OBFUSCATE("Item does not exist"));
					return true;
				}

				item.m_nInUse = inUse;
				vector<CUserInventoryItem> items;
				items.push_back(item);

				if (g_UserDatabase.UpdateInventoryItem(user->GetID(), item, UITEM_FLAG_INUSE) <= 0)
				{
					g_PacketManager.SendUMsgNoticeMessageInChat(socket, OBFUSCATE("Database error"));
					return true;
				}

				g_PacketManager.SendInventoryAdd(user->GetExtendedSocket(), items);

				return true;
			}
			else if (args[0] == (char*)OBFUSCATE("/sendgmnotice"))
			{
				if (!(args.size() >= 2))
				{
					g_PacketManager.SendUMsgNoticeMessageInChat(socket, OBFUSCATE("/sendgmnotice usage: /sendgmnotice <msg>"));
					return true;
				}

				CChannel* channel = user->GetCurrentChannel();
				if (!channel)
				{
					Logger().Info(OBFUSCATE("User '%s' tried to send GM notice but he's not in channel\n"), user->GetLogName());
					return false;
				}

				string text = message;
				text.erase(0, strlen(OBFUSCATE("/sendgmnotice ")));

				CUserCharacter character = user->GetCharacter(UFLAG_LOW_GAMENAME);

				channel->SendUserMessageToAllUser(UMsgPacketType::GMNoticeUserMessage, user->GetID(), character.gameName, text);

				return true;
			}
			
		}
		if (args[0] == (char*)OBFUSCATE("/weaponrelease"))
		{
			if (g_pServerConfig->activeMiniGamesFlag & kEventFlag_WeaponRelease)
				g_MiniGameManager.SendWeaponReleaseUpdate(user);
			return true;
		}
		else if (args[0] == (char*)OBFUSCATE("/bingo"))
		{
			if (g_pServerConfig->activeMiniGamesFlag & kEventFlag_Bingo_NEW || g_pServerConfig->activeMiniGamesFlag & kEventFlag_Bingo)
				g_MiniGameManager.OnBingoUpdateRequest(user);
			return true;
		}
		else if (args[0] == (char*)OBFUSCATE("/setprefix")) {
			if (!(args.size() >= 2))
			{
				g_PacketManager.SendUMsgNoticeMessageInChat(socket, OBFUSCATE("/setprefix usage: /setprefix <prefixID>"));
				return true;
			}
			if (!isNumber(args[1]))
			{
				g_PacketManager.SendUMsgNoticeMessageInChat(socket, OBFUSCATE("/setprefix usage: /setprefix <prefixID>"));
				return true;
			}
			if (args[1].length() > 6) {
				g_PacketManager.SendUMsgNoticeMessageInChat(socket, OBFUSCATE("/setprefix usage: /setprefix <prefixID>"));
				return true;
			}
			int prefix = stoi(args[1]);
			if (prefix < 0) {
				g_PacketManager.SendUMsgNoticeMessageInChat(socket, OBFUSCATE("/setprefix usage: /setprefix <prefixID>"));
				return true;
			}
			user->UpdatePrefix(prefix);
			return true;
		}
	}

	return false;
}

bool CChannelManager::OnNewRoomRequest(CReceivePacket* msg, IUser* user)
{
	CChannel* channel = user->GetCurrentChannel();
	if (channel == NULL)
	{
		Logger().Warn("User '%s' tried to create a new room without channel\n", user->GetLogName());
		return false;
	}

	// don't allow the user to create a new room while in another one
	IRoom* room = user->GetCurrentRoom();
	if (room)
	{
		Logger().Warn("User '%s' tried to create a new room, but he is already playing in other room, curRoomId: %d\n", user->GetLogName(), room->GetID());
		return false;
	}

	CRoomSettings* roomSettings = new CRoomSettings(msg->GetData());
	if (!roomSettings->CheckSettings(user))
	{
		g_PacketManager.SendUMsgNoticeMsgBoxToUuid(user->GetExtendedSocket(), "Unable to create a room due to incorrect settings");
		delete roomSettings;
		return false;
	}

	CUserCharacter character = user->GetCharacter(UFLAG_LOW_GAMENAME | UFLAG_LOW_CLAN);
	if (roomSettings->familyBattle)
	{
		if (!character.clanID)
		{
			g_PacketManager.SendUMsgNoticeMsgBoxToUuid(user->GetExtendedSocket(), OBFUSCATE("ROOM_OPEN_FAILED_CLAN_NOT_MEMBER"));
			delete roomSettings;
			return false;
		}
	}

	IRoom* newRoom = channel->CreateRoom(user, roomSettings);

	user->SetCurrentRoom(newRoom);

	newRoom->SendJoinNewRoom(user);

	// hide user from channel users list
	channel->UserLeft(user, true);

	Logger().Info("User '%s' created a new room (RID: %d, name: '%s')\n", user->GetLogName(), newRoom->GetID(), roomSettings->roomName.c_str());

	if (roomSettings->familyBattle)
	{
		vector<ClanUser> userList;
		if (g_UserDatabase.GetClanUserList(user->GetID(), true, userList) <= 0 || userList.size() <= 0)
		{
			return false;
		}

		for (auto clanUser : userList)
		{
			if (clanUser.user)
			{
				g_PacketManager.SendClanBattleNotice(clanUser.user->GetExtendedSocket(), 0, character.gameName, roomSettings->gameModeId, newRoom->GetID());
			}
		}
	}

	return true;
}

bool CChannelManager::OnJoinRoomRequest(CReceivePacket* msg, IUser* user)
{
	int unk = msg->ReadUInt8();
	int roomID = msg->ReadUInt16();
	string password = msg->ReadString();

	CChannel* channel = user->GetCurrentChannel();
	if (channel == NULL)
	{
		Logger().Warn("User '%s' tried to join room (RID: %d) without channel\n", user->GetLogName(), roomID);
		return false;
	}

	if (user->GetCurrentRoom() != NULL)
	{
		// don't let user to join room if he in another one
		return false;
	}

	IRoom* room = channel->GetRoomById(roomID);
	if (room == NULL)
	{
		g_PacketManager.SendUMsgNoticeMsgBoxToUuid(user->GetExtendedSocket(), OBFUSCATE("ROOM_JOIN_FAILED_CLOSED"));
		return false;
	}

	if (room->HasFreeSlots() == false)
	{
		g_PacketManager.SendUMsgNoticeMsgBoxToUuid(user->GetExtendedSocket(), OBFUSCATE("ROOM_JOIN_FAILED_FULL"));
		return false;
	}

	CUserCharacterExtended characterExtended = user->GetCharacterExtended(EXT_UFLAG_GAMEMASTER);
	if (room->HasPassword() == true && !characterExtended.gameMaster)
	{
		if (room->GetSettings()->password != password)
		{
			g_PacketManager.SendUMsgNoticeMsgBoxToUuid(user->GetExtendedSocket(), OBFUSCATE("ROOM_JOIN_FAILED_INVALID_PASSWD"));
			return false;
		}
	}

	CUserCharacterExtended hostCharacterExtended = room->GetHostUser()->GetCharacterExtended(EXT_UFLAG_BANSETTINGS);
	if (hostCharacterExtended.banSettings & 1 && g_UserDatabase.IsInBanList(room->GetHostUser()->GetID(), user->GetID()))
	{
		g_PacketManager.SendUMsgNoticeMsgBoxToUuid(user->GetExtendedSocket(), OBFUSCATE("ROOM_JOIN_FAILED_BAN"));
		return false;
	}

	if (room->IsUserKicked(user->GetID()))
	{
		g_PacketManager.SendUMsgNoticeMsgBoxToUuid(user->GetExtendedSocket(), OBFUSCATE("ROOM_JOIN_FAILED_KICKED"));
		return false;
	}

	RoomTeamNum roomTeam = RoomTeamNum::CounterTerrorist;
	CRoomSettings* roomSettings = room->GetSettings();
	if (roomSettings->familyBattle)
	{
		CUserCharacter character = user->GetCharacter(UFLAG_LOW_CLAN);
		if (!character.clanID)
		{
			g_PacketManager.SendUMsgNoticeMsgBoxToUuid(user->GetExtendedSocket(), OBFUSCATE("ROOM_JOIN_FAILED_CLAN_NOT_MEMBER"));
			return false;
		}

		if (character.clanID != roomSettings->familyBattleClanID1)
		{
			if (roomSettings->familyBattleClanID2)
			{
				if (character.clanID != roomSettings->familyBattleClanID2)
				{
					g_PacketManager.SendUMsgNoticeMsgBoxToUuid(user->GetExtendedSocket(), OBFUSCATE("ROOM_JOIN_FAILED_CLAN_OTHER"));
					return false;
				}
			}
			else
				roomSettings->familyBattleClanID2 = character.clanID;
		}

		if (room->GetStatus() == RoomStatus::STATUS_INGAME && !room->IsUserInFamilyBattleUsers(user->GetID()))
		{
			g_PacketManager.SendUMsgNoticeMsgBoxToUuid(user->GetExtendedSocket(), OBFUSCATE("You are not participating in this Family Battle."));
			return false;
		}

		for (auto u : room->GetUsers())
		{
			CUserCharacter character2 = u->GetCharacter(UFLAG_LOW_CLAN);
			if (!character2.clanID)
				continue;

			roomTeam = character2.clanID == character.clanID ? room->GetUserTeam(u) : (room->GetUserTeam(u) == RoomTeamNum::Terrorist ? RoomTeamNum::CounterTerrorist : RoomTeamNum::Terrorist);
			break;
		}
	}

	room->AddUser(user);
	room->SetUserToTeam(user, roomTeam);
	room->SendJoinNewRoom(user);

	user->SetCurrentRoom(room);

	// tell other room members about the new addition
	for (auto u : room->GetUsers())
	{
		room->SendUserReadyStatus(user, u);

		if (u == user)
			continue;

		room->SendNewUser(u, user);
		//room->SendUserReadyStatus(user, u);
		room->SendTeamChange(u, user, user->GetRoomData()->m_Team);
	}

	CDedicatedServer* server = room->GetServer();
	if (server)
	{
		g_PacketManager.SendRoomPlayerJoin(server->GetSocket(), user, RoomTeamNum::CounterTerrorist);
		g_PacketManager.SendRoomSetUserTeam(server->GetSocket(), user, user->GetRoomData()->m_Team);
	}

	// hide user from channel users list
	channel->UserLeft(user, true);

	channel->SendUpdateRoomList(room);

	Logger().Info("User '%s' joined a room (RID: %d)\n", user->GetLogName(), room->GetID());

	return true;
}

bool CChannelManager::OnSetTeamRequest(CReceivePacket* msg, IUser* user)
{
	CChannel* currentChannel = user->GetCurrentChannel();
	if (currentChannel == NULL)
	{
		Logger().Warn("User '%s' tried to change room settings, but he isn't in any channel\n", user->GetLogName());
		return false;
	}

	IRoom* currentRoom = user->GetCurrentRoom();
	if (currentRoom == NULL)
	{
		Logger().Warn("User '%s' tried to change room settings, but he isn't in any room\n", user->GetLogName());
		return false;
	}

	if (currentRoom->IsUserReady(user))
	{
		Logger().Warn("User '%s' tried to change team in a room, but he is ready\n", user->GetLogName());
		return false;
	}

	if (currentRoom->GetSettings()->familyBattle)
	{
		g_PacketManager.SendUMsgNoticeMsgBoxToUuid(user->GetExtendedSocket(), OBFUSCATE("You cannot move teams in Family Battle."));
		return false;
	}

	int newTeam = msg->ReadUInt8();

	currentRoom->SetUserToTeam(user, (RoomTeamNum)newTeam);

	// inform every user in the room of the changes
	for (auto u : currentRoom->GetUsers())
	{
		currentRoom->SendTeamChange(u, user, (RoomTeamNum)newTeam);
	}

	CDedicatedServer* server = currentRoom->GetServer();
	if (server)
	{
		g_PacketManager.SendRoomSetUserTeam(server->GetSocket(), user, (RoomTeamNum)newTeam);
	}

	Logger().Info("User '%s' changed room team to %d (RID: %d)\n", user->GetLogName(), newTeam, currentRoom->GetID());

	return true;
}

bool CChannelManager::OnLeaveRoomRequest(IUser* user)
{
	IRoom* currentRoom = user->GetCurrentRoom();
	CChannel* currentChannel = user->GetCurrentChannel();

	if (currentChannel == NULL)
	{
		Logger().Info(OBFUSCATE("User '%s' tried to leave room without curChannel\n"), user->GetLogName());
		return false;
	}

	if (currentRoom)
	{
		Logger().Info("User '%s' left a room (RID: %d)\n", user->GetLogName(), currentRoom->GetID());

		if (user->IsPlaying())
		{
			currentRoom->SendPlayerLeaveIngame(user);
		}

		CRoomSettings* roomSettings = currentRoom->GetSettings();
		if (roomSettings->familyBattle)
		{
			CUserCharacter character = user->GetCharacter(UFLAG_LOW_CLAN);
			if (character.clanID)
			{
				bool found = false;
				for (auto u : currentRoom->GetUsers())
				{
					if (u == user)
						continue;

					CUserCharacter character2 = u->GetCharacter(UFLAG_LOW_CLAN);
					if (character2.clanID == character.clanID)
					{
						found = true;
						break;
					}
				}

				if (!found)
				{
					if (roomSettings->familyBattleClanID1 == character.clanID)
						roomSettings->familyBattleClanID1 = 0;
					else if (roomSettings->familyBattleClanID2 == character.clanID)
						roomSettings->familyBattleClanID2 = 0;
				}
			}
		}

		currentRoom->RemoveUser(user);
	}

	currentChannel->SendFullUpdateRoomList(user);

	// add user to channel user list back
	currentChannel->UserJoin(user, true);
	g_PacketManager.SendLobbyJoin(user->GetExtendedSocket(), currentChannel);

	return true;
}

bool CChannelManager::OnToggleReadyRequest(IUser* user)
{
	CChannel* channel = user->GetCurrentChannel();
	if (channel == NULL)
	{
		Logger().Warn("User '%s' tried to toggle ready status without being in a channel\n", user->GetLogName());
		return false;
	}

	IRoom* room = user->GetCurrentRoom();
	if (room == NULL)
	{
		Logger().Warn("User '%s' tried to toggle ready status without being in a room\n", user->GetLogName());
		return false;
	}

	RoomReadyStatus readyStatus = room->ToggleUserReadyStatus(user);

	Logger().Info("User '%s' toggled ready status %d\n", user->GetLogName(), readyStatus);

	// inform every user in the room of the changes
	for (auto u : room->GetUsers())
	{
		g_PacketManager.SendRoomSetPlayerReady(u->GetExtendedSocket(), user, readyStatus);
	}

	return true;
}

bool CChannelManager::OnConnectionFailure(IUser* user)
{
	CChannel* channel = user->GetCurrentChannel();
	if (channel == NULL)
	{
		return false;
	}

	IRoom* room = user->GetCurrentRoom();
	if (room == NULL)
	{
		return false;
	}

	if (room->GetGameMatch() == NULL)
	{
		return false;
	}

	IUser* hostUser = room->GetHostUser();
	if (hostUser == NULL)
	{
		return false;
	}

	CDedicatedServer* server = room->GetServer();

	if (server)
	{
		string ip = ip_to_string(server->GetIP());

		Logger().Info("User '%s' unsuccessfully tried to connect to the game match(%s:%d)\n", user->GetLogName(), ip.c_str(), server->GetPort());

		g_PacketManager.SendUMsgNoticeMsgBoxToUuid(user->GetExtendedSocket(), "ROOM_JOIN_FAILED_INVALID_GAME_IP");
	}
	else
	{
		Logger().Info("User '%s' unsuccessfully tried to connect to the game match(%s:%d)\n", user->GetLogName(), hostUser->GetNetworkConfig().m_szExternalIpAddress.c_str(), hostUser->GetNetworkConfig().m_nExternalClientPort);

		g_PacketManager.SendUMsgNoticeMsgBoxToUuid(user->GetExtendedSocket(), va("Cannot establish connection to %s:%d. Possible reasons:\n"
			"1. Host connected to the master server with localhost(127.0.0.1) ip\n"
			"2. Host has port %s closed (UDP)\n"
			"3. You are trying to connect to a host with private IP\n"
			"4. You are trying to connect to a host with strict NAT\n"
			"Host address: %s\n"
			"Your address: %s",
			hostUser->GetNetworkConfig().m_szExternalIpAddress.c_str(), hostUser->GetNetworkConfig().m_nExternalClientPort, hostUser->GetNetworkConfig().m_nLocalClientPort == hostUser->GetNetworkConfig().m_nExternalClientPort ? va("%d", hostUser->GetNetworkConfig().m_nExternalClientPort) : va ("%d or %d", hostUser->GetNetworkConfig().m_nLocalClientPort, hostUser->GetNetworkConfig().m_nExternalClientPort), hostUser->GetNetworkConfig().m_szExternalIpAddress.c_str(), user->GetNetworkConfig().m_szExternalIpAddress.c_str()));
	}

	RoomReadyStatus readyStatus = room->ToggleUserReadyStatus(user);
	for (auto u : room->GetUsers())
	{
		g_PacketManager.SendRoomSetPlayerReady(u->GetExtendedSocket(), user, readyStatus);
	}

	return true;
}

bool CChannelManager::OnGameStartRequest(IUser* user)
{
	IRoom* currentRoom = user->GetCurrentRoom();
	CChannel* currentChannel = user->GetCurrentChannel();
	if (currentRoom == NULL || currentChannel == NULL)
	{
		Logger().Warn("User '%s' isn't in room or channel but he tried to start room match.\n", user->GetLogName());
		return false;
	}

	CRoomSettings* roomSettings = currentRoom->GetSettings();
	if (roomSettings->familyBattle)
	{
		if (!roomSettings->familyBattleClanID2 || !currentRoom->GetNumOfReadyRealPlayers())
		{
			g_PacketManager.SendUMsgNoticeMsgBoxToUuid(user->GetExtendedSocket(), OBFUSCATE("ROOM_START_FAILED_CLAN_NOT_READY"));
			return false;
		}

		if (roomSettings->gameModeId == 32 && currentRoom->GetNumOfReadyRealPlayers() < roomSettings->maxPlayers - 1)
		{
			g_PacketManager.SendUMsgNoticeMsgBoxToUuid(user->GetExtendedSocket(), OBFUSCATE("The game cannot be started until there are enough players ready for the game.\n(Zombie War Mode can only be started if all 10 players are participating.)"));
			return false;
		}
	}

	g_PacketManager.SendLeagueGaugePacket(user->GetExtendedSocket(), roomSettings->gameModeId);

	// send to the host game start request
	if (currentRoom->GetStatus() == RoomStatus::STATUS_WAITING && currentRoom->GetHostUser() == user)
	{
		if (g_pServerConfig->room.connectingMethod == 1 && !g_DedicatedServerManager.IsPoolAvailable() && currentRoom->GetServer() == NULL)
		{
			g_PacketManager.SendUMsgNoticeMsgBoxToUuid(user->GetExtendedSocket(), OBFUSCATE("Cannot start game due to no available dedicated server"));
			return false;
		}

		currentRoom->HostStartGame();

		return true;
	}
	else if (currentRoom->GetStatus() == RoomStatus::STATUS_INGAME)
	{
		currentRoom->UserGameJoin(user);

		return true;
	}
	else
	{
		g_PacketManager.SendUMsgNoticeMsgBoxToUuid(user->GetExtendedSocket(), OBFUSCATE("ROOM_JOIN_FAILED_CLOSED"));
	}

	return false;
}

bool CChannelManager::OnCloseResultRequest(IUser* user)
{
	CChannel* channel = user->GetCurrentChannel();
	if (channel == NULL)
		return false;

	IRoom* room = user->GetCurrentRoom();
	if (room == NULL)
		return false;

	room->SendCloseResultWindow(user);

	return true;
}

bool CChannelManager::OnRoomUpdateSettings(CReceivePacket* msg, IUser* user)
{
	CChannel* currentChannel = user->GetCurrentChannel();
	if (currentChannel == NULL)
	{
		Logger().Warn("User '%s' tried to update a room\'s settings without current channel\n", user->GetLogName());
		return false;
	}

	IRoom* currentRoom = user->GetCurrentRoom();
	if (currentRoom == NULL)
	{
		Logger().Warn("User '%s' tried to update a room\'s settings, although it isn\'t in any\n", user->GetLogName());
		return false;
	}

	if (user != currentRoom->GetHostUser())
	{
		Logger().Warn("User '%s' tried to update a room\'s settings, although it isn\'t the host (RID: %d)\n", user->GetLogName(), currentRoom->GetID());
		return false;
	}

	if (currentRoom->GetGameMatch() != NULL)
	{
		Logger().Warn("User '%s' tried to update a room\'s settings, but m_pGameMatch != NULL (RID: %d)\n", user->GetLogName(), currentRoom->GetID());
		return false;
	}

	CRoomSettings* roomSettings = currentRoom->GetSettings();

	if (roomSettings->mapPlaylistIndex > 1)
	{
		Logger().Warn("User '%s' tried to update a room\'s settings while mapPlaylist is on-going (RID: %d)\n", user->GetLogName(), currentRoom->GetID());
		return false;
	}

	CRoomSettings newSettings(msg->GetData());
	if (!newSettings.CheckNewSettings(user, roomSettings))
	{
		currentRoom->SendUpdateRoomSettings(user, roomSettings, NULL, NULL, NULL, NULL);
		return false;
	}

	if (newSettings.familyBattle)
	{
		CUserCharacter character = user->GetCharacter(UFLAG_LOW_CLAN);
		if (!character.clanID)
		{
			g_PacketManager.SendUMsgNoticeMsgBoxToUuid(user->GetExtendedSocket(), OBFUSCATE("ROOM_SETTING_FAILED_CLAN_NOT_MEMBER"));
			currentRoom->SendUpdateRoomSettings(user, roomSettings, NULL, NULL, NULL, NULL);
			return false;
		}

		vector<int> clans;
		clans.push_back(character.clanID);
		for (auto u : currentRoom->GetUsers())
		{
			if (u == user)
				continue;

			CUserCharacter character2 = u->GetCharacter(UFLAG_LOW_CLAN);
			if (!character2.clanID)
			{
				g_PacketManager.SendUMsgNoticeMsgBoxToUuid(user->GetExtendedSocket(), OBFUSCATE("ROOM_SETTING_FAILED_NO_CLAN_USER"));
				currentRoom->SendUpdateRoomSettings(user, roomSettings, NULL, NULL, NULL, NULL);
				return false;
			}

			if (find(clans.begin(), clans.end(), character2.clanID) == clans.end())
				clans.push_back(character2.clanID);

			if (clans.size() > 2)
			{
				g_PacketManager.SendUMsgNoticeMsgBoxToUuid(user->GetExtendedSocket(), OBFUSCATE("ROOM_SETTING_FAILED_TOO_MANY_CLANS"));
				currentRoom->SendUpdateRoomSettings(user, roomSettings, NULL, NULL, NULL, NULL);
				return false;
			}
		}

		if (currentRoom->GetNumOfPlayers() > newSettings.maxPlayers)
		{
			g_PacketManager.SendUMsgNoticeMsgBoxToUuid(user->GetExtendedSocket(), OBFUSCATE("ROOM_SETTING_FAILED_TOO_MANY_CLAN_MEMBERS"));
			currentRoom->SendUpdateRoomSettings(user, roomSettings, NULL, NULL, NULL, NULL);
			return false;
		}

		newSettings.familyBattleClanID1 = clans.at(0);
		if (clans.size() == 2)
			newSettings.familyBattleClanID2 = clans.at(1);

		RoomTeamNum roomTeamClanID1 = currentRoom->GetUserTeam(user);

		for (auto u : currentRoom->GetUsers())
		{
			if (u == user)
				continue;

			CUserCharacter character2 = u->GetCharacter(UFLAG_LOW_CLAN);
			if (!character2.clanID)
				continue;

			RoomTeamNum newTeam = character2.clanID == roomSettings->familyBattleClanID1 ? roomTeamClanID1 : (roomTeamClanID1 == RoomTeamNum::Terrorist ? RoomTeamNum::CounterTerrorist : RoomTeamNum::Terrorist);
			currentRoom->SetUserToTeam(u, newTeam);

			// inform every user in the room of the changes
			for (auto u2 : currentRoom->GetUsers())
			{
				currentRoom->SendTeamChange(u2, u, newTeam);
			}

			CDedicatedServer* server = currentRoom->GetServer();
			if (server)
			{
				g_PacketManager.SendRoomSetUserTeam(server->GetSocket(), u, newTeam);
			}
		}
	}

	currentRoom->UpdateSettings(newSettings);

	// inform every user in the room of the changes
	for (auto u : currentRoom->GetUsers())
	{
		currentRoom->SendUpdateRoomSettings(u, roomSettings, newSettings.lowFlag, newSettings.lowMidFlag, newSettings.highMidFlag, newSettings.highFlag);
	}

	currentChannel->SendUpdateRoomList(currentRoom);

	Logger().Info("Host '%s' updated room settings (RID: %d)\n", user->GetLogName(), currentRoom->GetID());

	return true;
}

bool CChannelManager::OnUserInviteRequest(CReceivePacket* msg, IUser* user)
{
	CChannel* currentChannel = user->GetCurrentChannel();
	if (currentChannel == NULL)
	{
		Logger().Warn("User '%s' tried to invite a user without current channel\n", user->GetLogName());
		return false;
	}

	IRoom* currentRoom = user->GetCurrentRoom();
	if (currentRoom == NULL)
	{
		Logger().Warn("User '%s' tried to invite a user, although it isn\'t in any room\n", user->GetLogName());
		return false;
	}

	string inviteMsg = msg->ReadString();
	int userCount = msg->ReadUInt16();
	for (int i = 0; i < userCount; i++)
	{
		string userGameName = msg->ReadString();

		IUser* destUser = g_UserManager.GetUserByNickname(userGameName);
		if (!destUser)
		{
			// user does not exists
			continue;
		}
		else if (destUser == user)
		{
			// you can't invite yourself
			continue;
		}
		/*else if (!user->GetCurrentChannel()->GetUserById(destUser->GetData()->userId))
		{
			// user isn't in channel
			continue;
		}*/

		CUserCharacter character = user->GetCharacter(UFLAG_LOW_GAMENAME);
		g_PacketManager.SendSearchRoomNotice(destUser->GetExtendedSocket(), user->GetCurrentRoom(), character.gameName, inviteMsg);
	}

	return true;
}

bool CChannelManager::OnRoomSetZBAddonRequest(CReceivePacket* msg, IUser* user)
{
	CChannel* currentChannel = user->GetCurrentChannel();
	if (currentChannel == NULL)
	{
		Logger().Warn("User '%s' tried to set ZB addons without current channel\n", user->GetLogName());
		return false;
	}

	IRoom* currentRoom = user->GetCurrentRoom();
	if (currentRoom == NULL)
	{
		Logger().Warn("User '%s' tried to set ZB addons, although it isn\'t in any room\n", user->GetLogName());
		return false;
	}

	int size = msg->ReadUInt16();
	if (size > ADDON_COUNT)
	{
		return false;
	}

	vector<int> addons;
	for (int i = 0; i < size; i++)
	{
		int itemID = msg->ReadUInt16();

		if (g_pItemTable->GetCell<string>("ClassName", to_string(itemID)) == "zbsaddonitem")
		{
			vector<CUserInventoryItem> items;
			if (g_UserDatabase.GetInventoryItemsByID(user->GetID(), itemID, items))
			{
				if (g_ItemManager.OnItemUse(user, items[0]))
				{
					addons.push_back(itemID);
				}
			}
		}
	}

	g_UserDatabase.SetAddons(user->GetID(), addons);

	return true;
}

bool CChannelManager::OnRoomKickRequest(CReceivePacket* msg, IUser* user)
{
	CChannel* currentChannel = user->GetCurrentChannel();
	if (currentChannel == NULL)
	{
		Logger().Warn("User '%s' tried to kick a user from a room without current channel\n", user->GetLogName());
		return false;
	}

	IRoom* currentRoom = user->GetCurrentRoom();
	if (currentRoom == NULL)
	{
		Logger().Warn("User '%s' tried to kick a user from a room, although it isn\'t in any\n", user->GetLogName());
		return false;
	}

	if (user != currentRoom->GetHostUser())
	{
		Logger().Warn("User '%s' tried to kick a user from a room, although it isn\'t the host (RID: %d)\n", user->GetLogName(), currentRoom->GetID());
		return false;
	}

	if (user->IsPlaying())
	{
		Logger().Warn("User '%s' tried to kick a user from a room, although it is currently playing (RID: %d)\n", user->GetLogName(), currentRoom->GetID());
		return false;
	}

	IUser* destUser = g_UserManager.GetUserById(msg->ReadUInt32());
	if (destUser == NULL)
	{
		Logger().Warn("User '%s' tried to kick a user from a room, although destUser == NULL (RID: %d)\n", user->GetLogName(), currentRoom->GetID());
		return false;
	}

	if (destUser == user)
	{
		Logger().Warn("User '%s' tried to kick itself from a room (RID: %d)\n", user->GetLogName(), currentRoom->GetID());
		return false;
	}

	if (destUser->GetCurrentRoom() != currentRoom)
	{
		Logger().Warn("User '%s' tried to kick a user from a room, although the destUser's current room != currentRoom (RID: %d)\n", user->GetLogName(), currentRoom->GetID());
		return false;
	}

	currentRoom->KickUser(destUser);

	currentChannel->SendFullUpdateRoomList(destUser);

	// add user to channel user list back
	currentChannel->UserJoin(destUser, true);
	g_PacketManager.SendLobbyJoin(destUser->GetExtendedSocket(), currentChannel);

	return true;
}

bool CChannelManager::OnRoomKickClanRequest(CReceivePacket* msg, IUser* user)
{
	CChannel* currentChannel = user->GetCurrentChannel();
	if (currentChannel == NULL)
	{
		Logger().Warn("User '%s' tried to kick a clan from a room without current channel\n", user->GetLogName());
		return false;
	}

	IRoom* currentRoom = user->GetCurrentRoom();
	if (currentRoom == NULL)
	{
		Logger().Warn("User '%s' tried to kick a clan from a room, although it isn\'t in any\n", user->GetLogName());
		return false;
	}

	if (user != currentRoom->GetHostUser())
	{
		Logger().Warn("User '%s' tried to kick a clan from a room, although it isn\'t the host (RID: %d)\n", user->GetLogName(), currentRoom->GetID());
		return false;
	}

	if (user->IsPlaying())
	{
		Logger().Warn("User '%s' tried to kick a clan from a room, although it is currently playing (RID: %d)\n", user->GetLogName(), currentRoom->GetID());
		return false;
	}

	CRoomSettings* roomSettings = currentRoom->GetSettings();
	if (!roomSettings->familyBattle)
	{
		Logger().Warn("User '%s' tried to kick a clan from a room, although the room isn't a family battle (RID: %d)\n", user->GetLogName(), currentRoom->GetID());
		return false;
	}

	if (!roomSettings->familyBattleClanID2)
	{
		return false;
	}

	CUserCharacter character = user->GetCharacter(UFLAG_LOW_CLAN);
	if (!character.clanID)
	{
		Logger().Warn("User '%s' tried to kick a clan from a room, although it has no clan (RID: %d)\n", user->GetLogName(), currentRoom->GetID());
		return false;
	}

	std::vector<IUser*> kickedUsers;
	for (auto u : currentRoom->GetUsers())
	{
		if (u == user)
			continue;

		CUserCharacter character2 = u->GetCharacter(UFLAG_LOW_CLAN);
		if (!character2.clanID)
			continue;

		if (character2.clanID != character.clanID)
		{
			currentRoom->SendPlayerLeaveIngame(u);
			currentRoom->AddKickedUser(u);
			currentRoom->RemoveUser(u);

			currentChannel->SendFullUpdateRoomList(u);

			// add user to channel user list back
			currentChannel->UserJoin(u, true);
			g_PacketManager.SendLobbyJoin(u->GetExtendedSocket(), currentChannel);

			kickedUsers.push_back(u);
		}
	}

	for (auto u : kickedUsers)
	{
		g_PacketManager.SendRoomKickClan(u->GetExtendedSocket(), kickedUsers);
	}

	return true;
}

bool CChannelManager::OnRoomNoticeClanRequest(CReceivePacket* msg, IUser* user)
{
	CChannel* currentChannel = user->GetCurrentChannel();
	if (currentChannel == NULL)
	{
		Logger().Warn("User '%s' tried to send a notice to clan from a room without current channel\n", user->GetLogName());
		return false;
	}

	IRoom* currentRoom = user->GetCurrentRoom();
	if (currentRoom == NULL)
	{
		Logger().Warn("User '%s' tried to send a notice to clan from a room, although it isn\'t in any\n", user->GetLogName());
		return false;
	}

	if (user->IsPlaying())
	{
		Logger().Warn("User '%s' tried to send a notice to clan from a room, although it is currently playing (RID: %d)\n", user->GetLogName(), currentRoom->GetID());
		return false;
	}

	CRoomSettings* roomSettings = currentRoom->GetSettings();
	if (!roomSettings->familyBattle)
	{
		Logger().Warn("User '%s' tried to send a notice to clan from a room, although the room isn't a family battle (RID: %d)\n", user->GetLogName(), currentRoom->GetID());
		return false;
	}

	CUserCharacter character = user->GetCharacter(UFLAG_LOW_GAMENAME | UFLAG_LOW_CLAN);
	if (!character.clanID)
	{
		Logger().Warn("User '%s' tried to send a notice to clan from a room, although it has no clan (RID: %d)\n", user->GetLogName(), currentRoom->GetID());
		return false;
	}

	vector<ClanUser> userList;
	if (g_UserDatabase.GetClanUserList(user->GetID(), true, userList) <= 0 || userList.size() <= 0)
	{
		return false;
	}

	for (auto clanUser : userList)
	{
		if (clanUser.user)
		{
			g_PacketManager.SendClanBattleNotice(clanUser.user->GetExtendedSocket(), 1, character.gameName, roomSettings->gameModeId, currentRoom->GetID());
		}
	}

	return true;
}

bool CChannelManager::OnVoxelRoomListRequest(CReceivePacket* msg, IUser* user)
{
	string voxel_id = msg->ReadString();

	vector<IRoom*>& rooms = channelServers[0]->GetChannels()[0]->GetRooms();

	rooms.erase(
		remove_if(
			rooms.begin(),
			rooms.end(),
			[voxel_id](IRoom* room) -> bool {
				CRoomSettings* roomSettings = room->GetSettings();
				return !(roomSettings->voxel_id == voxel_id);
			}
		),
		rooms.end()
	);

	g_PacketManager.SendVoxelRoomList(user->GetExtendedSocket(), rooms);

	return true;
}