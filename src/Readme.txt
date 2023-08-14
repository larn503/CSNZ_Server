CSNZ Server
Originally based on code from CSO2 Server from Ochii

How to build:
git config --global core.autocrlf input
git clone https://github.com/JusicP/CSNZ_Server --recursive --depth 1

Windows:
Requirements:
1. Visual Studio 2019 compiler

Linux:
TODO

ServerConfig fields:
HostName - unused (def. "CSO Server")
Description - unused (def. "")
Port - TCP and UDP ports used to connect to the server (def. "30002")
TCPSendBufferSize - send buffer size for TCP (def. 131072)
MaxPlayers - the maximum number of players that can login the server (def. 100)
WelcomeMessage - the message that is displayed after logging in (def. "")
RestartOnCrash - restart the server on crash (def. false)
InventorySlotMax - the maximum number of user inventory slots user can use (def. 3000)
CheckClientBuild - checks client timestamp and launcher version (def. false)
AllowedClientTimestamp - allowed timestamp of client libraries (hw.dll, client.dll, mp.dll, cstrike-online.exe, gameui.dll, vgui2.dll)
AllowedLauncherVersion - allowed launcher version (def. 67)
DefaultUser (default values that apply to the user after registration):
GameMaster - game master (admin) privilege (def. false)
Level (def. 1)
Exp (def. 0)
Points (def. 0)
HonorPoints (def. 0)
PrefixID (def. 0)
Kills (def. 0)
Deaths (def. 0)
Battles (def. 0)
Win (def. 0)
PasswordBoxes (def. 0)
MileagePoints (def. 0)
DefaultItems - items that cannot be edited by user. (def. {175, 459, 460, 8079, 8080, 8138, 358, 136, 137, 25, 305, 138, 26, 306, 27,
		28, 252, 29, 30, 31, 200, 201, 214, 8222, 215, 216, 163, 8115, 390, 391, /*characters*/ 40, 41, 42, 43, 44, 49, 50, 51, 52, 53})
PseudoDefaultItems - the same as DefaultItems but user can do whatever they want with these items (except remove). (def. {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 161, 525})
!!! Changing PseudoDefaultItems only affects new users!
Loadouts (def. 0)
BuyMenu (def. 0)

Notices:


Database
The server uses SQLite to work with the database.
При запуске сервера запускается скрипт создания базы данных: Data/SQL/main.sql
в котором содержиться информация о версии базы данных и таблицах. Версия базы данных задается не только в main.sql, но так же и в
UserDatabase_SQLite.cpp через дефайн (ссылка на дефайн)//#define LAST_DB_VERSION 1//. Если версия базы данных отличается от LAST_DB_VERSION,
вызывается функция CUserDatabaseSQLite::UpgradeDatabase(int currentDatabaseVer), которая последовательно в цикле выполняет скрипты Data/SQL/Update_X.sql, где X - номер версии
от currentDatabaseVer до LAST_DB_VERSION.

Обновление базы данных (без потери данных пользователей)
Для обновления базы данных необходимо
1. Обновить дефайн LAST_DB_VERSION, увеличив значение на 1.
2. Создать скрипт Data/SQL/Update_X.sql, где X - это номер новой версии, куда вы вводите DDL/DML/DCL statements you need.

Обновление базы данных (с потерей данных пользователей)
Для обновления базы данных необходимо
1. Удалить UserDatabase.db3.
2. Обновить дефайн LAST_DB_VERSION, увеличив значение на 1.
3. Отредактировать скрипт Data/SQL/main.sql, при этом задав PRAGMA user_version значение из LAST_DB_VERSION.
