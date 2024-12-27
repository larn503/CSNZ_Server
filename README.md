# About
This is modified fork for JusicP's CSNZ server made for solo and online play with economic and progression. The main purpose of this project is to create a fully playable CSNZ(CSN) experience with replayability. If you just want to test and play with all new guns, i recommend to use [original repo for this](https://github.com/JusicP/CSNZ_Server).  
Still work in progress. Balance can change over time. If you find any server bugs or crashes, feel free to create issue.  
  
**Don't use it for commercial purposes.**  
  
**Only for 29.11.24 Build Game**. Go to [Download Section](#download-and-installation) to see how to install it.   


# Download and installation
You need to have a specific version of the client. Newest clients are not supported.
1. Download a supported client
    1. Run Steam and open a console. To open the console type `steam://open/console` in a browser url
    2. Type in the steam console: `download_depot 273110 273111 493173891445608967`
    3. Wait for a download. This can take a while
    4. Now you can find the game client in steam folder: `C:\Program Files (x86)\Steam\steamapps\content\app_273110`
2. Download the latest server release. [You can find it here](https://github.com/larn503/CSNZ_Server/releases)
3. Extract archive to any folder (Use 7Zip, WinRar or anything).
4. Put CSOLauncher.exe in client CSNZ\Bin folder. Optionally you can also put CSOHLDS.exe if you need it.
5. Now launch CSNZ_Server.exe from Server folder and CSOLauncher.exe to launch client.

If you don't want to use Steam to play game, you can replace steam_api.dll in CSNZ\Bin folder from release archive. [Credit to steamapi patch](https://gitlab.com/Mr_Goldberg/goldberg_emulator)  
You can find additional information from Docs folder.  
> [!IMPORTANT]
> Every player has GameMaster mode by default (basically they are admins). To change that go to the ServerConfig.json and find "DefaultUser" section and set "GameMaster" to false.

# Features
All game's features from the original server, plus:  
* Fully configured "Earn points and buy decoders" economic  
* Player progression  
* Tweaks for items and gameplay  
* Some bugfixes 

## Economic
Earn points ingame and waste them to buy Epic decoder in order to get new items and weapons! Epic decoder can give you other types of decoder such as:
* Rare decoder & class decoder - Low tier weapons & classes with costumes.
* Unique decoder - Mid tier weapons
* Premium class decoder - All available in game classes include transcendence classes.
* Transcendence premium decoder - Top tier weapons (All gold, buff classes and epics). Always unlimited
* Top 50 Unlimited decoder - All old CSNZ weapons up to 2017 year. Some of the items were not in that time, but still fit in this theme.
* Weapon Paint decoder - All paints, nameplates and sprays.
* Top 30 decoder - All items that can help you at Zombie Scenario.

The better decoder you have the better chance to get an unlimited item.  
Also, you can buy zombie costumes and paints in shop (and some other things).

## Progression
Don't expect to have all transcendence and epic weapons at your start. I tried to made it hard at beginning, so most of your time you are going to have low and mid tier weapons. But there is some tips:
1. Every Transcendence class give you +10% to points and exp (and they can stack!), so try to collect as much as you can to have a big bonus.
2. Sometimes you can find Premium Room Host item. It always gives you 50% to points and exp, even if you are not host(for players who play in this room and don't have that item will still get 10%).
3. All bots modes (AI original, Zombie Hero and others) have an additional xp bonus for a bot difficulty. If you feel very easy, try to increase the bot's difficulty. 
4. You can earn points from zombies and boxes in scenario depend on the difficulty.
5. Don't forget about points and experience coupons.
6. Also every 5 levels you can open your level box and get a lot of useful and exclusive things.

After some time you are going to get more and more points and it will be easier for you get everything you want.

## Additional tweaks
* Reworked Zombie Scenario.
    - Now you can get difficulty boxes at every successful completion of a ZS map. This can also give you honor points (depend on difficulty).
    - Zombies and bosses are now drops points (depend on difficulty).
    - Elites zombies are now giving addons for players who defeated them.
* Uncapped point and experience. You can get as many as you want in single game.
* /setprefix <prefixId> command to set any prefix you want.
* Removed any broken weapons from pool (that cause crashes or not working at all, like some new weapons.). Some of them still have broken modes, but they are still playable.
* Restored parts combination

## Bug fixes
* Fixed win and rounds calculation.
* Fixed ZS medkits and revive usage.
* And others that i forgot

## Future plans
* Sort all working maps and gamemodes, return old event maps.

# Building
Release has all files for client, you can find their source in their original repos. All my changes was done in server.  
[Client Source](https://github.com/JusicP/Launcher_CSNZ)  
[HLDS Source](https://github.com/SmilexGamer/HLDS_CSNZ)  

Currently only Windows and Linux are supported, other platforms not tested.

Clone a repository:
```
git clone https://github.com/larn503/CSNZ_Server
git submodule init
git submodule update --depth 1
```

### Windows
* [Visual Studio 2019 or newer](https://visualstudio.microsoft.com/thank-you-downloading-visual-studio/?sku=Community)
* [CMake](https://www.cmake.org/download/)
* [Qt 6.5.3 (optional)](https://www.qt.io/download-qt-installer)
  
**Set `QTDIR` environment variable: `<PathToQT>\Qt\6.5.3\msvc2019_64` if you want to use GUI.**

Open project folder in Visual Studio and wait for CMake cache generation.
Then click on Build -> Build All.

### Linux
* GCC compiler
* [CMake](https://www.cmake.org/download/)
* [Qt 6.5.3 (optional)](https://www.qt.io/download-qt-installer)

```
cd PathToRepo/CSNZ_Server/src
cmake -S . -B build
cmake --build build
```
