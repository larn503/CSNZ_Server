cmake_minimum_required(VERSION 3.20)
set(PROJECTNAME "CSO Server")

project(PROJECTNAME CXX)

option(CSO_SERVER_USE_GUI "Use QT GUI" OFF)
option(CSO_SERVER_PUBLIC "Public build" OFF)
option(CSO_SERVER_PROTECTION "Enable protection" OFF)
option(CSO_SERVER_DBSQLITE "SQLite database" ON)

add_subdirectory(Thirdparty)

if (CSO_SERVER_USE_GUI)
	add_subdirectory(GUI)
endif()

add_executable(PROJECTNAME)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo" CACHE STRING "" FORCE)

target_link_libraries(PROJECTNAME PRIVATE keyvalues)
target_link_libraries(PROJECTNAME PRIVATE nlohmann_json)
target_link_libraries(PROJECTNAME PRIVATE wolfssl)
if (CSO_SERVER_USE_GUI)
	target_link_libraries(PROJECTNAME PRIVATE gui)
	qt6_disable_unicode_defines(PROJECTNAME)
	add_definitions(-DUSE_GUI)
endif()

target_sources(PROJECTNAME PRIVATE "main.cpp")
target_sources(PROJECTNAME PRIVATE "ServerInstance.cpp")
target_sources(PROJECTNAME PRIVATE "ServerConfig.cpp")
target_sources(PROJECTNAME PRIVATE "Buffer.cpp")
target_sources(PROJECTNAME PRIVATE "BuildNum.cpp")
target_sources(PROJECTNAME PRIVATE "Console.cpp")
target_sources(PROJECTNAME PRIVATE "Thirdparty/XZip.cpp")
target_sources(PROJECTNAME PRIVATE "User.cpp")
target_sources(PROJECTNAME PRIVATE "UserInventoryItem.cpp")
target_sources(PROJECTNAME PRIVATE "UserLoadout.cpp")
target_sources(PROJECTNAME PRIVATE "Room.cpp")
target_sources(PROJECTNAME PRIVATE "RoomSettings.cpp")
target_sources(PROJECTNAME PRIVATE "GameMatch.cpp")
target_sources(PROJECTNAME PRIVATE "Channel.cpp")
target_sources(PROJECTNAME PRIVATE "ChannelServer.cpp")
target_sources(PROJECTNAME PRIVATE "CSVTable.cpp")
target_sources(PROJECTNAME PRIVATE "Quest.cpp")
target_sources(PROJECTNAME PRIVATE "QuestEvent.cpp")

target_sources(PROJECTNAME PRIVATE "UserManager.cpp")
if (CSO_SERVER_DBSQLITE)
	target_sources(PROJECTNAME PRIVATE "UserDatabase_SQLite.cpp")
endif()
target_sources(PROJECTNAME PRIVATE "ChannelManager.cpp")
target_sources(PROJECTNAME PRIVATE "PacketManager.cpp")
target_sources(PROJECTNAME PRIVATE "ShopManager.cpp")
target_sources(PROJECTNAME PRIVATE "ItemManager.cpp")
target_sources(PROJECTNAME PRIVATE "LuckyItemManager.cpp")
target_sources(PROJECTNAME PRIVATE "HostManager.cpp")
target_sources(PROJECTNAME PRIVATE "DedicatedServerManager.cpp")
target_sources(PROJECTNAME PRIVATE "QuestManager.cpp")
target_sources(PROJECTNAME PRIVATE "MiniGameManager.cpp")
target_sources(PROJECTNAME PRIVATE "ClanManager.cpp")
target_sources(PROJECTNAME PRIVATE "RankManager.cpp")

target_sources(PROJECTNAME PRIVATE "Network.cpp")
target_sources(PROJECTNAME PRIVATE "ExtendedSocket.cpp")
target_sources(PROJECTNAME PRIVATE "ReceivePacket.cpp")
target_sources(PROJECTNAME PRIVATE "SendPacket.cpp")
target_sources(PROJECTNAME PRIVATE "PacketHelper_FullUserInfo.cpp")
target_sources(PROJECTNAME PRIVATE "Manager.cpp")
if (WIN32)
	target_sources(PROJECTNAME PRIVATE "CrashDump.cpp")
endif()

target_sources(PROJECTNAME PRIVATE "Common/Thread.cpp")
target_sources(PROJECTNAME PRIVATE "Common/Utils.cpp")

target_include_directories(PROJECTNAME PUBLIC
	"./"
	"Thirdparty"
	"Thirdparty/json/include"
	"Thirdparty/SQLiteCpp/include"
	"Thirdparty/KeyValues/include"
	"Thirdparty/wolfssl"
	"Common"
	"Interfaces"
)

target_precompile_headers(PROJECTNAME PRIVATE main.h)

add_definitions(-D_CRT_SECURE_NO_WARNINGS)

if (CSO_SERVER_PROTECTION)
	add_definitions(-DPROTECTION)
endif()

if (CSO_SERVER_DBSQLITE)
	add_definitions(-DDB_SQLITE)
	target_link_libraries(PROJECTNAME PRIVATE SQLiteCpp sqlite3)
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
elseif (MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
else()
	set(CMAKE_CXX_STANDARD 17)
	set(CMAKE_CXX_STANDARD_REQUIRED True)
endif()

set(CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES}" CACHE STRING "Reset Default Build Config")

set_target_properties(PROJECTNAME PROPERTIES OUTPUT_NAME "CSNZ_Server")

set(EXECUTABLE_OUTPUT_PATH  ${PROJECT_SOURCE_DIR}/../bin)

# define max numbers of connected users by changing FD_SETSIZE
if (CSO_SERVER_PUBLIC)
	add_definitions(-DFD_SETSIZE=30)
	add_definitions(-DPUBLIC_RELEASE)
else()
	add_definitions(-DFD_SETSIZE=1024)
endif()

if (NOT WIN32)
	add_definitions(-DPOSIX=1)
endif()

if (MSVC)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:CONSOLE")

	set (CMAKE_C_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Od")
	set (CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -O1")
		
	set_target_properties(PROJECTNAME PROPERTIES LINK_FLAG_RELEASE "/WHOLEARCHIVE")
	
	target_link_libraries(PROJECTNAME PRIVATE ws2_32)
	target_link_libraries(PROJECTNAME PRIVATE Shlwapi)

	target_compile_options(PROJECTNAME PRIVATE /W3)
else()
	# don't treat some warnings as errors
	target_compile_options(PROJECTNAME PRIVATE -Wno-error=sign-compare -Wno-error=unused-variable -Wno-error=delete-non-virtual-dtor -Wno-error=unused-parameter -Wno-error=conversion-null -Wno-error=register -Wno-error=unused-value -Wno-error=reorder -Wno-error=attributes)
endif()